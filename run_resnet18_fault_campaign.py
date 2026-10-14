#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
run_resnet18_fault_campaign.py — P3-3 ResNet-18 故障注入战役脚本
=================================================================

目标 (Phase 3 主战役):
  扫描 10^-6 ~ 10^-2 故障率, 输出四条消融曲线 (Baseline / 仅零掩蔽 / 仅 BIST / 双循环)
  的 Top-1 与 SDC / DUE 分布。

错误模型参数 —— 严格接入本地实测台账数据 (非合成均匀噪声):
  1) 785 条 RTL 实测误差分布 (campaign_results.csv 中 pre_err>0 的故障场景):
     由真实 RTL 故障注入回归的卡死值 (psum 40-bit / weight 16-bit) 经 RNE+SAT16
     映射为 Q1.15 污染码, 构成真实"误差幅值分布"。
  2) 100 条瞬态脉冲分布 (sim/transient_distribution.csv):
     瞬态软错误产生的 ABFT 误差脉冲计数实测 (0/1/2/ge3 = 87/13/0/0),
     用于刻画"可被零掩蔽吸收"的瞬态特征 (ge3=0 => 瞬态永不触发 BIST)。

四条消融曲线语义:
  - baseline     : 无防护基线, 所有故障静默污染输出 -> SDC。
  - zero_mask    : 仅内循环零掩蔽, ABFT 检出并置零 -> 检出但降级 (DUE)。
  - bist_reroute : 仅外循环 BIST, 永久硬故障热替换治愈, 瞬态无内循环 -> SDC。
  - dual_loop    : 双循环完整版, 瞬态掩蔽 + 永久热替换, 仅备用列耗尽残余 SDC。

示例:
  python run_resnet18_fault_campaign.py \
      --checkpoint sim/resnet18_cifar10_q115.pt \
      --campaign-csv campaign_results.csv \
      --transient-csv sim/transient_distribution.csv \
      --max-samples 256 --out sim/resnet18_fault_campaign.csv

  无 CIFAR/无权重时的管线冒烟:
  python run_resnet18_fault_campaign.py --synthetic --max-samples 32 --rates 1e-4,1e-2
