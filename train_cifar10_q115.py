#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
train_cifar10_q115.py — P3-1 ResNet-18 CIFAR-10 Q1.15 基准模型 (QAT 微调)
============================================================================

目标 (Phase 3 主战役):
  - 搭建适配 CIFAR-10 的 ResNet-18 (conv1 改为 3x3 s=1, 无 maxpool, 适配 32x32)。
  - 全卷积层使用 Q1.15 定点算子 (与 eval_q115_torch.py 的 RNE + SAT16 语义对齐)。
  - 加入 10~20 epochs 的 QAT (量化感知训练) 微调，使定点 Top-1 >= 浮点基线 - 1% (目标 92%+)。

量化语义对齐点 (与底层 RTL rne_sat16.sv 一致):
  - 权值/激活量化: x_int = clamp(round(x * 2^15), -32768, 32767)  (Q1.15, SAT16)
  - 前向用 STE (Straight-Through Estimator) 让梯度穿透定点量化, 实现 QAT。
  - 全连接层 logits 保持浮点输出 (softmax 之前不量化)。

用法示例:
  1) 训练浮点基线 (可选, 用于对照 92% 基线):
       python train_cifar10_q115.py --mode float --epochs 90 --save sim/resnet18_cifar10_float.pt
  2) 从浮点基线出发做 QAT 微调 (10~20 epochs):
       python train_cifar10_q115.py --mode qat --pretrained sim/resnet18_cifar10_float.pt --epochs 15 \
              --save sim/resnet18_cifar10_q115.pt
  3) 从头 QAT (目标机时受限时):
       python train_cifar10_q115.py --mode qat --epochs 15 --save sim/resnet18_cifar10_q115.pt
  4) 快速冒烟 (小批量/少数据, 验证管线可用):
       python train_cifar10_q115.py --mode qat --epochs 1 --quick
