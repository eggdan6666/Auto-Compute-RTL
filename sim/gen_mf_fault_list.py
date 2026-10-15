#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""gen_mf_fault_list.py — 丰富故障清单生成器（P1 分支 A，配合 gen_mf_campaign_tb.py）

输出 fault_list_mf{N}.hex（128bit/行，见 gen_mf_campaign_tb.py 头注释），
按"场景类别"分层构造，覆盖 E1 单故障空间压不到的角落：

  cat=0  对照（nf=0）                    —— 期望: detected=0, unhealable=0, post_err=0
  cat=1  单故障（复现 E1 基线）           —— 期望: detected=1, healed=1, post_err=0
  cat=2  同列双故障（两故障同物理列）      —— 期望: healed=1（同列仍属"单列故障"）
  cat=3  跨列双故障（两故障不同物理列）    —— 期望: detected=1 且 unhealable=1（合同预期，非危害）
  cat=4  备用列(col=N) × 活跃列           —— 探查备用列兼作 ABFT 校验列时被污染
  cat=5  同 PE 双故障(同 col&row, 不同 model) —— 单 PE 上 psum 与 w 同时卡死

用法:  python sim/gen_mf_fault_list.py 4
"""
import random
import sys

N = int(sys.argv[1]) if len(sys.argv) > 1 else 4
N1 = N + 1
SEED = 20261015

CTRL, SINGLE, SAME_COL, CROSS_COL, SPARE, SAME_PE = 0, 1, 2, 3, 4, 5
COUNTS = {CTRL: 40, SINGLE: 120, SAME_COL: 100, CROSS_COL: 100, SPARE: 80, SAME_PE: 60}


def fault(model, col, row, val):
    return (model << 46) | (col << 43) | (row << 40) | (val & ((1 << 40) - 1))


def pack(nf, f1, f2, cat):
    return f"{((nf << 120) | (f1 << 72) | (f2 << 24) | (cat << 16)):032X}"


def rand_fault(rng):
    model = rng.choice([1, 1, 2])
    col = rng.randrange(N1)
    row = rng.randrange(N)
    val = rng.getrandbits(40) if model == 1 else rng.getrandbits(16)
    return fault(model, col, row, val), model, col, row


def main():
    rng = random.Random(SEED)
    lines = []

    # cat=0 对照
    for _ in range(COUNTS[CTRL]):
        lines.append(pack(0, 0, 0, CTRL))

    # cat=1 单故障
    for _ in range(COUNTS[SINGLE]):
        f, *_ = rand_fault(rng)
        lines.append(pack(1, f, 0, SINGLE))

    # cat=2 同列双故障（**两个故障必须落在同一物理列**，行可同可不同）
    for _ in range(COUNTS[SAME_COL]):
        col = rng.randrange(N1)
        m1, r1 = rng.choice([1, 1, 2]), rng.randrange(N)
        v1 = rng.getrandbits(40) if m1 == 1 else rng.getrandbits(16)
        m2, r2 = rng.choice([1, 1, 2]), rng.randrange(N)
        v2 = rng.getrandbits(40) if m2 == 1 else rng.getrandbits(16)
        lines.append(pack(2, fault(m1, col, r1, v1), fault(m2, col, r2, v2), SAME_COL))

    # cat=3 跨列双故障（列不同）
    for _ in range(COUNTS[CROSS_COL]):
        c1 = rng.randrange(N1)
        c2 = rng.choice([c for c in range(N1) if c != c1])
        r1, r2 = rng.randrange(N), rng.randrange(N)
        m1, m2 = rng.choice([1, 2]), rng.choice([1, 2])
        v1 = rng.getrandbits(40) if m1 == 1 else rng.getrandbits(16)
        v2 = rng.getrandbits(40) if m2 == 1 else rng.getrandbits(16)
        lines.append(pack(2, fault(m1, c1, r1, v1), fault(m2, c2, r2, v2), CROSS_COL))

    # cat=4 备用列(col=N) × 活跃列(col<N) —— 备用列兼作 ABFT 校验列
    for _ in range(COUNTS[SPARE]):
        r_spare, r_act = rng.randrange(N), rng.randrange(N)
        m_s, m_a = rng.choice([1, 2]), rng.choice([1, 2])
        v_s = rng.getrandbits(40) if m_s == 1 else rng.getrandbits(16)
        v_a = rng.getrandbits(40) if m_a == 1 else rng.getrandbits(16)
        lines.append(pack(2, fault(m_s, N, r_spare, v_s), fault(m_a, rng.randrange(N), r_act, v_a), SPARE))

    # cat=5 同 PE 双故障（同 col & row，模型一个 1 一个 2）
    for _ in range(COUNTS[SAME_PE]):
        col, row = rng.randrange(N1), rng.randrange(N)
        f1 = fault(1, col, row, rng.getrandbits(40))
        f2 = fault(2, col, row, rng.getrandbits(16))
        lines.append(pack(2, f1, f2, SAME_PE))

    with open(f"fault_list_mf{N}.hex", "w", encoding="ascii") as f:
        f.write("\n".join(lines) + "\n")

    total = sum(COUNTS.values())
    print(f"fault_list_mf{N}.hex: {total} 场景 "
          f"(ctrl {COUNTS[CTRL]}, single {COUNTS[SINGLE]}, same_col {COUNTS[SAME_COL]}, "
          f"cross_col {COUNTS[CROSS_COL]}, spare {COUNTS[SPARE]}, same_pe {COUNTS[SAME_PE]}), N={N}, seed={SEED}")


if __name__ == "__main__":
    main()
