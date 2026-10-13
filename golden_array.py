#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
golden_array.py — Auto-Compute-RTL Phase 2 脉动阵列独立 Golden Oracle
=====================================================================

严格依据《systolic_array_spec.md》计算 C = A x W:
  - PE 部分和: 纯整数精确累加 sum_r A[m][r]*W[r][c] (Q2.30 对齐, 30 位小数)
  - 列出口: 复用 golden_model.mac_golden(0,0,sum) —— 该核已经
    Fraction 精确 RNE 全组合自检, 含 clamp40 + RNE + 16 位饱和

输出 ($readmemh 可直接读取):
  array_weights.hex : S*N*N 行, 每行 16 bit, 索引 s*N*N + r*N + c -> W[r][c]
  array_vectors.hex : S*M 行, 每行 2*N*16 bit:
                        位 [16*(N+r) +: 16] = A[m][r]  (输入)
                        位 [16*c     +: 16] = C[m][c]  (黄金预期)
"""

from golden_model import mac_golden, to_signed

N = 4   # 阵列规模 (与 tb_systolic_array 的 localparam N 一致)
M = 8   # 每个场景的激活行数

EDGE16 = [0x0000, 0x0001, 0x7FFF, 0x8000, 0x8001, 0xFFFF, 0x4000, 0xC000]


def s16(v):
    return to_signed(v, 16)


def golden_row(a_row, w_mat):
    """a_row: N 个 16 位无符号码; w_mat: NxN (无符号码). 返回 N 个 16 位输出码"""
    out = []
    for c in range(N):
        acc = sum(s16(a_row[r]) * s16(w_mat[r][c]) for r in range(N))
        out.append(mac_golden(0, 0, acc))  # clamp40 + RNE + SAT16
    return out


# ---------------------------------------------------------------------------
# 场景矩阵
# ---------------------------------------------------------------------------
def build_scenarios():
    scn = []

    # S0 单位阵 —— 验证波前路由: C 的每个元素只含一个乘积项
    W = [[0x7FFF if r == c else 0x0000 for c in range(N)] for r in range(N)]  # ≈I (0x7FFF≈1.0)
    # 注: 对角用 0x7FFF (~0.99997) 而非不可表示的 1.0, 顺带考验单项目舍入
    A = [
        [0x4000, 0xC000, 0x2000, 0xE000],
        [0x7FFF, 0x8000, 0x0001, 0xFFFF],
        [0x0000, 0x0000, 0x0000, 0x0000],
        [0x1234, 0x5678, 0x9ABC, 0xDEF0],
        [0x0001, 0x0002, 0x0004, 0x0008],
        [0xFFFF, 0xFFFE, 0xFFFC, 0xFFF8],
        [0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF],
        [0x8000, 0x8000, 0x8000, 0x8000],
    ]
    scn.append(("S0 near-identity", W, A))

    # S1 全最大正 —— 4 路最大正乘积累加 -> 正向饱和
    W = [[0x7FFF] * N for _ in range(N)]
    A = [[0x7FFF] * N for _ in range(M)]
    scn.append(("S1 all-max-pos saturate+", W, A))

    # S2 全 -1 x -1 —— 每个乘积 +2^30, 4 路累加 = 2^32, 考验 40 位 Guard Bits
    W = [[0x8000] * N for _ in range(N)]
    A = [[0x8000] * N for _ in range(M)]
    scn.append(("S2 (-1)x(-1) x4 guard-bits", W, A))

    # S3 全最大负乘积 —— 4 路 -32768*32767 累加 -> 负向饱和
    W = [[0x7FFF] * N for _ in range(N)]
    A = [[0x8000] * N for _ in range(M)]
    scn.append(("S3 all-max-neg saturate-", W, A))

    # S4 RNE 中点奇偶分流 —— 对角 W=0.5(0x4000), 小整数激活产生 x.5 LSB 中点
    W = [[0x4000 if r == c else 0x0000 for c in range(N)] for r in range(N)]
    half = [0x0001, 0x0003, 0x0005, 0xFFFF, 0xFFFD, 0xFFFB, 0x0002, 0xFFFE]
    # a*0.5: 1->+0.5LSB->0, 3->1.5->2, 5->2.5->2, -1->-0.5->0, -3->-1.5->-2, -5->-2.5->-2, 2->1.0->1, -2->-1.0->-1
    A = [[half[(m + c) % 8] for c in range(N)] for m in range(M)]
    scn.append(("S4 RNE midpoint odd/even", W, A))

    # S5 全幅伪随机 (固定种子, 可复现)
    import random
    rng = random.Random(20261010)
    W = [[rng.getrandbits(16) for _ in range(N)] for _ in range(N)]
    A = [[rng.getrandbits(16) for _ in range(N)] for _ in range(M)]
    scn.append(("S5 random full-range", W, A))

    return scn


def pack_vector(a_row, c_row):
    """位 [16*(N+r)+:16]=a[r], 位 [16*c+:16]=c[c]"""
    v = 0
    for r in range(N):
        v |= a_row[r] << (16 * (N + r))
    for c in range(N):
        v |= c_row[c] << (16 * c)
    return v


def main():
    scn = build_scenarios()
    print("=" * 96)
    print(f"Phase 2 Golden Oracle — Systolic Array {N}x{N}, {len(scn)} scenarios x {M} rows")
    print("=" * 96)

    w_lines, v_lines = [], []
    for si, (name, W, A) in enumerate(scn):
        print(f"[S{si}] {name}")
        for r in range(N):
            for c in range(N):
                w_lines.append(f"{W[r][c]:04X}")
        for m in range(M):
            C = golden_row(A[m], W)
            v_lines.append(f"{pack_vector(A[m], C):032X}")
            if m < 4 or si in (1, 2, 3):  # 饱和场景全打印, 其余打印前 4 行
                astr = " ".join(f"{x:04X}" for x in A[m])
                cstr = " ".join(f"{x:04X}" for x in C)
                print(f"  row{m}: A=[{astr}] -> C=[{cstr}]")

    with open("array_weights.hex", "w", encoding="ascii") as f:
        f.write("\n".join(w_lines) + "\n")
    with open("array_vectors.hex", "w", encoding="ascii") as f:
        f.write("\n".join(v_lines) + "\n")

    print("-" * 96)
    print(f"array_weights.hex: {len(w_lines)} 行 (16 bit/行)")
    print(f"array_vectors.hex: {len(v_lines)} 行 ({2*N*16} bit/行 = 输入行+黄金输出行)")


if __name__ == "__main__":
    main()
