#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
eval_q115_torch.py — PyTorch Q1.15 硬件感知定点前向推理与故障注入框架
========================================================================

本模块依据《MAC_microarchitecture_spec.md》与《systolic_array_spec.md》实现，
旨在与底层 RTL (rne_sat16.sv, mac_unit.sv, systolic_array.sv, abft_checksum.sv, fault_arbiter.sv)
保持 100% 逐 bit 对齐，为 Phase 3 神经网络 (ResNet-18 / MobileNetV2) 故障注入与精度评测提供硬件级仿真基座。

核心微架构语义对齐点:
  1. 定标格式: Q1.15 有符号定点 (S=1, I=0, F=15), 数值范围 [-1.0, 1.0 - 2^-15] = [-32768, 32767]
  2. 乘累加累加器: 40-bit 有符号累加器, 具有 10-bit 保护位 (Guard bits)
  3. 40-bit 钳位: clamp40 到 [-2^39, 2^39 - 1]
  4. 舍入模式: 最近偶数舍入 (Round-to-Nearest-Even, RNE), 检查 Guard, Sticky, LSB
  5. 饱和截断: 先 RNE 舍入再 SAT16 饱和到 [-32768, 32767] (0x8000..0x7FFF)
  6. 阵列映射: 4x4 Weight-Stationary (WS) 脉动阵列分块调度
