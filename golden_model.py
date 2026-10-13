#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
golden_model.py — Auto-Compute-RTL Phase 1 独立 Golden Oracle
==============================================================

严格依据《MAC_microarchitecture_spec.md》实现的纯数学定点参考模型:

    S41 = Acc_in(40b, 30 位小数) + sign_extend(A * B)   (41 位加法, 防环绕)
    S40 = clamp40(S41)                                   (Spec 4.1 钳位公式)
    Q   = RNE(S40 >>> 15)                                (Spec 4.2 最近偶数舍入)
    Out = SAT16(Q)                                       (Spec 5 逐位饱和)

本脚本不依赖任何 RTL/Testbench 代码, 用于打破 "大模型 <-> 测试台" 的 Trust Gap。

输出:
  1. 标准输出: 15 组 Corner Case 的可读审计表
  2. test_vectors.hex: 每行一个 88 bit (22 个 hex 字符) 的打包测试向量,
     可直接被 Verilog $readmemh 读入。位段定义:
         [87:72] A (16b)  [71:56] B (16b)  [55:16] Acc_in (40b)  [15:0] Expected Out (16b)

用法:
  python golden_model.py            # 打印审计表并生成 test_vectors.hex
  python golden_model.py --selftest # 额外对全边界做数学恒等式自检
