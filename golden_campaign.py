#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
golden_campaign.py — E1 千组故障注入回归的激励生成与结果汇总
==============================================================

生成 fault_list.hex: 每行 48 bit (12 hex), 位段:
  [47:46] model  : 0=无故障对照, 1=psum_out 卡死, 2=w_reg 卡死
  [45:43] col    : 物理列 0..4 (4 为备用列)
  [42:40] row    : PE 行 0..3
  [39:0]  val    : 卡死值

campaign_results.csv 汇总 -> 检出率/定位率/修复率/误报率/良性掩蔽率
"""

import random
import csv
import sys

N_FAULT = 1000
N_CTRL  = 100
SEED    = 20261010


def gen_fault_list(path="fault_list.hex"):
    rng = random.Random(SEED)
    lines = []
    # 故障场景
    for _ in range(N_FAULT):
        model = rng.choice([1, 1, 2])     # psum 卡死 2/3, 权重卡死 1/3
        col   = rng.randrange(5)          # 含备用列 4
        row   = rng.randrange(4)
        val   = rng.getrandbits(40) if model == 1 else rng.getrandbits(16)
        entry = (model << 46) | (col << 43) | (row << 40) | val
        lines.append(f"{entry:012X}")
    # 对照场景 (无故障)
    for _ in range(N_CTRL):
        lines.append(f"{0:012X}")
    with open(path, "w", encoding="ascii") as f:
        f.write("\n".join(lines) + "\n")
    print(f"fault_list.hex: {len(lines)} 场景 ({N_FAULT} 故障 + {N_CTRL} 对照), seed={SEED}")


def aggregate(path="campaign_results.csv"):
    """列: id,model,col,row,val,pre_err,detected,loc_ok,healed,post_err"""
    rows = list(csv.DictReader(open(path, encoding="utf-8")))
    faults = [r for r in rows if int(r["model"]) != 0]
    ctrls  = [r for r in rows if int(r["model"]) == 0]

    def pct(x, n):
        return 100.0 * x / n if n else float("nan")

    det    = sum(1 for r in faults if int(r["detected"]) == 1)
    loc    = sum(1 for r in faults if int(r["loc_ok"]) == 1)
    healed = sum(1 for r in faults if int(r["healed"]) == 1 and int(r["post_err"]) == 0)
    # 未检出细分: 良性掩蔽(任务仍正确) vs 有害漏检(任务出错)
    missed_benign  = sum(1 for r in faults if int(r["detected"]) == 0 and int(r["post_err"]) == 0)
    missed_harmful = sum(1 for r in faults if int(r["detected"]) == 0 and int(r["post_err"]) != 0)
    pre_bad  = sum(1 for r in faults if int(r["pre_err"]) != 0)
    post_bad = sum(1 for r in faults if int(r["post_err"]) != 0)
    ctrl_false = sum(1 for r in ctrls if int(r["detected"]) != 0)
    ctrl_clean = sum(1 for r in ctrls if int(r["pre_err"]) == 0 and int(r["post_err"]) == 0)

    n, m = len(faults), len(ctrls)
    print("=" * 72)
    print(f"E1 故障注入回归汇总: {n} 故障场景 + {m} 对照场景")
    print("=" * 72)
    print(f"检出率        : {det}/{n} = {pct(det,n):.2f}%")
    print(f"定位准确率    : {loc}/{det} = {pct(loc,det):.2f}% (在已检出中)")
    print(f"修复成功率    : {healed}/{n} = {pct(healed,n):.2f}% (检出且修复后任务全对)")
    print(f"未检出-良性掩蔽: {missed_benign}/{n} = {pct(missed_benign,n):.2f}% (任务输出仍全对)")
    print(f"未检出-有害漏检: {missed_harmful}/{n} = {pct(missed_harmful,n):.2f}% (MISR 混叠/掩蔽失效)")
    print(f"自愈前任务错误场景: {pre_bad}/{n} = {pct(pre_bad,n):.2f}%  (基线: 无自愈时)")
    print(f"自愈后任务错误场景: {post_bad}/{n} = {pct(post_bad,n):.2f}%")
    print(f"对照组误报率  : {ctrl_false}/{m} = {pct(ctrl_false,m):.2f}%")
    print(f"对照组全对率  : {ctrl_clean}/{m} = {pct(ctrl_clean,m):.2f}%")
    return {
        "n_fault": n, "n_ctrl": m, "detected": det, "loc_ok": loc,
        "healed_ok": healed, "missed_benign": missed_benign,
        "missed_harmful": missed_harmful, "pre_bad": pre_bad,
        "post_bad": post_bad, "ctrl_false": ctrl_false,
    }


if __name__ == "__main__":
    if "--aggregate" in sys.argv:
        aggregate()
    else:
        gen_fault_list()