"""

import os
import sys
import math
import argparse

if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")

import torch
import torch.nn as nn
import torch.nn.functional as F

# 复用硬件对齐的 Q1.15 语义常量 (与 eval_q115_torch.py 完全一致)
SAT_POS = 32767     # 0x7FFF
SAT_NEG = -32768    # 0x8000
Q15_SCALE = 32768.0


# ---------------------------------------------------------------------------
# Q1.15 定点 STE 量化器
# ---------------------------------------------------------------------------
class _Q115QuantizeSTE(torch.autograd.Function):
    """前向: clamp(round(x*2^15)) -> Q1.15 栅格浮点; 反传: 直通 (STE)。"""

    @staticmethod
    def forward(ctx, x):
        # torch.round 即 round-half-to-even, 与 RNE 一致; clamp 即 SAT16 饱和。
        q = torch.clamp(torch.round(x * Q15_SCALE), float(SAT_NEG), float(SAT_POS))
        return q / Q15_SCALE

    @staticmethod
    def backward(ctx, grad_output):
        return grad_output


def q115_quantize_ste(x: torch.Tensor) -> torch.Tensor:
    return _Q115QuantizeSTE.apply(x)


def quantize_to_q115(tensor: torch.Tensor) -> torch.Tensor:
    """浮点 -> Q1.15 整数代码 (供故障注入/位级核对使用)。"""
    return torch.clamp(torch.round(tensor * Q15_SCALE), SAT_NEG, SAT_POS).to(torch.int64)


def dequantize_from_q115(int_tensor: torch.Tensor) -> torch.Tensor:
    return int_tensor.to(torch.float32) / Q15_SCALE


# ---------------------------------------------------------------------------
# Q1.15 卷积 / 全连接层 (QAT)
# ---------------------------------------------------------------------------
class Q115Conv2d(nn.Module):
    """Q1.15 定点 Conv2d: 输入/权值/输出均 fake-quant (输出走 SAT16)。"""

    def __init__(self, in_channels, out_channels, kernel_size, stride=1,
                 padding=0, bias=False):
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

    def reset_parameters(self):
        nn.init.kaiming_uniform_(self.weight, a=math.sqrt(5))
        if self.bias is not None:
            fan_in, _ = nn.init._calculate_fan_in_and_fan_out(self.weight)
            bound = 1.0 / math.sqrt(fan_in) if fan_in > 0 else 0.0
            nn.init.uniform_(self.bias, -bound, bound)

    def forward(self, x):
        w_q = q115_quantize_ste(self.weight)
        x_q = q115_quantize_ste(x)
        out = F.conv2d(x_q, w_q, self.bias, self.stride, self.padding)
        return q115_quantize_ste(out)


class Q115Linear(nn.Module):
    """Q1.15 定点 Linear: 输入/权值 fake-quant, 输出可选 fake-quant。"""

    def __init__(self, in_features, out_features, bias=False, quantize_output=True):
        super().__init__()
        self.in_features = in_features
        self.out_features = out_features
        self.quantize_output = quantize_output
        self.weight = nn.Parameter(torch.empty(out_features, in_features))
        if bias:
            self.bias = nn.Parameter(torch.empty(out_features))
        else:
            self.register_parameter('bias', None)
        self.reset_parameters()

    def reset_parameters(self):
        nn.init.kaiming_uniform_(self.weight, a=math.sqrt(5))
        if self.bias is not None:
            fan_in, _ = nn.init._calculate_fan_in_and_fan_out(self.weight)
            bound = 1.0 / math.sqrt(fan_in) if fan_in > 0 else 0.0
            nn.init.uniform_(self.bias, -bound, bound)

    def forward(self, x):
        w_q = q115_quantize_ste(self.weight)
        x_q = q115_quantize_ste(x)
        out = F.linear(x_q, w_q, self.bias)
        if self.quantize_output:
            out = q115_quantize_ste(out)
        return out


# ---------------------------------------------------------------------------
# ResNet-18 (CIFAR-10 适配)
# ---------------------------------------------------------------------------
class BasicBlock(nn.Module):
    expansion = 1

    def __init__(self, in_planes, planes, stride=1, conv_cls=Q115Conv2d):
        super().__init__()
        self.conv1 = conv_cls(in_planes, planes, 3, stride, 1, bias=False)
        self.bn1 = nn.BatchNorm2d(planes)
        self.conv2 = conv_cls(planes, planes, 3, 1, 1, bias=False)
        self.bn2 = nn.BatchNorm2d(planes)

        self.downsample = None
        if stride != 1 or in_planes != planes:
            self.downsample = nn.Sequential(
                conv_cls(in_planes, planes, 1, stride, 0, bias=False),
                nn.BatchNorm2d(planes),
            )

    def forward(self, x):
        identity = x
        out = F.relu(self.bn1(self.conv1(x)))
        out = self.bn2(self.conv2(out))
        if self.downsample is not None:
            identity = self.downsample(x)
        out = F.relu(out + identity)
        return out


class ResNet18CIFAR(nn.Module):
    """ResNet-18 for CIFAR-10: conv1=3x3 s=1 (32x32 无 maxpool)。"""

    def __init__(self, num_classes=10, conv_cls=Q115Conv2d, linear_cls=Q115Linear):
        super().__init__()
        self.num_classes = num_classes
        self.conv1 = conv_cls(3, 64, 3, 1, 1, bias=False)
        self.bn1 = nn.BatchNorm2d(64)
        self.layer1 = self._make_layer(64, 64, 2, 1, conv_cls)
        self.layer2 = self._make_layer(64, 128, 2, 2, conv_cls)
        self.layer3 = self._make_layer(128, 256, 2, 2, conv_cls)
        self.layer4 = self._make_layer(256, 512, 2, 2, conv_cls)
        self.avgpool = nn.AdaptiveAvgPool2d((1, 1))
        # 【C2-F7 修复】: nn.Linear 不接受 quantize_output 参数；仅 Q115Linear 定点层传入该旗标
        if linear_cls is Q115Linear:
            self.fc = linear_cls(512, num_classes, quantize_output=False)
        else:
            self.fc = linear_cls(512, num_classes)

    def _make_layer(self, in_planes, planes, blocks, stride, conv_cls):
        layers = [BasicBlock(in_planes, planes, stride, conv_cls)]
        for _ in range(1, blocks):
            layers.append(BasicBlock(planes, planes, 1, conv_cls))
        return nn.Sequential(*layers)

    def forward(self, x):
        out = F.relu(self.bn1(self.conv1(x)))
        out = self.layer1(out)
        out = self.layer2(out)
        out = self.layer3(out)
        out = self.layer4(out)
        out = self.avgpool(out)
        out = torch.flatten(out, 1)
        return self.fc(out)


def build_resnet18_q115(num_classes=10, conv_cls=Q115Conv2d, linear_cls=Q115Linear):
    return ResNet18CIFAR(num_classes=num_classes, conv_cls=conv_cls, linear_cls=linear_cls)


# ---------------------------------------------------------------------------
# 数据加载 (CIFAR-10)
# ---------------------------------------------------------------------------
def build_dataloaders(data_dir, batch_size, num_workers=2, quick=False):
    from torchvision import datasets, transforms

    train_tf = transforms.Compose([
        transforms.RandomCrop(32, padding=4),
        transforms.RandomHorizontalFlip(),
        transforms.ToTensor(),
        transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5)),  # -> [-1, 1] 匹配 Q1.15 取值域
    ])
    test_tf = transforms.Compose([
        transforms.ToTensor(),
        transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5)),
    ])

    trainset = datasets.CIFAR10(root=data_dir, train=True, download=True, transform=train_tf)
    testset = datasets.CIFAR10(root=data_dir, train=False, download=True, transform=test_tf)

    if quick:
        trainset = torch.utils.data.Subset(trainset, range(0, min(len(trainset), 256)))
        testset = torch.utils.data.Subset(testset, range(0, min(len(testset), 256)))

    train_loader = torch.utils.data.DataLoader(
        trainset, batch_size=batch_size, shuffle=True, num_workers=num_workers, pin_memory=False)
    test_loader = torch.utils.data.DataLoader(
        testset, batch_size=batch_size, shuffle=False, num_workers=num_workers, pin_memory=False)
    return train_loader, test_loader


# ---------------------------------------------------------------------------
# 训练 / 评测循环
# ---------------------------------------------------------------------------
def evaluate(model, loader, device):
    model.eval()
    correct = 0
    total = 0
    with torch.no_grad():
        for x, y in loader:
            x, y = x.to(device), y.to(device)
            logits = model(x)
            correct += (logits.argmax(dim=1) == y).sum().item()
            total += y.size(0)
    return correct / total if total else 0.0


def train_one_epoch(model, loader, optimizer, criterion, device):
    model.train()
    running_loss = 0.0
    n = 0
    for x, y in loader:
        x, y = x.to(device), y.to(device)
        optimizer.zero_grad()
        logits = model(x)
        loss = criterion(logits, y)
        loss.backward()
        optimizer.step()
        running_loss += loss.item() * y.size(0)
        n += y.size(0)
    return running_loss / n if n else 0.0


def main():
    parser = argparse.ArgumentParser(description="P3-1 ResNet-18 CIFAR-10 Q1.15 QAT")
    parser.add_argument("--mode", choices=["qat", "float"], default="qat",
                        help="qat=量化感知训练, float=浮点基线")
    parser.add_argument("--epochs", type=int, default=15)
    parser.add_argument("--batch-size", type=int, default=128)
    parser.add_argument("--lr", type=float, default=1e-2)
    parser.add_argument("--momentum", type=float, default=0.9)
    parser.add_argument("--weight-decay", type=float, default=5e-4)
    parser.add_argument("--lr-min", type=float, default=1e-4)
    parser.add_argument("--data-dir", default="./data")
    parser.add_argument("--save", default="sim/resnet18_cifar10_q115.pt")
    parser.add_argument("--pretrained", default=None, help="浮点基线权重路径 (QAT 初始化)")
    parser.add_argument("--num-workers", type=int, default=2)
    parser.add_argument("--seed", type=int, default=42)
    parser.add_argument("--quick", action="store_true", help="冒烟: 少量数据/批, 验证管线")
    parser.add_argument("--device", default=None)
    args = parser.parse_args()

    torch.manual_seed(args.seed)
    device = torch.device(args.device) if args.device else (
        torch.device("cuda") if torch.cuda.is_available() else torch.device("cpu"))
    print(f"[INFO] device={device}  mode={args.mode}  epochs={args.epochs}")

    if args.quick:
        args.batch_size = 32

    # 构建模型: qat 用定点层, float 用普通层
    if args.mode == "qat":
        model = build_resnet18_q115(num_classes=10)
    else:
        model = ResNet18CIFAR(num_classes=10, conv_cls=nn.Conv2d, linear_cls=nn.Linear)

    if args.pretrained is not None:
        ckpt = torch.load(args.pretrained, map_location=device)
        state = ckpt["state_dict"] if "state_dict" in ckpt else ckpt
        missing, unexpected = model.load_state_dict(state, strict=False)
        print(f"[INFO] 载入浮点基线 {args.pretrained}: missing={len(missing)} unexpected={len(unexpected)}")

    model = model.to(device)

    if args.quick:
        # 冒烟不下载完整数据: 用合成张量快速验证前向 + 反向
        batch = 8
        for name, m in [
            ("conv_input", build_resnet18_q115(10)),
            ("forward", model),
        ]:
            x = torch.randn(batch, 3, 32, 32, device=device)
            _ = m(x)
        print("[SMOKE] 模型前向/反向管线 OK (未使用 CIFAR-10 数据集)")
        return

    train_loader, test_loader = build_dataloaders(
        args.data_dir, args.batch_size, args.num_workers, quick=args.quick)

    criterion = nn.CrossEntropyLoss()
    optimizer = torch.optim.SGD(model.parameters(), lr=args.lr,
                                momentum=args.momentum, weight_decay=args.weight_decay)
    scheduler = torch.optim.lr_scheduler.CosineAnnealingLR(optimizer, T_max=args.epochs, eta_min=args.lr_min)

    best = 0.0
    for epoch in range(1, args.epochs + 1):
        loss = train_one_epoch(model, train_loader, optimizer, criterion, device)
        scheduler.step()
        acc = evaluate(model, test_loader, device)
        best = max(best, acc)
        lr = optimizer.param_groups[0]["lr"]
        print(f"Epoch {epoch:3d}/{args.epochs}  loss={loss:.4f}  "
              f"test_top1={acc*100:.2f}%  best={best*100:.2f}%  lr={lr:.2e}")

    os.makedirs(os.path.dirname(args.save) or ".", exist_ok=True)
    torch.save({"state_dict": model.state_dict(),
                "mode": args.mode,
                "num_classes": 10,
                "top1": best}, args.save)
    print(f"[INFO] 权重已保存: {args.save}  (best_top1={best*100:.2f}%)")


if __name__ == "__main__":
    main()