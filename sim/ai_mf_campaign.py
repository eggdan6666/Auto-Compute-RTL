#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""ai_mf_campaign.py — 丰富故障模型上的 AI 故障注入验证（P1 / 分支 A 的 AI 层）

在**只读复用**现有 TB 的基础上，做三件 P0 没做的事：
  1) 候选值先验播种：val 不再纯随机，而是从真实数据（bist_weights.hex /
     mission4_weights.hex 的实际字 + 单 bit 翻转 + 边界值）播种，并加入
     "掩蔽候选 × 真故障" 的定向配对——这才是撞 MISR 混叠/良性掩蔽的主力；
  2) 结构化合同判定：期望由 (nf, 涉及物理列集合) **推导**，绝不信任 hex 里的
     cat 标签；违约**双向记账**（"本应自愈却没自愈" 与 "本应不可愈却自愈" 都算）；
  3) 校准门：先用打分器复算已知 campaign 结果，violation 必须为 0，才允许 AI 上场。

只读复用：tb/tb_fault_campaign_mf_n{N}.sv（若缺则调 gen_mf_campaign_tb.py 生成）、
        bist_*.hex、mission{N}_*.hex。
只写：   <work>/  (默认 sim/ai_mf_work/)  —— fault_list_mf{N}.hex / vvp / CSV / 台账
MN 可选：--ledger 时把"真命中"追加到 sim/experiment_ledger.csv（追加，不覆盖）。

用法:
  python sim/ai_mf_campaign.py --n 4 --rounds 12 --batch 200
  python sim/ai_mf_campaign.py --n 8 --rounds 12 --batch 200      # N=4 全零后复跑
