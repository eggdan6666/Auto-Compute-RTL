#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
ai_fault_campaign.py — AI 驱动的故障注入验证引擎（P0）
=======================================================
定位：在现有 campaign 链路**之上**加一层 AI 故障注入引擎。它**只读复用**
      现有 RTL / TB / 激励数据，绝不修改任何已有文件；所有产物只写进独立
      工作目录（默认 sim/ai_fault_work/）。

复用（只读）                              本脚本新增（仅写 <work>/）
--------------------------------------    ------------------------------
rtl/*.sv              自愈阵列+BIST/ABFT   fault_list{N}.hex   每轮重写
tb/tb_fault_campaign_n{N}.sv  现成注入 TB   tb_campaign{N}.vvp   编译产物
bist_*.hex             BIST 激励(供 cwd)    campaign_results{N}.csv  每轮覆盖
mission{N}_*.hex       任务权重/向量        ai_fault_log.csv    累计台账
                                          ai_campaign_summary.json 汇总

工作机理（AI 主动搜索"最坏故障"）:
  1) 每轮生成一批故障场景 -> 写 <work>/fault_list{N}.hex
  2) 调用现有 iverilog/vvp 跑现有 campaign TB（cwd=<work>，从 <work> 读激励）
  3) 解析 campaign_results{N}.csv -> 计算"危害分"(escaped/未检出/误定位/未修复)
  4) 训练代理模型（torch MLP 集成）预测危害分，用 UCB 主动采样下一批候选；
     同时保留一部分随机样本作为**同预算随机基线**对照
  5) 汇总：AI vs 随机 的发现曲线、最坏用例、覆盖率

用法:
  python sim/ai_fault_campaign.py --n 4 --rounds 8 --batch 200
  python sim/ai_fault_campaign.py --n 4 --rounds 8 --batch 400 --explore-frac 0.5
  python sim/ai_fault_campaign.py --n 4 --rebuild          # 强制重编译 TB

说明：本脚本不产生 MTTR 量化（那属于 tb_phase2 外循环闭环验证），P0 只做
      "AI 搜索故障空间 -> 命中检出/定位/修复的结构性边角"这一层。
"""

import argparse
import csv
import json
import os
import random
import shutil
import subprocess
import sys
import time
from pathlib import Path

import numpy as np

# ----------------------------------------------------------------------------
# 0. 路径与常量
# ----------------------------------------------------------------------------
ROOT = Path(__file__).resolve().parent.parent          # 仓库根 (…/Auto-Compute-RTL)
DEFAULT_WORK = ROOT / "sim" / "ai_fault_work"
DEFAULT_OSS = Path(r"E:\tools\oss-cad-suite")          # 便携 Icarus/Verilator/Yosys

RTL_FILES = [
    "systolic_pe.sv", "rne_sat16.sv", "systolic_array.sv",
    "misr16.sv", "bist_controller.sv", "self_healing_array.sv",
    "abft_checksum.sv", "zero_mask.sv", "fault_arbiter.sv",
]
# TB/RTL 以 cwd 相对路径读取的激励文件（必须放进 <work>）
DATA_FILES = ["bist_weights.hex", "bist_rows.hex", "bist_golden.hex"]

MODEL_NONE, MODEL_PSUM, MODEL_W = 0, 1, 2
VAL_BITS = {MODEL_PSUM: 40, MODEL_W: 16}


def encode(model, col, row, val):
    """(model,col,row,val) -> 48bit -> 12 位十六进制字符串（与 golden_campaign.py 一致）"""
    return f"{((model << 46) | (col << 43) | (row << 40) | (val & ((1 << 40) - 1))):012X}"


def harm_score(r):
    """把 campaign 结果折算成"危害分"（越大越坏），用于 AI 目标函数。

    关键：区分对照场景(model=0)。对照本就无故障，"未检出/未修复"是**期望行为**，
    对对照只惩罚"误报"；否则 AI 会去刷无意义的对照场景。
    """
    model = int(r["model"])
    post_err = int(r["post_err"])
    det = int(r["detected"])
    loc = int(r["loc_ok"])
    hld = int(r["healed"])

    if model == MODEL_NONE:                # 对照：仅"误报"算坏
        return 100.0 if det != 0 else 0.0

    s = 0.0
    if post_err > 0:                       # 自愈后任务仍出错 = 逃逸(最严重)
        s += 100.0
    if det == 0:                           # 未检出：有害漏检 / 良性掩蔽
        s += 50.0 if post_err > 0 else 5.0
    if det == 1 and loc == 0:              # 检出但定位错
        s += 50.0
    if det == 1 and hld == 0:              # 检出但未修复
        s += 30.0
    s += 2.0 * min(post_err, 20)           # 残余错误量
    return s


def feature_dim(n):
    return 3 + (n + 1) + n + 6


def feats(model, col, row, val, n):
    f = np.zeros(feature_dim(n), dtype=np.float32)
    f[model] = 1.0
    f[3 + col] = 1.0
    f[3 + (n + 1) + row] = 1.0
    b = 3 + (n + 1) + n
    f[b + 0] = val.bit_count() / 40.0
    f[b + 1] = ((val >> 32) & 0xFF) / 255.0
    f[b + 2] = ((val >> 24) & 0xFF) / 255.0
    f[b + 3] = 1.0 if val == 0 else 0.0
    f[b + 4] = 1.0 if val == (1 << 40) - 1 else 0.0
    f[b + 5] = (val & 0xFFFF) / 65535.0
    return f


# ----------------------------------------------------------------------------
# 1. 工具链与工作目录
# ----------------------------------------------------------------------------
def find_exe(oss_dir, name):
    for c in (oss_dir / "bin" / name, oss_dir / "bin" / (name + ".exe")):
        if c.exists():
            return str(c)
    return shutil.which(name)


def prepare_work(work, n, rebuild=False, oss=DEFAULT_OSS):
    """建工作目录、拷贝只读激励、编译现有 TB（产物只落 <work>）。"""
    work.mkdir(parents=True, exist_ok=True)
    tb_src = ROOT / "tb" / f"tb_fault_campaign_n{n}.sv"
    if not tb_src.exists():
        sys.exit(f"[ERR] 找不到现成 TB: {tb_src}\n"
                 f"      本脚本为只读复用，不生成/覆写 tb/。请先用 gen_campaign_tb.py {n} 自行生成。")

    for name in DATA_FILES + [f"mission{n}_weights.hex", f"mission{n}_vectors.hex"]:
        src = ROOT / name
        if not src.exists():
            sys.exit(f"[ERR] 缺少只读激励文件: {src}")
        dst = work / name
        if not dst.exists() or dst.stat().st_mtime < src.stat().st_mtime:
            shutil.copy2(src, dst)

    vvp = work / f"tb_campaign{n}.vvp"
    if rebuild or not vvp.exists():
        iverilog = find_exe(oss, "iverilog")
        if not iverilog:
            sys.exit("[ERR] 找不到 iverilog（可设 --oss-dir 指向 oss-cad-suite 的 bin）")
        cmd = [iverilog, "-g2012", "-o", str(vvp)] + \
              [str(ROOT / "rtl" / f) for f in RTL_FILES] + [str(tb_src)]
        print(f"[build] {Path(iverilog).name} -> {vvp.name}")
        p = subprocess.run(cmd, capture_output=True, text=True, encoding="utf-8", errors="replace")
        if not vvp.exists():                        # iverilog 的 "sorry:" 是非致命告警
            sys.exit("[ERR] 编译失败:\n" + (p.stderr or "")[:2000])
    return work, vvp


# ----------------------------------------------------------------------------
# 2. 一轮仿真：写激励 -> 跑 vvp -> 解析 CSV
# ----------------------------------------------------------------------------
def run_round(work, vvp, n, entries, vvp_exe, quiet=True):
    """entries: list[(model,col,row,val)] -> 返回 list[dict] (含 harmscore)。"""
    (work / f"fault_list{n}.hex").write_text(
        "\n".join(encode(*e) for e in entries) + "\n", encoding="ascii")

    cmd = [vvp_exe, str(vvp), f"+NS={len(entries)}"]
    p = subprocess.run(cmd, cwd=str(work), capture_output=True, text=True,
                       encoding="utf-8", errors="replace")
    if not quiet:
        print(p.stdout[-400:])

    res_path = work / f"campaign_results{n}.csv"
    if not res_path.exists():
        sys.exit("[ERR] 未产出结果 CSV:\n" + (p.stderr or p.stdout or "")[:2000])
    out = []
    with open(res_path, encoding="utf-8") as f:
        for r in csv.DictReader(f):
            r["harmscore"] = harm_score(r)
            out.append(r)
    return out


# ----------------------------------------------------------------------------
# 3. 候选生成 + 代理模型（torch MLP 集成）
# ----------------------------------------------------------------------------
def rand_entry(n, rng):
    model = rng.choices([MODEL_NONE, MODEL_PSUM, MODEL_W], weights=[1, 6, 3])[0]
    col = rng.randrange(n + 1)
    row = rng.randrange(n)
    if model == MODEL_NONE:
        val = 0
    elif model == MODEL_PSUM:
        val = rng.getrandbits(40)
    else:
        val = rng.getrandbits(16)
    return model, col, row, val


def structured_pool(n):
    """结构性边角（全 0/全 1/交替/边界值 × 每个 (model,col,row) 单元）。"""
    pool = []
    for model in (MODEL_PSUM, MODEL_W):
        bits = VAL_BITS[model]
        mask = (1 << bits) - 1
        vals = [0, 1, mask, mask ^ 1, 0x5555555555 & mask, 0xAAAAAAAAAA & mask]
        for col in range(n + 1):
            for row in range(n):
                for v in vals:
                    pool.append((model, col, row, v))
    return pool


class Surrogate:
    """危害分回归的 MLP 集成；ensemble std 作为不确定度，UCB = mu + kappa*sigma。"""

    def __init__(self, n, k=5, epochs=300, seed=0):
        import torch  # 延迟导入
        self.torch = torch
        self.n, self.k, self.epochs = n, k, epochs
        torch.manual_seed(seed)
        d = feature_dim(n)
        self.nets = []
        for i in range(k):
            torch.manual_seed(seed * 100 + i)
            net = torch.nn.Sequential(
                torch.nn.Linear(d, 64), torch.nn.ReLU(),
                torch.nn.Linear(64, 64), torch.nn.ReLU(),
                torch.nn.Linear(64, 1))
            self.nets.append(net)

    def fit(self, X, y, rng):
        torch = self.torch
        Xt = torch.tensor(X, dtype=torch.float32)
        yt = torch.tensor(y, dtype=torch.float32).view(-1, 1)
        m = Xt.shape[0]
        for net in self.nets:
            opt = torch.optim.Adam(net.parameters(), lr=1e-2, weight_decay=1e-4)
            # bootstrap 子集（不同网络看到不同样本子集 -> 提供不确定度）
            idx = torch.tensor(rng.integers(0, m, size=m), dtype=torch.long)
            Xb, yb = Xt[idx], yt[idx]
            for _ in range(self.epochs):
                opt.zero_grad()
                loss = torch.nn.functional.mse_loss(net(Xb), yb)
                loss.backward()
                opt.step()

    def predict_ucb(self, X, kappa):
        torch = self.torch
        Xt = torch.tensor(X, dtype=torch.float32)
        with torch.no_grad():
            preds = torch.stack([n(Xt).view(-1) for n in self.nets], dim=0)  # [k, m]
        mu = preds.mean(dim=0).numpy()
        sd = preds.std(dim=0).numpy()
        return mu + kappa * sd


# ----------------------------------------------------------------------------
# 4. 主流程
# ----------------------------------------------------------------------------
def main():
    ap = argparse.ArgumentParser(description="AI 驱动的故障注入验证引擎 (P0)")
    ap.add_argument("--n", type=int, default=4, help="阵列规模 N（需存在 tb_fault_campaign_nN.sv）")
    ap.add_argument("--rounds", type=int, default=8, help="AI 迭代轮数")
    ap.add_argument("--batch", type=int, default=200, help="每轮注入场景数")
    ap.add_argument("--explore-frac", type=float, default=0.5,
                    help="每轮随机样本占比（=同预算随机基线）")
    ap.add_argument("--kappa", type=float, default=0.5, help="UCB 探索系数")
    ap.add_argument("--pool", type=int, default=20000, help="候选池大小")
    ap.add_argument("--epochs", type=int, default=300)
    ap.add_argument("--seed", type=int, default=20261015)
    ap.add_argument("--work", type=str, default=str(DEFAULT_WORK))
    ap.add_argument("--oss-dir", type=str, default=str(DEFAULT_OSS))
    ap.add_argument("--rebuild", action="store_true")
    args = ap.parse_args()

    n = args.n
    rng = np.random.default_rng(args.seed)
    prng = random.Random(args.seed)
    work = Path(args.work)
    oss = Path(args.oss_dir)

    vvp_exe = find_exe(oss, "vvp")
    if not vvp_exe:
        sys.exit("[ERR] 找不到 vvp（可用 --oss-dir 指定）")

    work, vvp = prepare_work(work, n, rebuild=args.rebuild, oss=oss)
    print(f"[cfg] N={n} rounds={args.rounds} batch={args.batch} "
          f"explore={args.explore_frac} kappa={args.kappa} work={work}")

    n_rand = int(round(args.batch * args.explore_frac))
    n_ai = args.batch - n_rand
    seen = set()                       # 已评估过的故障编码（去重）
    log = []                           # 累计台账
    X, Y = [], []                      # 代理模型训练集 (feats, harmscore/scale)
    SCALE = 250.0
    surro = None
    t0 = time.time()

    print("=" * 74)
    print(" 轮次 | 来源 | 场景数 | 本轮最大危害 | 累计最大危害(AI/随机)")
    print("=" * 74)

    for rd in range(args.rounds):
        src_of = {}                    # entry -> 'ai' | 'random'
        batch = []

        # --- 随机基线切片 ---
        while len(batch) < n_rand:
            e = rand_entry(n, prng)
            code = encode(*e)
            if code in seen:
                continue
            seen.add(code)
            src_of[e] = "random"
            batch.append(e)

        # --- AI 切片（首轮若无模型则以随机充当探索） ---
        ai_pool = structured_pool(n) if rd == 0 else None
        if ai_pool is None:
            pool, cur = [], 0
            while cur < args.pool:
                e = rand_entry(n, prng)
                if encode(*e) not in seen:
                    pool.append(e)
                    cur += 1
            pool += structured_pool(n)
            pool = [e for e in pool if encode(*e) not in seen]
            if X and surro is not None:
                surro.fit(np.array(X, dtype=np.float32),
                          np.array(Y, dtype=np.float32), rng)
                ucb = surro.predict_ucb(
                    np.array([feats(*e, n) for e in pool], dtype=np.float32), args.kappa)
                order = np.argsort(-ucb)
                cand = [pool[i] for i in order]
            else:
                prng.shuffle(pool)
                cand = pool
        else:
            cand = ai_pool

        for e in cand:
            if len(batch) >= args.batch:
                break
            code = encode(*e)
            if code in seen:
                continue
            seen.add(code)
            src_of[e] = "ai"
            batch.append(e)

        # --- 跑一轮 ---
        res = run_round(work, vvp, n, batch, vvp_exe)
        best_this = max((float(r["harmscore"]) for r in res), default=0.0)
        for r in res:
            i = int(r["id"])
            e = batch[i]
            src = src_of[e]
            r["source"] = src
            r["round"] = rd
            log.append(dict(r))
            X.append(feats(e[0], e[1], e[2], e[3], n))
            Y.append(float(r["harmscore"]) / SCALE)

        if surro is None:
            import torch  # noqa: F401
            surro = Surrogate(n, epochs=args.epochs, seed=args.seed)

        best_ai = max((float(r["harmscore"]) for r in log if r["source"] == "ai"), default=0.0)
        best_rd = max((float(r["harmscore"]) for r in log if r["source"] == "random"), default=0.0)
        print(f"  {rd:>3} |  {len(res):>4} 场景 | 本轮 max={best_this:6.1f} "
              f"| 累计 AI={best_ai:6.1f}  随机={best_rd:6.1f}")

    dt = time.time() - t0

    # --- 汇总 ---
    log_path = work / "ai_fault_log.csv"
    cols = ["round", "source", "id", "model", "col", "row", "val",
            "pre_err", "detected", "loc_ok", "healed", "post_err", "harmscore"]
    with open(log_path, "w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=cols)
        w.writeheader()
        for r in log:
            w.writerow({k: r.get(k, "") for k in cols})

    def stats(src):
        rs = [r for r in log if r["source"] == src]
        faults = [r for r in rs if int(r["model"]) != 0]
        ctrls = [r for r in rs if int(r["model"]) == 0]
        if not rs:
            return {}
        return {
            "n": len(rs), "n_fault": len(faults), "n_ctrl": len(ctrls),
            "mean_harm": float(np.mean([float(r["harmscore"]) for r in rs])),
            "max_harm": float(np.max([float(r["harmscore"]) for r in rs])),
            # —— 仅对"真实故障"统计（排除对照）——
            "escaped": sum(1 for r in faults if int(r["post_err"]) > 0),
            "missed_harmful": sum(1 for r in faults
                                   if int(r["detected"]) == 0 and int(r["post_err"]) > 0),
            "missed_benign": sum(1 for r in faults
                                 if int(r["detected"]) == 0 and int(r["post_err"]) == 0),
            "misloc": sum(1 for r in faults
                          if int(r["detected"]) == 1 and int(r["loc_ok"]) == 0),
            "heal_fail": sum(1 for r in faults
                             if int(r["detected"]) == 1 and int(r["healed"]) == 0),
            # 对照场景的误报数（须为 0 才健康）
            "ctrl_false": sum(1 for r in ctrls if int(r["detected"]) != 0),
        }

    ai_s, rd_s = stats("ai"), stats("random")
    cells_total = 3 * (n + 1) * n
    cells_hit = len({(r["model"], r["col"], r["row"]) for r in log})

    summary = {
        "config": vars(args),
        "elapsed_s": dt,
        "total_scenarios": len(log),
        "coverage_cells": f"{cells_hit}/{cells_total}",
        "ai": ai_s,
        "random": rd_s,
        "worst_ai": sorted([r for r in log if r["source"] == "ai"],
                           key=lambda r: -float(r["harmscore"]))[:10],
        "worst_overall": sorted(log, key=lambda r: -float(r["harmscore"]))[:10],
    }
    (work / "ai_campaign_summary.json").write_text(
        json.dumps(summary, indent=2, ensure_ascii=False), encoding="utf-8")

    print("=" * 74)
    print(f"完成：{len(log)} 场景 / {dt:.1f}s，覆盖率 {cells_hit}/{cells_total} 单元")
    for tag, s in (("AI  ", ai_s), ("随机", rd_s)):
        print(f"  {tag} : 故障={s.get('n_fault')} 对照={s.get('n_ctrl')} "
              f"max危害={s.get('max_harm',0):.1f} | 逃逸={s.get('escaped')} "
              f"有害漏检={s.get('missed_harmful')} 良性掩蔽={s.get('missed_benign')} "
              f"误定位={s.get('misloc')} 未修复={s.get('heal_fail')} "
              f"对照误报={s.get('ctrl_false')}")
    print(f"产物： {log_path.name}, ai_campaign_summary.json（均在 {work}）")
    print("=" * 74)


if __name__ == "__main__":
    main()
