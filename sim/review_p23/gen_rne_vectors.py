#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""复审对照实验: rne_sat16_torch (eval_q115_torch.py) vs 纯整数参考模型 vs RTL rne_sat16.sv
生成边界+随机 40-bit 向量, 供 SV TB 灌入 rne_sat16, 三方逐位比对。"""
import sys, random
sys.path.insert(0, ".")
from eval_q115_torch import rne_sat16_torch, MIN40, MAX40

random.seed(20261014)

vals = []
# --- 边界/角点 ---
for v in [0, 1, -1, 2, -2, 0x3FFF, 0x4000, 0x4001, -0x3FFF, -0x4000, -0x4001,
          0x7FFF, 0x8000, 0x8001, -0x7FFF, -0x8000, -0x8001,
          0xFFFE, 0xFFFF, 0x10000, -0xFFFE, -0xFFFF, -0x10000,
          0x3FFF8000, 0x40008000, 0x3FFF0000, 0x3FFFC000,  # 1.5/1.4999/1.25/1.49994 等舍入边界
          -0x3FFF8000, -0x40008000, -0x4000C000,
          0x7FFF0000, 0x7FFF8000, 0x80000000, 0x80008000,  # 饱和轨附近
          -0x7FFF0000, -0x7FFF8000, -0x80000000, -0x80008000,
          MIN40, MAX40, MIN40 + 1, MAX40 - 1, MIN40 + 0x8000, MAX40 - 0x8000,
          0x100000000, -0x100000000, 0x40000000, -0x40000000]:
    vals.append(v)
# --- .tie 用例: G=1, St=0, L=0/1 (精确半点) ---
for base in [0x4000 * k for k in range(-4, 5)]:
    vals.append(base)                      # 整数, G=0
    vals.append(base + 0x2000)             # G=1,St=0,L=0 -> 偶数保持
    vals.append(base + 0x2000 + 0x8000)    # G=1,St=0,L=1 -> 进位
    vals.append(base + 0x2001)             # G=1,St=1 -> 进位
    vals.append(base - 0x2000)             # G=1,St=0 (负侧), L 视 base 而定
    vals.append(base - 0x2001)
# --- 随机 ---
for _ in range(2000):
    vals.append(random.randint(MIN40, MAX40))
for _ in range(500):
    vals.append(random.randint(-0x20000, 0x20000))   # 饱和区附近密集

def ref_rne_sat16(acc: int) -> int:
    """独立纯整数参考模型 (不依赖 torch): 40-bit clamp -> RNE -> SAT16"""
    s = max(MIN40, min(MAX40, acc))
    q_floor = s >> 15            # Python // 对负数为 floor, 等价算术右移
    F = s & 0x7FFF
    G = (F >> 14) & 1
    St = 1 if (F & 0x3FFF) else 0
    L = q_floor & 1
    inc = G & (St | L)
    q = q_floor + inc
    return max(-32768, min(32767, q))

with open("sim/review_p23/rne_in.hex", "w") as f:
    for v in vals:
        f.write(f"{v & 0xFFFFFFFFFF:010X}\n")

exp_torch = rne_sat16_torch.__wrapped__ if hasattr(rne_sat16_torch, "__wrapped__") else None
import torch
t = torch.tensor(vals, dtype=torch.int64)
out_t = rne_sat16_torch(t).tolist()
out_r = [ref_rne_sat16(v) for v in vals]

mismatch_tr = [(v, a, b) for v, a, b in zip(vals, out_t, out_r) if a != b]
print(f"[gen] {len(vals)} vectors; torch-vs-ref mismatches: {len(mismatch_tr)}")
for v, a, b in mismatch_tr[:10]:
    print(f"   MISMATCH in={v:#x} torch={a} ref={b}")

with open("sim/review_p23/rne_exp_ref.hex", "w") as f:
    for o in out_r:
        f.write(f"{o & 0xFFFF:04X}\n")
print("[gen] wrote rne_in.hex / rne_exp_ref.hex")