"""

import argparse
import csv
import json
import random
import shutil
import subprocess
import sys
import time
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parent.parent
DEFAULT_WORK = ROOT / "sim" / "ai_mf_work"
DEFAULT_OSS = Path(r"E:\tools\oss-cad-suite")

RTL_FILES = ["systolic_pe.sv", "rne_sat16.sv", "systolic_array.sv", "misr16.sv",
             "bist_controller.sv", "self_healing_array.sv", "abft_checksum.sv",
             "zero_mask.sv", "fault_arbiter.sv"]
DATA_FILES = ["bist_weights.hex", "bist_rows.hex", "bist_golden.hex"]

M_PSUM, M_W = 1, 2
MASK40 = (1 << 40) - 1


# ---------------------------------------------------------------------------
# 编码 / 解码（与 gen_mf_campaign_tb.py 的 128bit 格式一致）
# ---------------------------------------------------------------------------
def pack(nf, f1, f2, cat=0):
    w1 = (f1[0] << 46) | (f1[1] << 43) | (f1[2] << 40) | (f1[3] & MASK40)
    w2 = (f2[0] << 46) | (f2[1] << 43) | (f2[2] << 40) | (f2[3] & MASK40)
    return f"{((nf << 120) | (w1 << 72) | (w2 << 24) | (cat << 16)):032X}"


def dec_word(w):
    return ((w >> 46) & 0x3, (w >> 43) & 0x7, (w >> 40) & 0x7, w & MASK40)


# ---------------------------------------------------------------------------
# 结构化合同判定（核心；cat 标签不参与）
# ---------------------------------------------------------------------------
def expectation(nf, f1, f2):
    """由 (故障数, 涉及物理列) 推导合同期望。"""
    cols = {f[1] for f in (f1, f2)[:max(0, min(2, nf))] if f[0] != 0}
    if not cols:                       # 对照
        return {"detected": 0, "healed": 0, "unhealable": 0, "post_err": 0}
    if len(cols) == 1:                 # 单物理列（含同列双/同PE双）
        return {"detected": 1, "healed": 1, "unhealable": 0, "post_err": 0}
    return {"detected": 1, "healed": 0, "unhealable": 1, "post_err": None}  # 跨列双


def row_faults(r):
    nf = int(r["nf"])
    f1 = (int(r["f1m"]), int(r["f1c"]), int(r["f1r"]), int(r["f1v"], 16))
    f2 = (int(r["f2m"]), int(r["f2c"]), int(r["f2r"]), int(r["f2v"], 16))
    return nf, f1, f2


def violations(r):
    """返回违约条款列表；空列表=符合合同。双向记账。"""
    nf, f1, f2 = row_faults(r)
    exp = expectation(nf, f1, f2)
    v = []
    for k in ("detected", "healed", "unhealable"):
        if int(r[k]) != exp[k]:
            v.append(f"{k}:{int(r[k])}!=期望{exp[k]}")
    if exp["post_err"] is not None and int(r["post_err"]) != exp["post_err"]:
        v.append(f"post_err:{int(r['post_err'])}!=期望{exp['post_err']}")
    return v


def viol_score(r):
    return 100.0 * len(violations(r))


# ---------------------------------------------------------------------------
# 工具链 / 工作目录
# ---------------------------------------------------------------------------
def find_exe(oss, name):
    for c in (oss / "bin" / name, oss / "bin" / (name + ".exe")):
        if c.exists():
            return str(c)
    return shutil.which(name)


def prepare_work(work, n, oss, rebuild=False):
    work.mkdir(parents=True, exist_ok=True)
    tb_src = ROOT / "tb" / f"tb_fault_campaign_mf_n{n}.sv"
    if not tb_src.exists():
        gen = ROOT / "sim" / "gen_mf_campaign_tb.py"
        if not gen.exists():
            sys.exit(f"[ERR] 缺 TB 且缺生成器: {tb_src}")
        print(f"[gen] 生成 {tb_src.name} ...")
        subprocess.run([sys.executable, str(gen), str(n)], cwd=str(ROOT), check=True)

    need = DATA_FILES + [f"mission{n}_weights.hex", f"mission{n}_vectors.hex"]
    for name in need:
        src = ROOT / name
        if not src.exists():
            sys.exit(f"[ERR] 缺只读激励: {src}")
        dst = work / name
        if not dst.exists() or dst.stat().st_mtime < src.stat().st_mtime:
            shutil.copy2(src, dst)

    vvp = work / f"tb_mf{n}.vvp"
    if rebuild or not vvp.exists():
        iverilog = find_exe(oss, "iverilog")
        if not iverilog:
            sys.exit("[ERR] 找不到 iverilog（--oss-dir 指定）")
        cmd = [iverilog, "-g2012", "-o", str(vvp)] + \
              [str(ROOT / "rtl" / f) for f in RTL_FILES] + [str(tb_src)]
        print(f"[build] -> {vvp.name}")
        p = subprocess.run(cmd, capture_output=True, text=True, encoding="utf-8", errors="replace")
        if not vvp.exists():
            sys.exit("[ERR] 编译失败:\n" + (p.stderr or "")[:1500])
    return work, vvp


def run_round(work, vvp, n, entries, vvp_exe):
    (work / f"fault_list_mf{n}.hex").write_text(
        "\n".join(pack(*e) for e in entries) + "\n", encoding="ascii")
    p = subprocess.run([vvp_exe, str(vvp), f"+NS={len(entries)}"], cwd=str(work),
                       capture_output=True, text=True, encoding="utf-8", errors="replace")
    res = work / f"campaign_results{n}_mf.csv"
    if not res.exists():
        sys.exit("[ERR] 未产出结果:\n" + (p.stderr or p.stdout or "")[:1500])
    return list(csv.DictReader(open(res, encoding="utf-8")))


# ---------------------------------------------------------------------------
# 候选值先验播种：从真实数据来
# ---------------------------------------------------------------------------
def load_words(path):
    ws = []
    for tok in Path(path).read_text().split():
        tok = tok.strip()
        if tok:
            try:
                ws.append(int(tok, 16) & 0xFFFF)
            except ValueError:
                pass
    return ws


def seed_pools(n, seed=0):
    """返回 (w16 候选池, psum40 候选池)。确定性（用局部 RNG）。"""
    rng = random.Random(seed)
    words = load_words(ROOT / "bist_weights.hex") + load_words(ROOT / f"mission{n}_weights.hex")
    w16 = set([0, 1, 0xFFFF, 0x8000, 0x7FFF])
    for w in words:
        w16.add(w)
        for b in range(16):                     # 单 bit 翻转
            w16.add(w ^ (1 << b))
    w16 = sorted(w16)

    p40 = {0, 1, MASK40, 0xFFFF, 0xFFFFFFFF, 0x5555555555 & MASK40, 0xAAAAAAAAAA & MASK40}
    for w in words:
        se = w if w < 0x8000 else (w | 0xFFFF0000)     # 16->40 符号扩展
        rep = (w << 24) | (w << 8) | (w >> 8)
        for base in (se & MASK40, rep & MASK40, (w * 2) & MASK40, (w * 3) & MASK40,
                     (w * 5) & MASK40, (w * 7) & MASK40):
            p40.add(base)
            for _ in range(3):                          # 少量 bit 翻转变体
                p40.add(base ^ (1 << rng.randrange(40)))
    p40 = sorted(p40)
    return w16, p40


# ---------------------------------------------------------------------------
# 候选构造
# ---------------------------------------------------------------------------
def val_for(model, w16, p40, rng):
    return rng.choice(p40) if model == M_PSUM else rng.choice(w16)


def cand_single(n, w16, p40, rng):
    m = rng.choice([M_PSUM, M_PSUM, M_W])
    return (1, (m, rng.randrange(n + 1), rng.randrange(n), val_for(m, w16, p40, rng)), (0, 0, 0, 0))


def cand_double(n, w16, p40, rng, same_col):
    c1 = rng.randrange(n + 1)
    c2 = c1 if same_col else rng.choice([c for c in range(n + 1) if c != c1])
    m1, m2 = rng.choice([M_PSUM, M_W]), rng.choice([M_PSUM, M_W])
    f1 = (m1, c1, rng.randrange(n), val_for(m1, w16, p40, rng))
    f2 = (m2, c2, rng.randrange(n), val_for(m2, w16, p40, rng))
    return (2, f1, f2)


def cand_mask_pair(n, w16, p40, rng, same_col):
    """掩蔽候选(m=2 取真实 BIST 权重值) × 真故障(m=1)，专打 '一个被掩蔽' 的角落。"""
    c1 = rng.randrange(n + 1)
    c2 = c1 if same_col else rng.choice([c for c in range(n + 1) if c != c1])
    mask_val = rng.choice(load_words(ROOT / "bist_weights.hex"))
    f1 = (M_W, c1, rng.randrange(n), mask_val)
    f2 = (M_PSUM, c2, rng.randrange(n), rng.choice(p40))
    return (2, f1, f2)


def cand_random(n, rng):
    m = rng.choices([0, M_PSUM, M_W], weights=[1, 6, 3])[0]
    if m == 0:
        return (0, (0, 0, 0, 0), (0, 0, 0, 0))
    if rng.random() < 0.5:
        return (1, (m, rng.randrange(n + 1), rng.randrange(n), rng.getrandbits(40 if m == M_PSUM else 16)), (0, 0, 0, 0))
    m2 = rng.choice([M_PSUM, M_W])
    return (2,
            (m, rng.randrange(n + 1), rng.randrange(n), rng.getrandbits(40 if m == M_PSUM else 16)),
            (m2, rng.randrange(n + 1), rng.randrange(n), rng.getrandbits(40 if m2 == M_PSUM else 16)))


class Surrogate:
    """违约分回归 MLP 集成 + UCB（首批正样本出现前它只是放大器，不是探测器）。"""

    def __init__(self, n, k=5, epochs=250, seed=0):
        import torch
        self.t = torch
        self.k, self.epochs = k, epochs
        torch.manual_seed(seed)
        d = self._dim(n)
        self.nets = []
        for i in range(k):
            torch.manual_seed(seed * 100 + i)
            self.nets.append(torch.nn.Sequential(
                torch.nn.Linear(d, 64), torch.nn.ReLU(),
                torch.nn.Linear(64, 64), torch.nn.ReLU(),
                torch.nn.Linear(64, 1)))

    @staticmethod
    def _dim(n):
        return 3 + (n + 1) + n + 6

    @staticmethod
    def _feat(e, n):
        nf, f1, f2 = e
        f = np.zeros(Surrogate._dim(n), dtype=np.float32)
        f[M_PSUM] = 1.0 if nf >= 1 and f1[0] == M_PSUM else 0.0
        f[M_W] = 1.0 if nf >= 1 and f1[0] == M_W else 0.0
        f[0] = 1.0 if nf == 0 else 0.0
        cols = {x[1] for x in (f1, f2)[:nf] if x[0] != 0}
        f[3 + (min(cols) if cols else 0)] = 1.0
        f[3 + (n + 1) + (f1[2] if nf >= 1 else 0)] = 1.0
        b = 3 + (n + 1) + n
        f[b + 0] = len(cols) / 2.0
        f[b + 1] = f1[3].bit_count() / 40.0
        f[b + 2] = f2[3].bit_count() / 40.0
        f[b + 3] = 1.0 if f1[3] == 0 else 0.0
        f[b + 4] = 1.0 if nf == 2 else 0.0
        f[b + 5] = (f1[3] & 0xFFFF) / 65535.0
        return f

    def feats(self, pool, n):
        return np.array([Surrogate._feat(e, n) for e in pool], dtype=np.float32)

    def fit(self, X, y, rng):
        t = self.t
        Xt, yt = t.tensor(X), t.tensor(y, dtype=t.float32).view(-1, 1)
        m = Xt.shape[0]
        for net in self.nets:
            opt = t.optim.Adam(net.parameters(), lr=1e-2, weight_decay=1e-4)
            idx = t.tensor(rng.integers(0, m, size=m), dtype=t.long)
            Xb, yb = Xt[idx], yt[idx]
            for _ in range(self.epochs):
                opt.zero_grad()
                t.nn.functional.mse_loss(net(Xb), yb).backward()
                opt.step()

    def ucb(self, pool, n, kappa):
        t = self.t
        Xt = t.tensor(self.feats(pool, n))
        with t.no_grad():
            p = t.stack([net(Xt).view(-1) for net in self.nets], 0)
        return p.mean(0).numpy() + kappa * p.std(0).numpy()


# ---------------------------------------------------------------------------
# 校准门
# ---------------------------------------------------------------------------
def calibration_gate(path):
    if not path.exists():
        print(f"[gate] 跳过（无 {path.name}）")
        return
    rows = list(csv.DictReader(open(path, encoding="utf-8")))
    bad = [(r["id"], violations(r)) for r in rows if violations(r)]
    print(f"[gate] 校准于 {path.name}：{len(rows)} 行，违约 {len(bad)} 行 -> "
          f"{'PASS' if not bad else 'FAIL(打分器有误)'}")
    if bad:
        for i, v in bad[:5]:
            print("   id=%s %s" % (i, v))
        sys.exit("[ERR] 校准门未过：先修打分器，再放 AI。")


# ---------------------------------------------------------------------------
# main
# ---------------------------------------------------------------------------
def main():
    ap = argparse.ArgumentParser(description="丰富故障模型上的 AI 故障注入验证")
    ap.add_argument("--n", type=int, default=4)
    ap.add_argument("--rounds", type=int, default=12)
    ap.add_argument("--batch", type=int, default=200)
    ap.add_argument("--explore-frac", type=float, default=0.5, help="随机基线占比")
    ap.add_argument("--kappa", type=float, default=0.5)
    ap.add_argument("--pool", type=int, default=6000)
    ap.add_argument("--seed", type=int, default=20261015)
    ap.add_argument("--work", default=str(DEFAULT_WORK))
    ap.add_argument("--oss-dir", default=str(DEFAULT_OSS))
    ap.add_argument("--rebuild", action="store_true")
    ap.add_argument("--ledger", action="store_true", help="真命中追加到 sim/experiment_ledger.csv")
    args = ap.parse_args()

    n, rng = args.n, np.random.default_rng(args.seed)
    prng = random.Random(args.seed)
    oss = Path(args.oss_dir)

    calibration_gate(ROOT / "sim" / "mf_work" / f"campaign_results{n}_mf.csv")

    vvp_exe = find_exe(oss, "vvp")
    if not vvp_exe:
        sys.exit("[ERR] 找不到 vvp")
    work, vvp = prepare_work(Path(args.work), n, oss, args.rebuild)
    w16, p40 = seed_pools(n, args.seed)
    print(f"[cfg] N={n} rounds={args.rounds} batch={args.batch} explore={args.explore_frac} "
          f"| 播种池: w16={len(w16)} psum40={len(p40)} | work={work}")

    seen, log, X, Y = set(), [], [], []
    surro, SCALE, t0 = None, 100.0, time.time()
    n_rand = int(round(args.batch * args.explore_frac))
    hits = []

    print("=" * 78)
    print(" 轮次 | 场景 | 本轮违约 | 累计违约(AI/随机) | 真命中")
    print("=" * 78)

    for rd in range(args.rounds):
        src, batch = {}, []

        def add(e, s):
            code = pack(*e)
            if code in seen:
                return False
            seen.add(code); src[code] = s; batch.append(e)
            return True

        while len(batch) < n_rand:
            add(cand_random(n, prng), "random")

        pool = []
        while len(pool) < args.pool:
            k = prng.random()
            if k < 0.30:
                pool.append(cand_double(n, w16, p40, prng, same_col=bool(prng.getrandbits(1))))
            elif k < 0.55:
                pool.append(cand_mask_pair(n, w16, p40, prng, same_col=bool(prng.getrandbits(1))))
            elif k < 0.75:
                pool.append(cand_double(n, w16, p40, prng, same_col=True))
            else:
                pool.append(cand_single(n, w16, p40, prng))
        if len(X) > 0 and surro is not None:
            surro.fit(np.array(X, dtype=np.float32), np.array(Y, dtype=np.float32), rng)
            order = np.argsort(-surro.ucb(pool, n, args.kappa))
            cand = [pool[i] for i in order]
        else:
            prng.shuffle(pool); cand = pool
        for e in cand:
            if len(batch) >= args.batch:
                break
            add(e, "ai")

        res = run_round(work, vvp, n, batch, vvp_exe)
        cur_v = 0
        for r in res:
            e = batch[int(r["id"])]
            rr = dict(r)
            rr["source"] = src[pack(*e)]
            rr["round"] = rd
            rr["viol"] = ";".join(violations(rr))
            log.append(rr)
            if rr["viol"]:
                hits.append(rr)
                cur_v += 1

        X = np.array([Surrogate._feat(row_faults(r), n) for r in log], dtype=np.float32)
        Y = np.array([viol_score(r) for r in log], dtype=np.float32) / SCALE

        if surro is None:
            surro = Surrogate(n, seed=args.seed)

        ai_v = sum(1 for r in log if r["source"] == "ai" and r["viol"])
        rd_v = sum(1 for r in log if r["source"] == "random" and r["viol"])
        print(f"  {rd:>3} | {len(res):>4} | {cur_v:>5} | AI={ai_v:>3} 随机={rd_v:>3} | {len(hits)}")

    dt = time.time() - t0
    cols = ["round", "source", "id", "nf", "cat", "f1m", "f1c", "f1r", "f1v",
            "f2m", "f2c", "f2r", "f2v", "pre_err", "detected", "fault_col",
            "healed", "unhealable", "post_err", "viol"]
    with open(work / "ai_mf_log.csv", "w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=cols)
        w.writeheader()
        for r in log:
            w.writerow({k: r.get(k, "") for k in cols})

    def brk(src):
        rs = [r for r in log if r["source"] == src]
        return {"n": len(rs), "viol": sum(1 for r in rs if r["viol"]),
                "cells": len({(r["nf"], r["f1c"], r["f2c"]) for r in rs})}

    ai_s, rd_s = brk("ai"), brk("random")
    print("=" * 78)
    print(f"完成：{len(log)} 场景 / {dt:.1f}s | AI 违约 {ai_s['viol']}/{ai_s['n']}，"
          f"随机违约 {rd_s['viol']}/{rd_s['n']} | 真命中 {len(hits)}")
    for r in hits[:12]:
        print(f"  HIT id={r['id']} src={r['source']} nf={r['nf']} "
              f"cols=({r['f1c']},{r['f2c']}) viol={r['viol']}")
    (work / "ai_mf_summary.json").write_text(json.dumps(
        {"config": vars(args), "elapsed_s": dt, "total": len(log),
         "ai": ai_s, "random": rd_s, "n_hits": len(hits),
         "hits": [{k: r[k] for k in cols} for r in hits[:50]]},
        indent=2, ensure_ascii=False), encoding="utf-8")
    print(f"产物： ai_mf_log.csv, ai_mf_summary.json（在 {work}）")

    if args.ledger and hits:
        led = ROOT / "sim" / "experiment_ledger.csv"
        new = not led.exists()
        with open(led, "a", newline="", encoding="utf-8") as f:
            w = csv.writer(f)
            if new:
                w.writerow(["campaign", "n", "scenario_id", "source", "nf", "cols",
                            "violation", "detected", "healed", "unhealable", "post_err"])
            for r in hits:
                w.writerow(["ai_mf", n, r["id"], r["source"], r["nf"],
                            f"{r['f1c']}/{r['f2c']}", r["viol"],
                            r["detected"], r["healed"], r["unhealable"], r["post_err"]])
        print(f"[ledger] 已追加 {len(hits)} 行到 {led}")


if __name__ == "__main__":
    main()
