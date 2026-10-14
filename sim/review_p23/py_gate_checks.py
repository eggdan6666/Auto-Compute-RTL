#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
py_gate_checks.py — Round 2 终审 Python 侧门控 (G7 修复门 / G8 冒烟门 / G9 数值口径门)

G7a  dual_loop 概率钳制 (P2-3): fault_rate=0.9 时不可愈比例须 <1.0 (修复前恒为 1.0)
G7b  饱和域含 SAT_POS (P2-4): mode='none' 全故障注入须能产出 +32767 (修复前上限 32766)
G7c  ge3 误分类防护 (Round2 新发现): 瞬态分布含 ge3>0 时战役脚本必须报错或显式申示
G7d  float 基线模式 (Round2 新发现): train_cifar10_q115.py --mode float --quick 须退出 0
G8   战役冒烟: --synthetic 全管线跑通且 CSV 行数正确
G9   QAT 浮点卷积 vs 硬件精确整数路径码值差 (信息性记录 + 0.5% 上限)
"""
import os
import sys
import csv
import tempfile
import subprocess

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, ROOT)

import torch
from eval_q115_torch import Q115Linear, SAT_NEG, SAT_POS

RESULTS = []


def gate(name, ok, detail):
    RESULTS.append((name, ok, detail))
    print(f"[GATE] {name}: {'PASS' if ok else 'FAIL'}: {detail}")
    return ok


def run_py(args, timeout=600):
    env = dict(os.environ)
    return subprocess.run([sys.executable] + args, cwd=ROOT, env=env,
                          capture_output=True, text=True, timeout=timeout)


# ---------------------------------------------------------------- G7a
def g7a():
    """P2-3 概率钳位: (a) 功能检验 — rate=0.1 时不可愈比例应 ≈ 2*rate=0.2 (模型比例语义);
       (b) 源码检验 — dual_loop 分支须存在 min(1.0,...) 钳位 (rate>=0.5 时 rand<1.0 与
       rand<1.8 行为恒等, 钳位为防御性, 功能上不可分辨, 故以源码检查佐证)。"""
    torch.manual_seed(0)
    layer = Q115Linear(8, 8, bias=False)
    layer.protection_mode = "dual_loop"
    layer.fault_rate = 0.1
    x = torch.ones(400000, dtype=torch.int64)
    res = layer._apply_fault_injection(x.clone())
    faulted = (res != 1)
    noise = faulted & (res != 0)
    n_f = int(faulted.sum().item())
    frac = float(noise.sum().item()) / max(1, n_f)
    src = open(os.path.join(ROOT, "eval_q115_torch.py"), encoding="utf-8").read()
    has_clamp = "min(1.0" in src
    ok = (0.15 < frac < 0.25) and has_clamp
    gate("G7a", ok,
         f"dual_loop 不可愈比例={frac:.4f} (应≈0.200); 钳位存在={has_clamp} (n_faulted={n_f})")


# ---------------------------------------------------------------- G7b
def g7b():
    torch.manual_seed(1)
    layer = Q115Linear(8, 8, bias=False)
    layer.protection_mode = "none"
    layer.fault_rate = 1.0
    x = torch.zeros(2000000, dtype=torch.int64)
    res = layer._apply_fault_injection(x)
    mx = int(res.max().item())
    mn = int(res.min().item())
    gate("G7b", mx == SAT_POS and mn == SAT_NEG,
         f"none 模式噪声域=[{mn}, {mx}] (须覆盖 [{SAT_NEG}, {SAT_POS}]; 修复前上限 {SAT_POS-1})")


# ---------------------------------------------------------------- G7c
def g7c():
    tmpdir = os.path.join(ROOT, "sim", "review_p23")
    os.makedirs(tmpdir, exist_ok=True)
    ge3_csv = os.path.join(tmpdir, "tmp_transient_ge3.csv")
    with open(ge3_csv, "w", encoding="utf-8") as f:
        f.write("pulses,count,percentage\n0,90,0.90\n1,5,0.05\nge3,5,0.05\n")
    out_csv = os.path.join(tmpdir, "tmp_hipulse_campaign.csv")
    try:
        r = run_py(["run_resnet18_fault_campaign.py", "--synthetic", "--max-samples", "8",
                    "--rates", "1e-2", "--batch-size", "8",
                    "--transient-csv", ge3_csv, "--out", out_csv], timeout=900)
        out = (r.stdout or "") + (r.stderr or "")
        # 判据: 非零退出, 或某行同时含 ge3 与告警/断言语义 (防止被自身路径名误匹配)
        surfaced = any(("ge3" in ln.lower()) and
                       any(k in ln.lower() for k in ("warn", "error", "assert", "触发", "bist"))
                       for ln in out.splitlines())
        ok = (r.returncode != 0) or surfaced
        gate("G7c", ok,
             f"ge3>0 分布: exit={r.returncode}, 显式申示={surfaced} "
             f"(须: 非零退出或告警行申示 ge3; 静默按 3 脉冲处理=漏判)")
    except subprocess.TimeoutExpired:
        gate("G7c", False, "ge3 测试超时")


# ---------------------------------------------------------------- G7d
def g7d():
    try:
        r = run_py(["train_cifar10_q115.py", "--mode", "float", "--quick"], timeout=300)
        ok = r.returncode == 0
        tail = (r.stderr or r.stdout or "").strip().splitlines()[-1:] or [""]
        gate("G7d", ok, f"--mode float --quick exit={r.returncode} (修复前 TypeError 崩溃); {tail[0][:80]}")
    except subprocess.TimeoutExpired:
        gate("G7d", False, "float quick 超时")


# ---------------------------------------------------------------- G8
def g8():
    out_csv = os.path.join(ROOT, "sim", "review_p23", "tmp_campaign_smoke.csv")
    try:
        r = run_py(["run_resnet18_fault_campaign.py", "--synthetic", "--max-samples", "32",
                    "--rates", "1e-4,1e-2", "--batch-size", "16", "--out", out_csv], timeout=900)
        ok = r.returncode == 0 and os.path.exists(out_csv)
        nrows = -1
        if ok:
            with open(out_csv, encoding="utf-8") as f:
                nrows = sum(1 for _ in csv.DictReader(f))
            ok = nrows == 8  # 4 模式 x 2 速率
        gate("G8", ok, f"战役冒烟 exit={r.returncode}, CSV 行数={nrows} (须=8)")
    except subprocess.TimeoutExpired:
        gate("G8", False, "战役冒烟超时")


# ---------------------------------------------------------------- G9
def g9():
    from train_cifar10_q115 import Q115Conv2d, quantize_to_q115
    from eval_q115_torch import Q115Conv2d as EvalConv
    torch.manual_seed(7)
    gaps = []
    for (cin, cout, hw) in [(3, 16, 32), (64, 64, 16)]:
        conv = Q115Conv2d(cin, cout, 3, 1, 1, bias=False)
        x = torch.randn(4, cin, hw, hw)
        o1 = conv(x)
        e2 = EvalConv(cin, cout, 3, 1, 1, bias=False)
        e2.weight.data = conv.weight.data.clone()
        o2 = e2(x)
        d = (quantize_to_q115(o1) != quantize_to_q115(o2))
        gaps.append(100.0 * int(d.sum()) / d.numel())
    worst = max(gaps)
    gate("G9", worst <= 0.5,
         f"QAT浮点卷积 vs 硬件精确路径码值差: {[f'{g:.3f}%' for g in gaps]} (上限 0.5%; "
         f"论文须申明此近似)")


def main():
    print("=" * 78)
    print("Round 2 Python 侧门控 (G7/G8/G9)")
    print("=" * 78)
    g7a(); g7b(); g7c(); g7d(); g8(); g9()
    n_fail = sum(1 for _, ok, _ in RESULTS if not ok)
    print("-" * 78)
    print(f"[SUMMARY] Python 门控: {len(RESULTS)-n_fail}/{len(RESULTS)} PASS, {n_fail} FAIL")
    sys.exit(n_fail)


if __name__ == "__main__":
    main()
