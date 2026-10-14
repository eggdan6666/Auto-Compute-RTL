#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
golden_bist.py — Phase 3 自愈验证 Golden Oracle
================================================

为 self_healing_array (N 逻辑列 + 1 物理备用列) 生成:

1. bist_weights.hex   : BIST 物理权重矩阵 N x (N+1), 每行 16 bit
                        索引 r*(N+1)+p -> W_b[r][p] (物理列寻址)
2. bist_rows.hex      : BIST 激活行 ROM, MB 行, 每行 N*16 bit
                        位 [16*r +: 16] = a[r]
3. bist_golden.hex    : N+1 个物理列的 MISR16 黄金签名 (每行 16 bit)
                        签名 = 对该列 MB 个黄金输出字逐字压缩
4. mission_weights.hex: 任务模式逻辑权重 N x N (r*N+c)
5. mission_vectors.hex: 任务模式向量, M 行, 每行 2*N*16 bit
                        位 [16*(N+r)+:16]=A[m][r], 位 [16*c+:16]=C[m][c]

MISR16 (Fibonacci, 与 rtl/misr16.sv 逐位一致):
    fb = s[15]; s' = ((s<<1)&0xFFFF) ^ (fb ? 0x002D : 0) ^ data
    初值 0xACE1
"""

import random
from golden_model import mac_golden, to_signed

import sys as _sys
N   = int(_sys.argv[1]) if len(_sys.argv) > 1 else 4  # 逻辑列数 (= 行数)
MB  = 16      # BIST 激活行数
M   = 8       # 任务模式行数
MISR_INIT = 0xACE1
MISR_POLY = 0x002D


def s16(v):
    return to_signed(v, 16)


def misr16(state, data):
    fb = (state >> 15) & 1
    s = (state << 1) & 0xFFFF
    if fb:
        s ^= MISR_POLY
    return s ^ data


def golden_word(a_row, w_col):
    """单列黄金输出: sum_r a[r]*w_col[r] -> clamp40 -> RNE -> SAT16"""
    acc = sum(s16(a_row[r]) * s16(w_col[r]) for r in range(N))
    return mac_golden(0, 0, acc)


def main():
    rng = random.Random(0x5E1F)

    # --- BIST 激励: 4 行定向 (全最大正/全-1/最小正码/全-1码) + 12 行伪随机 ---
    W_b = [[rng.getrandbits(16) for _ in range(N + 1)] for _ in range(N)]
    rows = (
        [[0x7FFF] * N, [0x8000] * N, [0x0001] * N, [0xFFFF] * N]
        + [[rng.getrandbits(16) for _ in range(N)] for _ in range(MB - 4)]
    )
    assert len(rows) == MB

    # --- 每物理列黄金 MISR 签名 ---
    sigs = []
    for p in range(N + 1):
        w_col = [W_b[r][p] for r in range(N)]
        sig = MISR_INIT
        for m in range(MB):
            sig = misr16(sig, golden_word(rows[m], w_col))
        sigs.append(sig)

    # --- 任务模式: 逻辑 N x N 权重 + M 行激活 ---
    W_m = [[rng.getrandbits(16) for _ in range(N)] for _ in range(N)]
    edge_row1 = [0x4000, 0xC000, 0x2000, 0xE000, 0x1000, 0xF000, 0x0800, 0xF800][:N]
    edge_row2 = [0x7FFF, 0x8000, 0x0001, 0xFFFF, 0x0002, 0xFFFE, 0x5555, 0xAAAA][:N]
    mission_rows = (
        [edge_row1, edge_row2]
        + [[rng.getrandbits(16) for _ in range(N)] for _ in range(M - 2)]
    )
    mission_vec = []
    for m in range(M):
        C = [golden_word(mission_rows[m], [W_m[r][c] for r in range(N)]) for c in range(N)]
        v = 0
        for r in range(N):
            v |= mission_rows[m][r] << (16 * (N + r))
        for c in range(N):
            v |= C[c] << (16 * c)
        mission_vec.append(v)

    # --- 写文件 ---
    with open(f"bist{N}_weights.hex", "w", encoding="ascii") as f:
        for r in range(N):
            for p in range(N + 1):
                f.write(f"{W_b[r][p]:04X}\n")

    with open(f"bist{N}_rows.hex", "w", encoding="ascii") as f:
        for m in range(MB):
            v = 0
            for r in range(N):
                v |= rows[m][r] << (16 * r)
            f.write(f"{v:0{N*4}X}\n")

    with open(f"bist{N}_golden.hex", "w", encoding="ascii") as f:
        for p in range(N + 1):
            f.write(f"{sigs[p]:04X}\n")

    with open(f"mission{N}_weights.hex", "w", encoding="ascii") as f:
        for r in range(N):
            for c in range(N):
                f.write(f"{W_m[r][c]:04X}\n")

    with open(f"mission{N}_vectors.hex", "w", encoding="ascii") as f:
        for m in range(M):
            f.write(f"{mission_vec[m]:0{2*N*4}X}\n")

    print("=" * 80)
    print(f"Phase 3 Golden Oracle — self-healing array N={N} (+1 spare), MB={MB}, M={M}")
    print("=" * 80)
    for p in range(N + 1):
        print(f"  物理列 {p}: MISR16 黄金签名 = {sigs[p]:04X}")
    print(f"  任务模式首行: A={['%04X' % x for x in mission_rows[0]]}")
    print(f"                C={[f'{mission_vec[0] >> (16*c) & 0xFFFF:04X}' for c in range(N)]}")
    print("-" * 80)
    print("已生成: bist_weights.hex / bist_rows.hex / bist_golden.hex /")
    print("        mission_weights.hex / mission_vectors.hex")


if __name__ == "__main__":
    main()
