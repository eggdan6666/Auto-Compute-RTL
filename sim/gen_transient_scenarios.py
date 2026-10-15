#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""gen_transient_scenarios.py — 瞬态脉冲串场景网格生成器（P1 分支 B）

输出 transient_train_scen{N}.hex（192bit/行，见 gen_transient_train_tb.py 头注释）。

刻意**骑在设计硬常数棱边上**（WINDOW=32 / THRESH=3）：
  纯瞬态类（应不触发）：单脉冲；2 脉冲间隔 5/31/32/33（窗内计数 <=2 的所有边角）
  瞬态簇类（窗内 >=3，静态可能触发）：3~4 脉冲密集间隔
  永久类（应在有界时延内触发）：psum / w_reg 永久卡死
  混合类                           ：永久 + 瞬态

标签（地面真值）:
  0 对照(无注入)   1 纯瞬态可吸收   2 瞬态簇(窗内>=3)   3 永久硬故障   4 混合

用法:  python sim/gen_transient_scenarios.py 4
"""
import random
import sys

N = int(sys.argv[1]) if len(sys.argv) > 1 else 4
SEED = 20261015
L_CTRL, L_ABSORB, L_BURST, L_PERM, L_MIX = 0, 1, 2, 3, 4

VAL = 0x0000          # 单拍瞬态/永久卡死值：0（与 tb_transient_sweep 同口径，小偏差必被检出）


def ev(model, col, row, start, dur, val=VAL):
    return ((model & 3) << 38) | ((col & 7) << 35) | ((row & 7) << 32) | \
           ((start & 0xFF) << 24) | ((dur & 0xFF) << 16) | (val & 0xFFFF)


def scen(n_ev, label, events):
    v = (n_ev << 184) | (label << 176)
    for k, e in enumerate(events[:4]):
        v |= e << (136 - 40 * k)
    return f"{v:048X}"


def rnd_pe(rng):
    return rng.choice([1, 2]), rng.randrange(N + 1), rng.randrange(N)


def build():
    rng = random.Random(SEED)
    out = []

    # 0 对照
    for _ in range(20):
        out.append(scen(0, L_CTRL, []))

    # 1 纯瞬态可吸收（窗内计数 <= 2）
    for _ in range(20):                                   # 单脉冲
        m, c, r = rnd_pe(rng)
        out.append(scen(1, L_ABSORB, [ev(m, c, r, rng.randrange(5, 20), 1)]))
    for gap in (5, 31, 32, 33):                           # 2 脉冲，间隔骑 32 棱边
        for _ in range(12):
            m, c, r = rnd_pe(rng)
            s = rng.randrange(5, 12)
            out.append(scen(2, L_ABSORB, [ev(m, c, r, s, 1), ev(m, c, r, s + gap, 1)]))
    for gap in (33, 34, 40, 41):                          # 3 脉冲，两两间隔 >32
        for _ in range(8):
            m, c, r = rnd_pe(rng)
            s = rng.randrange(5, 12)
            out.append(scen(3, L_ABSORB,
                            [ev(m, c, r, s, 1), ev(m, c, r, s + gap, 1), ev(m, c, r, s + 2 * gap, 1)]))

    # 2 瞬态簇（窗内计数 >= 3，静态大概率触发）
    for gaps in ([5, 5], [8, 8], [3, 3], [10, 2]):
        for _ in range(10):
            m, c, r = rnd_pe(rng)
            s = rng.randrange(5, 15)
            out.append(scen(3, L_BURST, [ev(m, c, r, s, 1), ev(m, c, r, s + gaps[0], 1),
                                         ev(m, c, r, s + gaps[0] + gaps[1], 1)]))
    for _ in range(12):                                   # 4 脉冲密集
        m, c, r = rnd_pe(rng)
        s = rng.randrange(5, 12)
        out.append(scen(4, L_BURST, [ev(m, c, r, s, 1), ev(m, c, r, s + 2, 1),
                                     ev(m, c, r, s + 4, 1), ev(m, c, r, s + 6, 1)]))

    # 3 永久硬故障（应在有界时延内触发）
    for model in (1, 2):
        for _ in range(15):
            c, r = rng.randrange(N + 1), rng.randrange(N)
            out.append(scen(1, L_PERM, [ev(model, c, r, rng.randrange(5, 12), 0)]))

    # 4 混合：永久 + 2 瞬态
    for _ in range(15):
        c, r = rng.randrange(N + 1), rng.randrange(N)
        s = rng.randrange(5, 10)
        out.append(scen(3, L_MIX, [ev(1, c, r, s, 0), ev(1, c, r, s + 20, 1), ev(1, c, r, s + 40, 1)]))

    return out


if __name__ == "__main__":
    lines = build()
    with open(f"transient_train_scen{N}.hex", "w", encoding="ascii") as f:
        f.write("\n".join(lines) + "\n")
    import collections
    cnt = collections.Counter(int(l, 16) >> 176 & 0xFF for l in lines)
    print(f"transient_train_scen{N}.hex: {len(lines)} 场景, 标签分布 "
          f"{{0对照:{cnt[0]}, 1可吸收:{cnt[1]}, 2瞬态簇:{cnt[2]}, 3永久:{cnt[3]}, 4混合:{cnt[4]}}}, N={N}, seed={SEED}")