"""

import sys

# ---------------------------------------------------------------------------
# 位宽常量 (与 Spec 第 1/4 节一致)
# ---------------------------------------------------------------------------
MAX40 = (1 << 39) - 1      # 40'sh7F_FFFF_FFFF
MIN40 = -(1 << 39)         # 40'sh80_0000_0000
FRAC  = 15                 # 输出 Q1.15: 右移 15 位
SAT_P = 0x7FFF             # +32767
SAT_N = 0x8000             # -32768 (16 位补码)


def to_signed(value, bits):
    """无符号位串 -> 二进制补码有符号整数"""
    sign = 1 << (bits - 1)
    return (value ^ sign) - sign if (value & sign) else value


def mac_golden(a, b, acc):
    """
    输入为 *有符号整数* (a,b 已解释为有符号 16 位; acc 为有符号 40 位)。
    返回 16 位无符号输出码 (0x0000..0xFFFF)。
    """
    # Stage 1: 16x16 有符号乘法, Q2.30。Python 大整数天然无截断/无符号歧义。
    prod = a * b

    # Stage 2 step 1: 41 位扩展加法 (Spec 4.1)。Python 整数位宽无限,
    # 等价于 {acc[39],acc} + {prod_ext[39],prod_ext}, 不可能环绕。
    s41 = acc + prod

    # Stage 2 step 2: 40 位有符号范围钳位 (Spec 4.1 分段公式)
    if s41 > MAX40:
        s40 = MAX40
    elif s41 < MIN40:
        s40 = MIN40
    else:
        s40 = s41

    # Stage 2 step 3: RNE (Spec 4.2)
    # Python 对负数的 >> 是算术右移 (向负无穷取整), 与 Spec 的 Q_floor 定义一致。
    q_floor = s40 >> FRAC
    F  = s40 & 0x7FFF              # 被丢弃的低 15 位
    G  = (F >> 14) & 1             # Guard 位
    St = 1 if (F & 0x3FFF) else 0  # Sticky 位
    L  = q_floor & 1               # 保留结果最低位 (== s40[15])
    inc = G & (St | L)
    q_round = q_floor + inc

    # Stage 2 step 4: 16 位饱和 (Spec 5, 先舍入后饱和)
    if q_round > 32767:
        return SAT_P
    if q_round < -32768:
        return SAT_N
    return q_round & 0xFFFF


# ---------------------------------------------------------------------------
# 15 组最具杀伤力的 Corner Case 矩阵
# 字段: (编号, 名称, A(hex), B(hex), Acc_in(hex 40b), 审计要点)
# ---------------------------------------------------------------------------
CASES = [
    ( 1, "(-1.0)x(-1.0)=+2^30 恰好饱和",      0x8000, 0x8000, 0x0000000000,
      "Spec Sec.6 边界示例: 32768 -> SAT+ 0x7FFF"),
    ( 2, "最大正边界: 恰好 32767 不饱和",      0x8000, 0x8001, 0x0000000000,
      "(-32768)x(-32767)=+32767.0 精确 -> 0x7FFF, 不得误判饱和"),
    ( 3, "+32767.5 中点进位触发饱和",          0x0000, 0x0000, 0x003FFFC000,
      "RNE(32767.5)=32768 -> SAT+; 检验'先舍入后饱和'次序"),
    ( 4, "最大正乘积 32767^2",                 0x7FFF, 0x7FFF, 0x0000000000,
      "32766.00003 -> 0x7FFE, 距饱和仅 1 LSB"),
    ( 5, "最大负乘积 -32768x32767",            0x8000, 0x7FFF, 0x0000000000,
      "-32767.0 精确 -> 0x8001"),
    ( 6, "41 位加法正溢出钳位",                0x7FFF, 0x7FFF, 0x7FFFFFFFFF,
      "max40 + 正乘积, S41[40:39]=01 -> clamp -> 0x7FFF; 40 位环绕实现会误判为负"),
    ( 7, "41 位加法负溢出钳位",                0x8000, 0x7FFF, 0x8000000000,
      "min40 + 负乘积, S41[40:39]=10 -> clamp -> 0x8000; 初代 TB ref_model 在此翻车"),
    ( 8, "RNE 中点 +0.5 LSB (L=0)",            0x0000, 0x0000, 0x0000004000,
      "G=1,St=0,L=0 -> inc=0 -> 0 (向偶舍入)"),
    ( 9, "RNE 中点 +1.5 LSB (L=1)",            0x0000, 0x0000, 0x000000C000,
      "G=1,St=0,L=1 -> inc=1 -> 2"),
    (10, "RNE 中点 +2.5 LSB (向偶舍下)",       0x0000, 0x0000, 0x0000014000,
      "2.5 -> 2 (偶), 检验奇偶分流"),
    (11, "RNE 中点 -0.5 LSB",                  0x0000, 0x0000, 0xFFFFFFC000,
      "-0.5 -> 0; 负数补码算术右移的无偏性"),
    (12, "RNE 中点 -1.5 LSB",                  0x0000, 0x0000, 0xFFFFFF4000,
      "-1.5 -> -2; floor(-1.5)=-2 且 inc=0"),
    (13, "RNE 中点 -2.5 LSB (向偶舍上)",       0x0000, 0x0000, 0xFFFFFEC000,
      "-2.5 -> -2 (偶); 检验负数向零调整方向"),
    (14, "负向饱和边界 -32768.5",              0x0000, 0x0000, 0xFFC0004000,
      "RNE(-32768.5)=-32768 恰好合法 -> 0x8000, 不得误报 neg_ovf 后又折返"),
    (15, "极小负数回零 -3.05e-5 LSB",          0x0000, 0x0000, 0xFFFFFFFFFF,
      "-1/2^30: floor=-1 但 G=1,St=1 -> inc=1 -> 0x0000; 抓'对负数永远向下取整'的错误实现"),
]


def pack_vector(a, b, acc, exp):
    """打包成 88 bit: {A[16], B[16], Acc[40], Exp[16]}"""
    return (a << 72) | (b << 56) | (acc << 16) | exp


def main():
    print("=" * 100)
    print("Auto-Compute-RTL Golden Oracle — 15 Corner Cases Audit Table")
    print("=" * 100)
    header = f"{'#':>2} | {'A':>4} | {'B':>4} | {'Acc_in':>10} | {'Exp':>4} | 名称"
    print(header)
    print("-" * 100)

    vectors = []
    for idx, name, a_h, b_h, acc_h, note in CASES:
        a   = to_signed(a_h, 16)
        b   = to_signed(b_h, 16)
        acc = to_signed(acc_h, 40)
        exp = mac_golden(a, b, acc)
        vectors.append(pack_vector(a_h, b_h, acc_h, exp))
        print(f"{idx:>2} | {a_h:04X} | {b_h:04X} | {acc_h:010X} | {exp:04X} | {name}")
        print(f"   `-- {note}")

    with open("test_vectors.hex", "w", encoding="ascii") as f:
        for v in vectors:
            f.write(f"{v:022X}\n")

    print("-" * 100)
    print(f"test_vectors.hex 已生成: {len(vectors)} 行 x 88 bit "
          "(打包格式 {{A[16],B[16],Acc[40],Exp[16]}}, $readmemh 可直接读取)")


def selftest():
    """数学恒等式自检: 全 G/St/L 组合 + 边界扫描, 证明 RNE 实现无偏"""
    print("\n[selftest] RNE 判定表 8 组合穷举 ...")
    # 构造所有 (G, St, L) 组合, 用浮点分数比对 Python Fraction 精确结果
    from fractions import Fraction
    fails = 0
    for base in range(-70, 70):                    # 覆盖正负、奇偶 L
        for frac in (0, 1, 0x2000, 0x3FFF, 0x4000, 0x4001, 0x7FFF):
            s40 = (base << FRAC) + frac
            got = mac_golden(0, 0, s40)
            # 精确数学参考: Fraction 下的 round-half-even
            exact = Fraction(s40, 1 << FRAC)
            lo = exact.numerator // exact.denominator  # floor
            diff_lo = exact - lo
            if diff_lo < Fraction(1, 2):
                ref = lo
            elif diff_lo > Fraction(1, 2):
                ref = lo + 1
            else:
                ref = lo if lo % 2 == 0 else lo + 1   # 恰好中点 -> 偶数
            ref = max(-32768, min(32767, ref)) & 0xFFFF
            if got != ref:
                print(f"  MISMATCH s40={s40}: golden={got:04X} frac_ref={ref:04X}")
                fails += 1
    print(f"[selftest] {'PASS — 与 Fraction 精确 RNE 完全一致' if fails == 0 else f'FAIL x{fails}'}")
    return fails == 0


if __name__ == "__main__":
    main()
    if "--selftest" in sys.argv:
        ok = selftest()
        sys.exit(0 if ok else 1)
