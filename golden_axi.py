#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
golden_axi.py — Phase 4 AXI4-Stream IP 独立 Golden Oracle
==========================================================

生成流式激励与黄金输出 (与 axi_stream_array_spec.md 的拍编码一致):

  axi_in.hex : 45 行输入拍, 每行 67 bit (17 hex):
                 位 [63:0] = tdata, 位 [65:64] = tuser, 位 [66] = tlast
               序列: 16 权重拍 + 6 激活拍(tlast) + 1 自愈命令拍
                     + 16 权重拍(修复后重装) + 6 激活拍(tlast)
  axi_out_golden.hex : 12 行黄金输出拍, 每行 65 bit (17 hex):
                 位 [63:0] = tdata (整行 C[m][:]), 位 [64] = tlast
"""

import random
from golden_model import mac_golden, to_signed

N = 4
M = 6            # 每矩阵行数
TU_ACT, TU_W, TU_CMD = 0b00, 0b01, 0b10


def s16(v):
    return to_signed(v, 16)


def golden_word(a_row, w_col):
    acc = sum(s16(a_row[r]) * s16(w_col[r]) for r in range(N))
    return mac_golden(0, 0, acc)


def in_beat(tuser, tlast, tdata):
    return (tlast << 66) | (tuser << 64) | tdata


def main():
    rng = random.Random(0xA510)

    W = [[rng.getrandbits(16) for _ in range(N)] for _ in range(N)]
    rows = (
        [[0x4000, 0xC000, 0x2000, 0xE000], [0x7FFF, 0x8000, 0x0001, 0xFFFF]]
        + [[rng.getrandbits(16) for _ in range(N)] for _ in range(M - 2)]
    )

    beats = []
    # 阶段 1: 权重装载
    for r in range(N):
        for c in range(N):
            beats.append(in_beat(TU_W, 0, W[r][c]))
    # 阶段 2: 矩阵 1
    for m in range(M):
        tdata = 0
        for r in range(N):
            tdata |= rows[m][r] << (16 * r)
        beats.append(in_beat(TU_ACT, 1 if m == M - 1 else 0, tdata))
    # 阶段 3: 自愈命令
    beats.append(in_beat(TU_CMD, 0, 1))
    # 阶段 4: 权重重装 (BIST 覆盖了驻留权重)
    for r in range(N):
        for c in range(N):
            beats.append(in_beat(TU_W, 0, W[r][c]))
    # 阶段 5: 矩阵 2 (修复后复测)
    for m in range(M):
        tdata = 0
        for r in range(N):
            tdata |= rows[m][r] << (16 * r)
        beats.append(in_beat(TU_ACT, 1 if m == M - 1 else 0, tdata))

    gold = []
    for _ in range(2):                      # 两个矩阵的黄金输出相同
        for m in range(M):
            tdata = 0
            for c in range(N):
                tdata |= golden_word(rows[m], [W[r][c] for r in range(N)]) << (16 * c)
            gold.append(((1 if m == M - 1 else 0) << 64) | tdata)

    with open("axi_in.hex", "w", encoding="ascii") as f:
        for b in beats:
            f.write(f"{b:017X}\n")
    with open("axi_out_golden.hex", "w", encoding="ascii") as f:
        for g in gold:
            f.write(f"{g:017X}\n")

    print(f"axi_in.hex: {len(beats)} 拍 (16W + {M}act + 1cmd + 16W + {M}act)")
    print(f"axi_out_golden.hex: {len(gold)} 拍 (2 矩阵 x {M} 行)")
    print(f"矩阵首行黄金输出: {gold[0] & (2**64-1):016X}")


if __name__ == "__main__":
    main()
