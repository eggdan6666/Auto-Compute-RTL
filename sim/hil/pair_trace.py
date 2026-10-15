#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""pair_trace.py — 【新增文件】Icarus vs Verilator 同场景逐拍对拍 (P1 分支 B 前置诊断)

背景:
  离线训练 TB (tb_transient_train_n4.sv, Icarus/vvp) 对 n_ev=1 的纯瞬态注入给出
  3~4 个窗内脉冲 (max_win32>=3, 误触发 55.9%); 而 Verilator HIL 同口径单拍注入
  只给出 <=1 个脉冲 (A3 分布 {0:10,1:10})。两者对"单拍"语义的解释存在差异,
  需同场景对拍定位。

做法:
  1) Icarus 侧: tb/tb_inj_trace.sv —— 与训练 TB 同一注入惯用法
     (negedge force / posedge release), 逐拍 dump 注入点 PE 寄存器 + abft_err。
  2) Verilator 侧: sim/hil/hil_main.cpp 的 MON + C1 —— 同一场景、同一流式驱动。
  3) 逐拍 diff, 重点看被注入信号在本拍"周期末"是否仍保持注入值 (1 拍 vs 2 拍)。

采样口径 (两端一致, 与仿真器区序无关):
    ne_*  = 第 t 拍 negedge、驱动/注入之前
    out_* = 第 t 拍周期末 (Icarus 在第 t+1 拍 negedge 注入前读回; Verilator 在 posedge eval 后读)