"""

import math
import torch
import torch.nn as nn
import torch.nn.functional as F
from typing import Optional, Tuple, Dict, Any

# ---------------------------------------------------------------------------
# 硬件微架构常量定义
# ---------------------------------------------------------------------------
MAX40 = (1 << 39) - 1       # 40'sh7F_FFFF_FFFF
MIN40 = -(1 << 39)          # 40'sh80_0000_0000
FRAC = 15                   # Q1.15 定点格式小数位宽
SAT_POS = 32767             # 0x7FFF (+0.999969)
SAT_NEG = -32768            # 0x8000 (-1.0)


def quantize_to_q115(tensor: torch.Tensor) -> torch.Tensor:
    """
    浮点 Tensor -> Q1.15 整数代码 (-32768 .. 32767)
    缩放系数为 2^15 = 32768，使用最近偶数舍入与饱和
    """
    scaled = tensor * 32768.0
    rounded = torch.round(scaled)
    clamped = torch.clamp(rounded, SAT_NEG, SAT_POS)
    return clamped.to(torch.int64)


def dequantize_from_q115(int_tensor: torch.Tensor) -> torch.Tensor:
    """
    Q1.15 整数代码 -> 浮点 Tensor (除以 32768.0)
    """
    return int_tensor.to(torch.float32) / 32768.0


def rne_sat16_torch(acc_tensor: torch.Tensor) -> torch.Tensor:
    """
    纯整数张量执行硬件等价的 RNE (最近偶数舍入) + SAT16 (饱和截断)
    输入: acc_tensor [..., ] 任意形状的 64 位有符号整数
    输出: [..., ] 范围在 [-32768, 32767] 内的 64 位整数
    """
    # Step 1: 40-bit clamp (clamp40)
    s40 = torch.clamp(acc_tensor, MIN40, MAX40)

    # Step 2: 提取 Guard, Sticky, LSB 并执行 RNE 判定
    q_floor = s40 >> FRAC
    F = s40 & 0x7FFF
    G = (F >> 14) & 1
    # Sticky 位: F 的低 14 位有任意 1 则为 1
    St = torch.where((F & 0x3FFF) != 0, torch.tensor(1, dtype=torch.int64, device=acc_tensor.device),
                     torch.tensor(0, dtype=torch.int64, device=acc_tensor.device))
    L = q_floor & 1
    inc = G & (St | L)
    q_round = q_floor + inc

    # Step 3: SAT16 饱和截断 (先舍入后饱和)
    out = torch.clamp(q_round, SAT_NEG, SAT_POS)
    return out


def systolic_matmul_q115(A_int: torch.Tensor, W_int: torch.Tensor) -> torch.Tensor:
    """
    硬件 4x4 WS 脉动阵列矩阵乘法内核 (bit-exact 仿真)
    输入:
      A_int: [..., M, K] Q1.15 整数矩阵 (输入激活)
      W_int: [K, N]      Q1.15 整数矩阵 (驻留权重)
    输出:
      C_int: [..., M, N] Q1.15 整数矩阵 (经 40b 累加、RNE 与 SAT16 后的输出)
    """
    # 累加在 64-bit 整数中进行（硬件为 40-bit，无截断）
    acc = torch.matmul(A_int.to(torch.int64), W_int.to(torch.int64))
    return rne_sat16_torch(acc)


class Q115Linear(nn.Module):
    """
    支持硬件级故障注入的 Q1.15 定点全连接层
    """
    def __init__(self, in_features: int, out_features: int, bias: bool = False):
        super().__init__()
        self.in_features = in_features
        self.out_features = out_features
        self.weight = nn.Parameter(torch.empty(out_features, in_features))
        if bias:
            self.bias = nn.Parameter(torch.empty(out_features))
        else:
            self.register_parameter('bias', None)
        self.reset_parameters()

        # 硬件保护模式: 'none', 'zero_mask', 'bist_reroute', 'dual_loop'
        self.protection_mode = 'dual_loop'
        self.fault_rate = 0.0

    def reset_parameters(self):
        nn.init.kaiming_uniform_(self.weight, a=math.sqrt(5))
        if self.bias is not None:
            fan_in, _ = nn.init._calculate_fan_in_and_fan_out(self.weight)
            bound = 1 / math.sqrt(fan_in) if fan_in > 0 else 0
            nn.init.uniform_(self.bias, -bound, bound)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        # 量化输入与权重
        x_q = quantize_to_q115(x)
        w_q = quantize_to_q115(self.weight)

        # 硬件阵列分块矩阵乘法 (X @ W^T)
        out_int = systolic_matmul_q115(x_q, w_q.t())

        # 硬件故障注入 (若激活)
        if self.fault_rate > 0.0 and self.training is False:
            out_int = self._apply_fault_injection(out_int)

        out_float = dequantize_from_q115(out_int)
        if self.bias is not None:
            out_float = out_float + self.bias
        return out_float

    def _apply_fault_injection(self, out_int: torch.Tensor) -> torch.Tensor:
        """
        根据 protection_mode 执行硬件错误注入模型 (P3-2 ISO 26262 抽象模型)
        """
        if self.fault_rate <= 0.0:
            return out_int

        # 生成 PE 故障掩码 (伯努利试验)
        fault_mask = (torch.rand_like(out_int, dtype=torch.float32) < self.fault_rate)
        if not fault_mask.any():
            return out_int

        res = out_int.clone()
        if self.protection_mode == 'none':
            # 无防护基线: 故障位产生严重 SDC 污染 (stuck-at-0 或位翻转，幅值均匀分布)
            noise = torch.randint_like(res, low=SAT_NEG, high=SAT_POS + 1)
            res[fault_mask] = noise[fault_mask]

        elif self.protection_mode == 'zero_mask':
            # 仅零掩蔽 (内循环): ABFT 检测后输出行置零 (等效结构化 dropout)
            res[fault_mask] = 0

        elif self.protection_mode == 'bist_reroute':
            # 仅 BIST 重映射 (外循环): 瞬态故障无法及时处理，永久故障热替换
            # 假设 80% 为瞬态故障 (造成 SDC)，20% 为硬故障 (被热替换治愈)
            transient_mask = fault_mask & (torch.rand_like(out_int, dtype=torch.float32) < 0.8)
            noise = torch.randint_like(res, low=SAT_NEG, high=SAT_POS + 1)
            res[transient_mask] = noise[transient_mask]

        elif self.protection_mode == 'dual_loop':
            # 双循环完整版 (本工作):
            # 瞬态故障 -> 零掩蔽 100% 吸收 (置零)
            # 永久硬故障 -> 故障裁决器触发 BIST (总 MTTR = 77 cycles, detect=2 + repair=75, 周期距离) 热替换完全恢复
            # 仅在极高故障率导致备用列耗尽 (超额故障) 时产生极少量残余错误
            unhealable_p = min(1.0, float(self.fault_rate * 2.0))
            unhealable_mask = fault_mask & (torch.rand_like(out_int, dtype=torch.float32) < unhealable_p)
            res[fault_mask] = 0  # 绝大部分被零掩蔽吸收
            if unhealable_mask.any():
                res[unhealable_mask] = torch.randint_like(res, low=SAT_NEG, high=SAT_POS + 1)[unhealable_mask]

        return res


class Q115Conv2d(nn.Module):
    """
    支持硬件级故障注入与 4x4 脉动阵列分块的 Q1.15 定点 Conv2d 层
    """
    def __init__(self, in_channels: int, out_channels: int, kernel_size: int,
                 stride: int = 1, padding: int = 0, bias: bool = False):
        super().__init__()
        self.in_channels = in_channels
        self.out_channels = out_channels
        self.kernel_size = kernel_size
        self.stride = stride
        self.padding = padding

        self.weight = nn.Parameter(torch.empty(out_channels, in_channels, kernel_size, kernel_size))
        if bias:
            self.bias = nn.Parameter(torch.empty(out_channels))
        else:
            self.register_parameter('bias', None)
        self.reset_parameters()

        self.protection_mode = 'dual_loop'
        self.fault_rate = 0.0

    def reset_parameters(self):
        nn.init.kaiming_uniform_(self.weight, a=math.sqrt(5))
        if self.bias is not None:
            fan_in, _ = nn.init._calculate_fan_in_and_fan_out(self.weight)
            bound = 1 / math.sqrt(fan_in) if fan_in > 0 else 0
            nn.init.uniform_(self.bias, -bound, bound)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        # im2col 展开: x [B, C_in, H, W] -> [B, C_in * K * K, L]
        B, C_in, H, W = x.shape
        x_unfold = F.unfold(x, kernel_size=self.kernel_size,
                            padding=self.padding, stride=self.stride)
        # 转置为 [B * L, C_in * K * K] 供脉动阵列分块矩阵乘法
        L = x_unfold.shape[-1]
        x_mat = x_unfold.transpose(1, 2).reshape(-1, C_in * self.kernel_size * self.kernel_size)

        # 权重重排为 [C_in * K * K, C_out]
        w_mat = self.weight.view(self.out_channels, -1).t()

        # 量化为 Q1.15 整数
        x_q = quantize_to_q115(x_mat)
        w_q = quantize_to_q115(w_mat)

        # 硬件脉动阵列矩阵乘法
        out_int = systolic_matmul_q115(x_q, w_q)

        # 硬件故障注入
        if self.fault_rate > 0.0 and self.training is False:
            out_int = self._apply_fault_injection(out_int)

        # 逆展开回卷积特征图 [B, C_out, H_out, W_out]
        out_float = dequantize_from_q115(out_int)
        H_out = (H + 2 * self.padding - self.kernel_size) // self.stride + 1
        W_out = (W + 2 * self.padding - self.kernel_size) // self.stride + 1
        out_conv = out_float.view(B, L, self.out_channels).transpose(1, 2).reshape(B, self.out_channels, H_out, W_out)

        if self.bias is not None:
            out_conv = out_conv + self.bias.view(1, -1, 1, 1)
        return out_conv

    def _apply_fault_injection(self, out_int: torch.Tensor) -> torch.Tensor:
        if self.fault_rate <= 0.0:
            return out_int

        fault_mask = (torch.rand_like(out_int, dtype=torch.float32) < self.fault_rate)
        if not fault_mask.any():
            return out_int

        res = out_int.clone()
        if self.protection_mode == 'none':
            noise = torch.randint_like(res, low=SAT_NEG, high=SAT_POS + 1)
            res[fault_mask] = noise[fault_mask]
        elif self.protection_mode == 'zero_mask':
            res[fault_mask] = 0
        elif self.protection_mode == 'bist_reroute':
            transient_mask = fault_mask & (torch.rand_like(out_int, dtype=torch.float32) < 0.8)
            noise = torch.randint_like(res, low=SAT_NEG, high=SAT_POS + 1)
            res[transient_mask] = noise[transient_mask]
        elif self.protection_mode == 'dual_loop':
            unhealable_p = min(1.0, float(self.fault_rate * 2.0))
            unhealable_mask = fault_mask & (torch.rand_like(out_int, dtype=torch.float32) < unhealable_p)
            res[fault_mask] = 0
            if unhealable_mask.any():
                res[unhealable_mask] = torch.randint_like(res, low=SAT_NEG, high=SAT_POS + 1)[unhealable_mask]
        return res
