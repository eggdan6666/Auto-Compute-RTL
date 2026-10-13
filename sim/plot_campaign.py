#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
plot_campaign.py — E1 故障注入回归结果图 (RESULT 类图件)
========================================================
真实输入映射: campaign_results.csv (tb_fault_campaign.sv 仿真真实输出,
              Icarus 与 Verilator 双仿真器逐位一致, 见 sim/campaign_results_verilator.csv)
执行证据    : 本脚本实际运行生成 campaign_fig.png
审核状态    : 待用户确认图位后方可进入论文稿
"""
import csv
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

rows = list(csv.DictReader(open("campaign_results.csv", encoding="utf-8")))
faults = [r for r in rows if int(r["model"]) != 0]
ctrls  = [r for r in rows if int(r["model"]) == 0]
n, m = len(faults), len(ctrls)

det      = sum(int(r["detected"]) for r in faults)
loc      = sum(int(r["loc_ok"]) for r in faults)
heal     = sum(1 for r in faults if int(r["healed"]) == 1 and int(r["post_err"]) == 0)
pre_bad  = sum(1 for r in faults if int(r["pre_err"]) != 0)
post_bad = sum(1 for r in faults if int(r["post_err"]) != 0)
false_al = sum(int(r["detected"]) for r in ctrls)

metrics = [
    ("Baseline error rate\n(no self-healing)", 100.0 * pre_bad / n),
    ("Detection rate",                         100.0 * det / n),
    ("Localization accuracy",                  100.0 * loc / max(det, 1)),
    ("Heal success rate",                      100.0 * heal / n),
    ("Post-heal error rate",                   100.0 * post_bad / n),
    ("False alarm rate\n(control)",            100.0 * false_al / m),
]

fig, ax = plt.subplots(figsize=(8, 4.5), dpi=150)
colors = ["#d62728", "#1f77b4", "#1f77b4", "#2ca02c", "#2ca02c", "#9467bd"]
bars = ax.bar([x[0] for x in metrics], [x[1] for x in metrics], color=colors)
for b, (_, v) in zip(bars, metrics):
    ax.text(b.get_x() + b.get_width() / 2, v + 1.5, f"{v:.1f}%",
            ha="center", fontsize=9)
ax.set_ylabel("Rate (%)")
ax.set_ylim(0, 115)
ax.set_title(f"E1 Fault-Injection Campaign: {n} fault + {m} control scenarios\n"
             f"(self-healing systolic array, N=4, stuck-at on PE psum/weight)")
ax.tick_params(axis="x", labelsize=8)
plt.xticks(rotation=12, ha="right")
plt.tight_layout()
plt.savefig("campaign_fig.png")
print("campaign_fig.png generated from campaign_results.csv (real simulation log)")
print("raw metrics:", {k: round(v, 2) for k, v in metrics})