用法:  python sim/hil/pair_trace.py
"""
import csv
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
OSS = Path(r"E:\tools\oss-cad-suite")
WORK = Path(r"E:\work\pair_trace")
HIL_EXE = Path(r"E:\work\hil_vl\obj_dir\hil.exe")
HIL_CWD = Path(r"E:\work\hil_vl")

RTL = ["self_healing_array.sv", "systolic_array.sv", "systolic_pe.sv", "rne_sat16.sv",
       "misr16.sv", "bist_controller.sv", "abft_checksum.sv", "zero_mask.sv",
       "fault_arbiter.sv"]

N, M = 4, 8
P2A = {0: 0x1000, 1: 0x2000, 2: 0x0800, 3: -0x1000}

W = [[P2A[(r + c) % 4] for c in range(N)] for r in range(N)]
A = [[P2A[(m + r + 1) % 4] for r in range(N)] for m in range(M)]
WCODE = [W[r][c] for r in range(N) for c in range(N)]
ACODE = [sum(((A[m][r] & 0xFFFF) << (16 * r)) for r in range(N)) for m in range(M)]

# 离线训练集里"单事件却产生 3~4 脉冲"的 4 个已知反例 (全部 model=2 / w_reg)
# 取自 transient_train_scen4.hex 的 id 23/26/27/39; 另加 model=1(psum_out) 对照
CASES = [
    (2, 5,  0, 1),    # id 27: w_reg r=0 c=1
    (2, 12, 1, 1),    # id 39: w_reg r=1 c=1
    (2, 14, 2, 1),    # id 26: w_reg r=2 c=1
    (2, 18, 3, 4),    # id 23: w_reg r=3 c=4
    (1, 5,  0, 1),    # 对照: psum_out, 同点位
    (1, 12, 1, 1),    # 对照: psum_out
    (1, 14, 2, 1),    # 对照: psum_out
    (1, 18, 3, 4),    # 对照: psum_out
]
N_CYC = 60


def env_oss():
    """纯净 OSS 环境 (勿混入 MinGW, 否则 vvp 静默退出/段错误)。"""
    e = dict(os.environ)
    e["PATH"] = f"{OSS / 'bin'};{OSS / 'lib'};{OSS / 'lib' / 'ivl'}"
    return e


def w_hex():
    v = 0
    for i, w in enumerate(WCODE):
        v |= (w & 0xFFFF) << (16 * i)
    return f"{v:064X}"


# ---------------------------------------------------------------------------
# Icarus 侧
# ---------------------------------------------------------------------------
def build_icarus():
    WORK.mkdir(parents=True, exist_ok=True)
    for f in ("bist_weights.hex", "bist_rows.hex", "bist_golden.hex"):
        src = ROOT / f
        if src.exists():
            shutil.copy2(src, WORK / f)
    cmd = [str(OSS / "bin" / "iverilog.exe"), "-g2012", "-o", str(WORK / "tb_pair.vvp"),
           "-I", str(ROOT / "rtl"), str(ROOT / "tb" / "tb_inj_trace.sv")]
    cmd += [str(ROOT / "rtl" / f) for f in RTL]
    p = subprocess.run(cmd, cwd=str(WORK), env=env_oss(), capture_output=True,
                       text=True, encoding="utf-8", errors="replace")
    keep = [l for l in (p.stdout + p.stderr).splitlines() if "sorry:" not in l and l.strip()]
    if p.returncode != 0 or not (WORK / "tb_pair.vvp").exists():
        print("[FAIL] iverilog 编译失败")
        print("\n".join(keep[-30:]))
        return False
    if keep:
        print("  [iverilog 提示]")
        for l in keep[:5]:
            print("   ", l)
    return True


def run_icarus(model, cyc, row, col, val=0, ncyc=N_CYC):
    csvp = WORK / "inj_trace_icarus.csv"
    if csvp.exists():
        csvp.unlink()
    cmd = [str(OSS / "bin" / "vvp.exe"), "-N", str(WORK / "tb_pair.vvp"),
           f"+INJ_CYC={cyc}", f"+INJ_ROW={row}", f"+INJ_COL={col}",
           f"+INJ_MODEL={model}", f"+INJ_VAL={val:X}", f"+NCYC={ncyc}"]
    p = subprocess.run(cmd, cwd=str(WORK), env=env_oss(), capture_output=True,
                       text=True, encoding="utf-8", errors="replace")
    if not csvp.exists():
        print("[FAIL] vvp 未产出 trace:", p.stdout[-400:], p.stderr[-400:])
        return None
    return list(csv.DictReader(open(csvp, encoding="utf-8")))


# ---------------------------------------------------------------------------
# Verilator 侧
# ---------------------------------------------------------------------------
class Hil:
    def __init__(self):
        self.p = subprocess.Popen([str(HIL_EXE)], cwd=str(HIL_CWD), stdin=subprocess.PIPE,
                                  stdout=subprocess.PIPE, text=True, bufsize=1)

    def cmd(self, s):
        self.p.stdin.write(s + "\n")
        self.p.stdin.flush()
        while True:
            line = self.p.stdout.readline()
            if not line:
                raise RuntimeError("hil.exe 提前退出")
            line = line.strip()
            if line.startswith("%Warning") or not line:
                continue
            return line

    def cyc1(self):
        f = self.cmd("C1").split()
        return {
            "ne_err": int(f[1]), "ne_trig": int(f[2]), "out_tcnt": int(f[3]),
            "out_busy": int(f[4]), "out_vout": int(f[5], 16),
            "out_cout": int(f[6], 16) & 0xFFFF,
            "ne_tcnt": int(f[7]), "ne_busy": int(f[8]),
            "ne_psum": int(f[9], 16), "ne_wreg": int(f[10], 16),
            "out_err": int(f[11]), "out_psum": int(f[12], 16), "out_wreg": int(f[13], 16),
            "out_trig": int(f[14]),
        }

    def close(self):
        try:
            self.cmd("Q")
        finally:
            self.p.kill()


def run_verilator(model, cyc, row, col, val=0, ncyc=N_CYC, idle=4):
    h = Hil()
    assert h.cmd("R") == "K"
    assert h.cmd(f"W {w_hex()}") == "K"
    assert h.cmd(f"MON {row} {col}") == "K"
    # 相位对齐: Icarus 的 load_w 于 negedge 置起, 装载沿 = 其后第一个 posedge;
    # 之后还有 3 个 repeat(3)@(negedge) + 1 个 @(negedge) 才进入流式 t=0 => 4 个 idle 拍。
    for _ in range(idle):
        assert h.cmd("D 0 0000000000000000") == "K"
        h.cyc1()
    rows = []
    for t in range(ncyc):
        assert h.cmd(f"D 1 {ACODE[t % M] & ((1 << 64) - 1):016X}") == "K"
        if t == cyc:
            assert h.cmd(f"I {model} {row} {col} {val:X}") == "K"
        ob = h.cyc1()
        ob["t"] = t
        rows.append(ob)
    h.close()
    return rows


# ---------------------------------------------------------------------------
# 对拍
# ---------------------------------------------------------------------------
COLS = [("ne_err", 1), ("out_err", 1), ("ne_psum", 16), ("out_psum", 16),
        ("ne_wreg", 16), ("out_wreg", 16), ("ne_tcnt", 1), ("out_tcnt", 1),
        ("ne_busy", 1), ("out_busy", 1), ("ne_trig", 1), ("out_trig", 1),
        ("out_cout", 16), ("out_vout", 1)]


HEX_COLS = ("ne_psum", "out_psum", "ne_wreg", "out_wreg", "out_cout")
_HEXDIG = set("0123456789abcdefABCDEF")


def hv(s):
    """容错 hex 解析: Icarus 侧可能出现 x/z。"""
    s = str(s).strip()
    if s and all(c in _HEXDIG for c in s):
        return int(s, 16)
    return None


def diff(ic_rows, vr_rows, cyc):
    ic = {int(r["t"]): r for r in ic_rows}
    bad = []
    for r in vr_rows:
        t = r["t"]
        if t not in ic:
            continue
        for k, _ in COLS:
            a = hv(ic[t][k]) if k in HEX_COLS else int(ic[t][k])
            if a is None or a != r[k]:
                bad.append((t, k, ic[t][k], r[k]))
    return bad


def show(ic_rows, vr_rows, cyc, row, col, model):
    ic = {int(r["t"]): r for r in ic_rows}
    vr = {r["t"]: r for r in vr_rows}
    err_ic = [t for t in sorted(ic) if int(ic[t]["out_err"])]
    err_vr = [t for t in sorted(vr) if vr[t]["out_err"]]
    key = "out_wreg" if model == 2 else "out_psum"
    print(f"  Icarus   out_err 脉冲拍号 = {err_ic}  (n={len(err_ic)})")
    print(f"  Verilat. out_err 脉冲拍号 = {err_vr}  (n={len(err_vr)})")
    print(f"  out_trig(Icarus/Verilator) = {[t for t in sorted(ic) if int(ic[t]['out_trig'])]}"
          f" / {[t for t in sorted(vr) if vr[t]['out_trig']]}")

    # 被注入信号"周期末"是否仍等于注入值 => 判定注入值是否被即刻覆盖
    print(f"  被注入 {key} 的周期末值 (注入值 = 0):")
    lo, hi = max(0, cyc - 2), min(len(vr), cyc + 7)
    for t in range(lo, hi):
        iv = int(ic[t][key], 16) if t in ic else None
        vv = vr[t][key] if t in vr else None
        mark = "  <== INJ_CYC" if t == cyc else ""
        print(f"    t={t:3d}  icarus=0x{iv:09X}  verilator=0x{vv:09X}{mark}")


def main():
    if not WORK.exists():
        WORK.mkdir(parents=True, exist_ok=True)
    print("编译 Icarus 对拍 TB ...")
    if not build_icarus():
        return 1
    if not HIL_EXE.exists():
        print(f"[ERR] 未找到 {HIL_EXE}; 请先 python sim/hil/build_hil.py")
        return 1

    print("=" * 78)
    print("Icarus(vvp) vs Verilator(hil.exe) 同场景逐拍对拍 —— 注入语义差异定位")
    print("=" * 78)
    summary = []
    for model, cyc, row, col in CASES:
        mname = "psum_out" if model == 1 else "w_reg"
        print(f"\n[场景] model={model}({mname}) row={row} col={col} INJ_CYC={cyc} val=0")
        ic_rows = run_icarus(model, cyc, row, col)
        vr_rows = run_verilator(model, cyc, row, col)
        if ic_rows is None:
            continue
        show(ic_rows, vr_rows, cyc, row, col, model)
        bad = diff(ic_rows, vr_rows, cyc)
        n_ic = sum(1 for r in ic_rows if int(r["out_err"]))
        n_vr = sum(1 for r in vr_rows if r["out_err"])
        if bad:
            print(f"  逐拍 diff: {len(bad)} 处不一致, 前 12 处:")
            for t, k, a, b in bad[:12]:
                print(f"    t={t:3d} {k:10s} icarus={a} verilator={b}")
        else:
            print(f"  逐拍 diff: 完全一致 (14 列 x {N_CYC} 拍)")
        summary.append((model, row, col, cyc, n_ic, n_vr, "一致" if not bad else f"{len(bad)}处差异"))

    print("\n" + "=" * 78)
    print("汇总:")
    print(f"  {'model':<10}{'row':>4}{'col':>4}{'cyc':>5}{'icarus脉冲':>11}{'verilator脉冲':>14}  逐拍diff")
    for model, row, col, cyc, n_ic, n_vr, d in summary:
        mname = "psum_out" if model == 1 else "w_reg"
        print(f"  {mname:<10}{row:>4}{col:>4}{cyc:>5}{n_ic:>11}{n_vr:>14}  {d}")
    print("=" * 78)
    return 0


if __name__ == "__main__":
    sys.exit(main())
