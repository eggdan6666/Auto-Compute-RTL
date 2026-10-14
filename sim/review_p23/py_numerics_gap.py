#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""复审实证: QAT 浮点卷积路径 vs eval 精确整数路径的码值差异 (A1)"""
import os
import sys
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))  # 仓库根

import torch
from train_cifar10_q115 import Q115Conv2d, quantize_to_q115
from eval_q115_torch import Q115Conv2d as EvalConv

torch.manual_seed(7)
conv = Q115Conv2d(3, 16, 3, 1, 1, bias=False)
x = torch.randn(8, 3, 32, 32)
out_train = conv(x)                      # float conv + fake-quant (QAT 路径)
econv = EvalConv(3, 16, 3, 1, 1, bias=False)
econv.weight.data = conv.weight.data.clone()
out_eval = econv(x)                      # 精确 int64 matmul + RNE+SAT16 (硬件路径)

c_train = quantize_to_q115(out_train)
c_eval = quantize_to_q115(out_eval)
diff = (c_train != c_eval)
n = diff.numel()
k = int(diff.sum())
print(f"[3->16 3x3] 元素总数={n}  码值不一致={k} ({100.0*k/n:.3f}%)")
if k:
    d = (c_train - c_eval)[diff]
    print(f"  差异分布: +1:{int((d==1).sum())}  -1:{int((d==-1).sum())}  其他:{int((d.abs()>1).sum())}  最大|差|={int(d.abs().max())}")

conv2 = Q115Conv2d(64, 64, 3, 1, 1, bias=False)
x2 = torch.randn(4, 64, 16, 16)
o1 = conv2(x2)
e2 = EvalConv(64, 64, 3, 1, 1, bias=False)
e2.weight.data = conv2.weight.data.clone()
o2 = e2(x2)
d2 = (quantize_to_q115(o1) != quantize_to_q115(o2))
k2 = int(d2.sum())
print(f"[64->64 3x3 累加深576] 不一致={k2}/{d2.numel()} ({100.0*k2/d2.numel():.3f}%)")

# argmax 影响: 用两层小网络看 Top-1 是否可能翻转 (仅说明量级)
lin1 = torch.nn.Linear(64, 10)