"""

import os
import sys
import csv
import math
import argparse

if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")

import numpy as np
import torch
import torch.nn.functional as F

# 复用 P3-1 的 Q1.15 模型结构与 P3-0/eval 的定点算子
from train_cifar10_q115 import (
    Q115Conv2d, Q115Linear, build_resnet18_q115,
    quantize_to_q115, dequantize_from_q115,
)
from eval_q115_torch import rne_sat16_torch, SAT_NEG, SAT_POS

MODES = ["baseline", "zero_mask", "bist_reroute", "dual_loop"]


# ---------------------------------------------------------------------------
# 本地实测误差模型
# ---------------------------------------------------------------------------
class RTLMeasuredErrorModel:
    """加载 785 条 RTL 误差分布 + 100 条瞬态脉冲分布, 作为错误模型参数。"""

    def __init__(self, campaign_csv, transient_csv):
        self.corrupt_values = self._load_error_distribution(campaign_csv)
        self.pulse_values, self.pulse_probs = self._load_transient_distribution(transient_csv)

    def _load_error_distribution(self, path):
        values = []
        n_fault = 0
        n_err = 0
        with open(path, "r", encoding="utf-8") as f:
            for r in csv.DictReader(f):
                if int(r["model"]) == 0:
                    continue  # 对照组无故障
                n_fault += 1
                if int(r["pre_err"]) == 0:
                    continue  # 备用列故障不影响任务输出
                model = int(r["model"])
                raw = int(r["val"], 16) & 0xFFFFFFFFFF  # 40-bit
                if model == 1:  # psum 卡死 (40-bit) -> 硬件 RNE+SAT16
                    v40 = raw
                    if v40 >= (1 << 39):
                        v40 -= (1 << 40)
                    code = int(rne_sat16_torch(torch.tensor([v40], dtype=torch.int64)).item())
                else:           # weight 卡死 (16-bit, 已为 Q1.15 码)
                    v16 = raw & 0xFFFF
                    if v16 >= (1 << 15):
                        v16 -= (1 << 16)
                    code = v16
                values.append(code)
                n_err += 1

        arr = torch.tensor(values, dtype=torch.int64)
        print(f"[ERROR-MODEL] 误差分布: {n_fault} 故障场景 -> {n_err} 条 RTL 实测污染码 "
              f"(min={arr.min().item()}, max={arr.max().item()}, 均值幅值={arr.abs().float().mean().item()/32768.0:.4f})")
        assert arr.numel() > 0, "误差分布为空, 请检查 campaign_results.csv"
        return arr

    def _load_transient_distribution(self, path):
        values, probs = [], []
        with open(path, "r", encoding="utf-8") as f:
            for r in csv.DictReader(f):
                p_str = r["pulses"].strip()
                pct = float(r["percentage"])
                # 【C1-F6 修复】: 瞬态脉冲 >=3 会触发外循环 BIST，不属于可掩蔽瞬态，若存在必须显式拦截
                if (p_str.lower().startswith("ge") or (p_str.isdigit() and int(p_str) >= 3)) and pct > 0:
                    raise ValueError(f"[ERROR] 瞬态分布包含 ge3={pct*100:.1f}% > 0：>=3 脉冲将触发 BIST，不属于可掩蔽瞬态，禁止按 pulses=3 处理！")
                values.append(3 if p_str.lower().startswith("ge") else int(p_str))
                probs.append(pct)
        probs = torch.tensor(probs, dtype=torch.float32)
        probs = probs / probs.sum()
        print(f"[ERROR-MODEL] 瞬态脉冲分布: pulses={values} probs={[round(x,4) for x in probs.tolist()]} (100 条实测)")
        return values, probs

    def sample_corrupt(self, n, device):
        idx = torch.randint(0, self.corrupt_values.numel(), (n,), device="cpu")
        return self.corrupt_values[idx].to(device)

    def sample_pulses(self, shape, device):
        probs = self.pulse_probs.to(device)
        n = int(np.prod(shape))
        idx = torch.multinomial(probs, n, replacement=True).to(device)
        vals = torch.tensor(self.pulse_values, dtype=torch.int64, device=device)[idx]
        return vals.reshape(shape)


def compute_wilson_ci(k, n, z=1.96):
    """计算 Wilson Score 95% 双侧置信区间 [low, high] (满足 R4 与论文 95% CI 规范)"""
    if n == 0:
        return 0.0, 0.0
    p = float(k) / float(n)
    denom = 1.0 + (z * z) / float(n)
    center = (p + (z * z) / (2.0 * float(n))) / denom
    margin = (z / denom) * math.sqrt(p * (1.0 - p) / float(n) + (z * z) / (4.0 * float(n) * float(n)))
    return max(0.0, center - margin), min(1.0, center + margin)


# ---------------------------------------------------------------------------
# 故障注入会话 (聚合逐样本 SDC / DUE 标志)
# ---------------------------------------------------------------------------
class FaultSession:
    def __init__(self, error_model, transient_ratio=0.8, overcapacity_factor=2.0):
        self.em = error_model
        self.transient_ratio = transient_ratio
        self.overcapacity_factor = overcapacity_factor
        self.silent = None
        self.detected = None
        self.total_fault_events = 0

    def reset(self, B, device):
        self.silent = torch.zeros(B, dtype=torch.bool, device=device)
        self.detected = torch.zeros(B, dtype=torch.bool, device=device)

    def _apply(self, res, mask):
        """在 mask=True 处写入实测污染码 (静态分配由调用方保证形状一致)。"""
        k = int(mask.sum().item())
        if k == 0:
            return
        vals = self.em.sample_corrupt(k, res.device)
        res[mask] = vals

    def inject(self, out_int, fault_rate, mode):
        if fault_rate <= 0.0 or out_int.numel() == 0:
            return out_int

        B = out_int.shape[0]
        dev = out_int.device
        flat = out_int.reshape(B, -1).clone()

        fault_mask = torch.rand(flat.shape, device=dev) < fault_rate
        if not fault_mask.any():
            return out_int
        self.total_fault_events += int(fault_mask.sum().item())

        # 故障类型划分: 瞬态软错误 (占比 transient_ratio) vs 永久硬故障
        is_trans = torch.rand(flat.shape, device=dev) < self.transient_ratio
        trans_hits = fault_mask & is_trans
        perm_hits = fault_mask & ~is_trans

        # 瞬态脉冲实测: ge3=0 => 全部可被零掩蔽吸收; 87% 为 0 脉冲(不产生污染)
        pulses = self.em.sample_pulses(flat.shape, dev)
        eff_trans = trans_hits & (pulses > 0)

        silent = torch.zeros(B, dtype=torch.bool, device=dev)
        detected = torch.zeros(B, dtype=torch.bool, device=dev)

        if mode == "baseline":
            corrupt = eff_trans | perm_hits
            self._apply(flat, corrupt)
            silent = corrupt.any(dim=1)

        elif mode == "zero_mask":
            masked = eff_trans | perm_hits  # 内循环 ABFT 检出 -> 置零 (优雅降级)
            flat[masked] = 0
            detected = masked.any(dim=1)

        elif mode == "bist_reroute":
            self._apply(flat, eff_trans)   # 瞬态无内循环 -> 静默污染
            healed = perm_hits              # 永久硬故障 -> 外循环热替换治愈
            silent = eff_trans.any(dim=1)
            detected = healed.any(dim=1)

        elif mode == "dual_loop":
            flat[eff_trans] = 0             # 瞬态 -> 零掩蔽
            p_over = min(1.0, fault_rate * self.overcapacity_factor)  # 备用列耗尽概率
            unhealable = perm_hits & (torch.rand(flat.shape, device=dev) < p_over)
            self._apply(flat, unhealable)   # 超出备用列容量 -> 残余静默污染
            healed = perm_hits & ~unhealable
            silent = unhealable.any(dim=1)
            detected = (eff_trans | healed).any(dim=1)

        else:
            raise ValueError(f"未知保护模式: {mode}")

        # 静默污染优先于检出 (若有静默逃逸, 归类 SDC)
        self.silent |= silent
        self.detected |= detected
        return flat.reshape_as(out_int)


# ---------------------------------------------------------------------------
# 故障注入层 (仅在推理时对卷积层注入; 分类 logits 保持浮点)
# ---------------------------------------------------------------------------
class FaultQ115Conv2d(Q115Conv2d):
    def __init__(self, *args, session=None, **kwargs):
        super().__init__(*args, **kwargs)
        self.session = session
        self.protection_mode = "dual_loop"
        self.fault_rate = 0.0

    def forward(self, x):
        out = super().forward(x)  # Q1.15 栅格浮点
        if self.fault_rate > 0.0 and not self.training and self.session is not None:
            out_int = quantize_to_q115(out)
            out_int = self.session.inject(out_int, self.fault_rate, self.protection_mode)
            out = dequantize_from_q115(out_int)
        return out


def configure_faults(model, mode, rate):
    """为模型中所有故障卷积层设置保护模式与故障率。"""
    for m in model.modules():
        if isinstance(m, FaultQ115Conv2d):
            m.protection_mode = mode
            m.fault_rate = rate


# ---------------------------------------------------------------------------
# SDK / DUE 评测
# ---------------------------------------------------------------------------
# SDK / DUE 评测
# ---------------------------------------------------------------------------
@torch.no_grad()
def evaluate_faults(model, loader, session, device):
    model.eval()
    session.total_fault_events = 0
    total = correct = sdc = due = 0
    for x, y in loader:
        x, y = x.to(device), y.to(device)
        B = x.size(0)
        session.reset(B, device)
        logits = model(x)
        pred = logits.argmax(dim=1)
        wrong = pred != y
        s_dc = wrong & session.silent
        d_due = wrong & (~session.silent) & session.detected
        correct += int((~wrong).sum().item())
        sdc += int(s_dc.sum().item())
        due += int(d_due.sum().item())
        total += B
    top1 = (correct / total) if total else 0.0
    ci_low, ci_high = compute_wilson_ci(correct, total)
    sdc_rate = (sdc / total) if total else 0.0
    due_rate = (due / total) if total else 0.0
    n_events = session.total_fault_events
    return (top1, ci_low, ci_high, sdc_rate, due_rate, total, n_events)


class _SyntheticSet(torch.utils.data.Dataset):
    """无 CIFAR 依赖的随机数据集, 供管线冒烟/离线验证。"""

    def __init__(self, n=256, seed=0):
        g = torch.Generator().manual_seed(seed)
        self.x = torch.randn(n, 3, 32, 32, generator=g)
        self.y = torch.randint(0, 10, (n,), generator=g)

    def __len__(self):
        return len(self.y)

    def __getitem__(self, i):
        return self.x[i], self.y[i]


def build_test_loader(args):
    if args.synthetic:
        ds = _SyntheticSet(args.max_samples, seed=args.seed)
        return torch.utils.data.DataLoader(ds, batch_size=args.batch_size, shuffle=False)
    from torchvision import datasets, transforms
    tf = transforms.Compose([
        transforms.ToTensor(),
        transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5)),
    ])
    ds = datasets.CIFAR10(root=args.data_dir, train=False, download=True, transform=tf)
    if args.max_samples and args.max_samples < len(ds):
        ds = torch.utils.data.Subset(ds, range(args.max_samples))
    return torch.utils.data.DataLoader(ds, batch_size=args.batch_size, shuffle=False, num_workers=args.num_workers)


def main():
    parser = argparse.ArgumentParser(description="P3-3 ResNet-18 故障注入战役 (四路消融)")
    parser.add_argument("--checkpoint", default="sim/resnet18_cifar10_q115.pt")
    parser.add_argument("--campaign-csv", default="campaign_results.csv")
    parser.add_argument("--transient-csv", default="sim/transient_distribution.csv")
    parser.add_argument("--out", default="sim/resnet18_fault_campaign.csv")
    parser.add_argument("--rates", default="1e-6,3e-6,1e-5,3e-5,1e-4,3e-4,1e-3,3e-3,1e-2")
    parser.add_argument("--transient-ratio", type=float, default=0.8)
    parser.add_argument("--overcapacity-factor", type=float, default=2.0)
    parser.add_argument("--batch-size", type=int, default=64)
    parser.add_argument("--max-samples", type=int, default=1000)
    parser.add_argument("--num-workers", type=int, default=2)
    parser.add_argument("--data-dir", default="./data")
    parser.add_argument("--synthetic", action="store_true", help="随机数据冒烟 (不下载 CIFAR)")
    parser.add_argument("--seed", type=int, default=42)
    parser.add_argument("--device", default=None)
    args = parser.parse_args()

    torch.manual_seed(args.seed)
    np.random.seed(args.seed)
    device = torch.device(args.device) if args.device else (
        torch.device("cuda") if torch.cuda.is_available() else torch.device("cpu"))
    print(f"[INFO] device={device}")

    # 1) 实测误差模型
    em = RTLMeasuredErrorModel(args.campaign_csv, args.transient_csv)
    session = FaultSession(em, args.transient_ratio, args.overcapacity_factor)

    # 2) 故障注入模型 + 权重
    model = build_resnet18_q115(
        num_classes=10,
        conv_cls=lambda *a, **k: FaultQ115Conv2d(*a, session=session, **k),
        linear_cls=Q115Linear,
    )
    if os.path.exists(args.checkpoint):
        ckpt = torch.load(args.checkpoint, map_location=device)
        state = ckpt["state_dict"] if "state_dict" in ckpt else ckpt
        missing, unexpected = model.load_state_dict(state, strict=False)
        print(f"[INFO] 载入权重 {args.checkpoint}: missing={len(missing)} unexpected={len(unexpected)}")
    else:
        print(f"[WARN] 权重不存在 {args.checkpoint}, 使用随机初始化 (仅验证战役管线, Top-1 无意义)")
    model = model.to(device)
    model.eval()

    # 3) 测试数据
    loader = build_test_loader(args)

    rates = [float(x) for x in args.rates.split(",") if x.strip()]
    print("\n" + "=" * 105)
    print(f"{'mode':<14} {'fault_rate':>10} {'top1':>8} {'95% CI':>17} {'sdc':>8} {'due':>8} {'events':>8}")
    print("-" * 105)

    rows = []
    n_samples = 0
    for mode in MODES:
        for rate in rates:
            configure_faults(model, mode, rate)
            top1, ci_low, ci_high, sdc, due, total_samp, n_events = evaluate_faults(model, loader, session, device)
            rows.append({
                "mode": mode, "fault_rate": rate,
                "top1": top1, "ci95_low": ci_low, "ci95_high": ci_high,
                "sdc": sdc, "due": due,
                "n_samples": total_samp, "n_events": n_events
            })
            ci_str = f"[{ci_low*100:.1f}%,{ci_high*100:.1f}%]"
            print(f"{mode:<14} {rate:>10.2e} {top1*100:>7.2f}% {ci_str:>17} {sdc*100:>7.2f}% {due*100:>7.2f}% {n_events:>8d}")
            n_samples = max(n_samples, total_samp)
    print("=" * 105)

    # 4) 写 CSV (满足 R4 与论文四曲线落账要求)
    os.makedirs(os.path.dirname(args.out) or ".", exist_ok=True)
    with open(args.out, "w", newline="", encoding="utf-8") as f:
        fieldnames = ["mode", "fault_rate", "top1", "ci95_low", "ci95_high", "sdc", "due", "n_samples", "n_events"]
        w = csv.DictWriter(f, fieldnames=fieldnames)
        w.writeheader()
        for r in rows:
            w.writerow({
                "mode": r["mode"],
                "fault_rate": f"{r['fault_rate']:.6e}",
                "top1": f"{r['top1']:.6f}",
                "ci95_low": f"{r['ci95_low']:.6f}",
                "ci95_high": f"{r['ci95_high']:.6f}",
                "sdc": f"{r['sdc']:.6f}",
                "due": f"{r['due']:.6f}",
                "n_samples": r["n_samples"],
                "n_events": r["n_events"],
            })
    print(f"[INFO] 战役结果已写至: {args.out}")
    print(f"[INFO] 4 条消融曲线 (top1/ci95/sdc/due/events) 已生成, 可用 matplotlib 绘制。")


if __name__ == "__main__":
    main()