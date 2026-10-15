#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""hil_client.py — Verilator 逐周期闭环的 Python「AI 宿主」(方案1)

角色: 给哑执行器 hil.exe 施加真实时间轴 —— mission 数据、故障注入决策、逐周期
      观测, 全在这里, 形成真正的"环": 观测 -> 决策 -> 注入 -> 再观测。

内容:
  Gate A 等价性  A1 无故障 mission 输出与黄金一致
                 A2 永久卡死在有界时延内触发裁决器
                 A3 单次真单拍注入的脉冲分布(应 <= 少量, 与 vvp 口径一致)
  闭环实验       每批 N_CYC 拍: 读观测 -> 实时决定下一个注入(骑 WINDOW=32 棱边)
                 -> 统计"在线误触发率", 与离线 CSV 对照

用法:  python sim/hil/hil_client.py
"""
import subprocess
import sys
from pathlib import Path

EXE = Path(r"E:\work\hil_vl\obj_dir\hil.exe")
CWD = Path(r"E:\work\hil_vl")
N, M = 4, 8
P2A = {0: 0x1000, 1: 0x2000, 2: 0x0800, 3: -0x1000}


def s16(v):
    return v if v < 0x8000 else v - 0x10000


# ---- mission 数据 + 黄金输出（与 tb_phase2_closed_loop.sv 同口径）----
W = [[P2A[(r + c) % 4] for c in range(N)] for r in range(N)]
A = [[P2A[(m + r + 1) % 4] for r in range(N)] for m in range(M)]

WCODE = [W[r][c] for r in range(N) for c in range(N)]
ACODE = [sum(((A[m][r] & 0xFFFF) << (16 * r)) for r in range(N)) for m in range(M)]
EXP = []
for m in range(M):
    row = []
    for c in range(N):
        acc = sum(A[m][r] * W[r][c] for r in range(N)) >> 15
        row.append(max(-32768, min(32767, acc)))
    EXP.append(row)


def w_hex():
    v = 0
    for i, w in enumerate(WCODE):
        v |= (w & 0xFFFF) << (16 * i)
    return f"{v:064X}"


class Hil:
    def __init__(self):
        self.p = subprocess.Popen([str(EXE)], cwd=str(CWD), stdin=subprocess.PIPE,
                                  stdout=subprocess.PIPE, text=True, bufsize=1)

    def cmd(self, s):
        self.p.stdin.write(s + "\n")
        self.p.stdin.flush()
        while True:
            line = self.p.stdout.readline()
            if not line:
                raise RuntimeError("hil.exe 提前退出")
            line = line.strip()
            if line.startswith("%Warning"):
                continue
            return line

    def close(self):
        try:
            self.cmd("Q")
        finally:
            self.p.kill()

    # ---- 高层动作 ----
    def reset(self):
        assert self.cmd("R") == "K"

    def load(self):
        assert self.cmd(f"W {w_hex()}") == "K"

    def drive(self, a):
        assert self.cmd(f"D 1 {a & ((1<<64)-1):016X}") == "K"

    def idle(self):
        assert self.cmd("D 0 0000000000000000") == "K"

    def cyc1(self):
        f = self.cmd("C1").split()
        return {"err": int(f[1]), "trig": int(f[2]), "tcnt": int(f[3]),
                "busy": int(f[4]), "valid": int(f[5], 16), "c": int(f[6], 16)}

    def inj(self, model, row, col, val):   # 单拍真注入
        assert self.cmd(f"I {model} {row} {col} {val:X}") == "K"

    def injp(self, model, row, col, val):  # 永久卡死
        assert self.cmd(f"IP {model} {row} {col} {val:X}") == "K"

    def ral(self):
        assert self.cmd("RAL") == "K"


# ---------------------------------------------------------------------------
# Gate A 等价性
# ---------------------------------------------------------------------------
def mission_check(h):
    """A1: 无故障 mission, 逐拍采集 valid/输出, 与黄金比对。"""
    h.reset()
    h.load()
    got = [[None] * N for _ in range(M)]
    cnt = [0] * N
    for m in range(M):
        h.drive(ACODE[m])
        ob = h.cyc1()
        for c in range(N):
            if (ob["valid"] >> c) & 1:
                got[cnt[c]][c] = ob["c"] >> (16 * c) & 0xFFFF
                cnt[c] += 1
    for _ in range(20):        # 排空
        h.idle()
        ob = h.cyc1()
        for c in range(N):
            if (ob["valid"] >> c) & 1:
                if cnt[c] < M:
                    got[cnt[c]][c] = ob["c"] >> (16 * c) & 0xFFFF
                    cnt[c] += 1
    bad = 0
    for c in range(N):
        for m in range(min(cnt[c], M)):
            exp = EXP[m][c] & 0xFFFF
            if got[m][c] != exp:
                bad += 1
                if bad <= 5:
                    print(f"    [A1] 列{c} 第{m}拍 got={got[m][c]:04X} exp={exp:04X}")
    print(f"  A1 无故障 mission: 采集 {cnt} 拍, 不匹配 {bad} -> {'PASS' if bad == 0 else 'FAIL'}")
    return bad == 0


def perm_check(h, model, row, col):
    """A2: 永久卡死应在有界时延内触发。"""
    h.reset(); h.load(); h.injp(model, row, col, 0x00FF if model == 2 else 0x100)
    lat = None
    for t in range(120):
        h.drive(ACODE[t % M])
        ob = h.cyc1()
        if ob["trig"] and lat is None:
            lat = t
            break
    h.ral()
    print(f"  A2 永久(model={model},r={row},c={col}): 触发时延 = {lat if lat is not None else '未触发'} "
          f"-> {'PASS' if lat is not None and lat < 100 else 'FAIL'}")
    return lat is not None and lat < 100


def single_shot(h, row, col, trials=20):
    """A3: 单次真单拍注入的脉冲数分布。"""
    import random
    rng = random.Random(7)
    hist = {}
    for _ in range(trials):
        h.reset(); h.load()
        st = rng.randrange(2, 6)
        for t in range(40):
            h.drive(ACODE[t % M])
            if t == st:
                h.inj(1, row, col, 0)
            ob = h.cyc1()
            if ob["trig"]:
                break
        # 统计该场景的脉冲总数? 这里只抽查单拍注入是否 <= 少量
        npu = 0
        h.reset(); h.load()
        for t in range(40):
            h.drive(ACODE[t % M])
            if t == st:
                h.inj(1, row, col, 0)
            ob = h.cyc1()
            npu += ob["err"]
        hist[npu] = hist.get(npu, 0) + 1
    print(f"  A3 单次单拍注入(20 次) 脉冲数分布 = {dict(sorted(hist.items()))}")
    return max(hist.keys()) <= 8


def closed_loop(h, batches=60, span=32):
    """B: 逐批观测 -> 实时决定下一个注入 -> 统计在线误触发率。"""
    import random
    rng = random.Random(11)
    inj_total = 0
    trig_on_transient = 0
    t = 0
    h.reset(); h.load()
    for b in range(batches):
        # 决策: 本批是否注入一个单拍瞬态, 间隔骑 31/32/33 棱边
        gap = rng.choice([31, 32, 33])
        sched = rng.randrange(0, span) if rng.random() < 0.6 else None
        trig = 0
        errs = 0
        for k in range(span):
            h.drive(ACODE[t % M])
            if sched is not None and k == sched:
                h.inj(1, rng.randrange(N), rng.randrange(N + 1), 0)
                inj_total += 1
            ob = h.cyc1()
            errs += ob["err"]
            trig += ob["trig"]
            t += 1
            if ob["busy"]:
                break
        if trig and sched is not None:
            trig_on_transient += 1
    print(f"  B 在线闭环: {batches} 批 x {span} 拍; 纯瞬态注入 {inj_total} 次, "
          f"其中被静态裁决器触发 {trig_on_transient} 次 "
          f"= {100.0*trig_on_transient/max(1,batches):.1f}% (每批口径)")
    return trig_on_transient


def main():
    if not EXE.exists():
        print(f"[ERR] 未找到执行器 {EXE}，请先 python sim/hil/build_hil.py")
        return 1
    h = Hil()
    print("=" * 70)
    print("方案1: Verilator 逐周期闭环 —— Gate A 等价性 + 在线闭环实验")
    print("=" * 70)
    ok1 = mission_check(h)
    ok2 = perm_check(h, 1, 1, 1)
    ok3 = single_shot(h, 1, 1)
    print("-" * 70)
    print(f"Gate A: A1={'PASS' if ok1 else 'FAIL'} A2={'PASS' if ok2 else 'FAIL'} "
          f"A3={'PASS' if ok3 else 'FAIL'}")
    print("-" * 70)
    closed_loop(h)
    h.close()
    print("=" * 70)
    return 0


if __name__ == "__main__":
    sys.exit(main())
