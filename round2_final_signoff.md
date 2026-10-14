# Round 2 终审签核记录 — Phase 2/3 闭环自愈与 Phase 3 故障注入体系

| 项 | 内容 |
|---|---|
| 签核对象 | Trae 对 F0~F3 / F6 / F7 的全部整改 + `round2_acceptance_checklist.md` 之 C1~C12 条件关闭申请 |
| 签核人 | 齐流晶（FPGA 交付总监）；技术细节按 rtl-designer / sim-verifier 视角执行 |
| 签核日期 | 2026-10-14 |
| 签核方法 | **不接受转述结论**：16 项门控由签核人**独立重跑**（2026-10-14 07:43，Icarus 14.0），全部静态证据（代码/论文/脚本/日志）逐项亲自核查 |
| 关联文档 | `round2_acceptance_checklist.md`、`peer_review_phase2_phase3.md`、`peer_review_phase3_python_r1.md`、`sim/review_p23/README.md` |

---

## 0. 终审结论

# ✅ Round 2 代码门控终审：通过（16/16，独立复跑退出码 0）
# ⚠️ 硬条件解除：Phase 3 神经网络战役 **正式解除**；论文数据录入 **部分解除**（RTL 侧全放行，DNN 四曲线章节附 4 项修订条件 R1~R4）

**独立复跑证据**（签核人亲自执行，非转述）：

```
$ bash sim/review_p23/run_round2_gates.sh
日期: 2026-10-14 07:43:21   工具: Icarus Verilog version 14.0 (devel)
[GATE] G1a: PASS: hist=[87,13,0,0] busy_viol=0        [GATE] G1b: PASS: 参照分布=[87,13,0,0]
[GATE] G1c: PASS: 已知 2 拍注入测得 >=2 脉冲           [GATE] G2 : PASS: 12/12; MTTR detect=2 repair=75 total=77
[GATE] G3a: PASS: detect_latency=2 (<=10)             [GATE] G3b: PASS: repair_latency=75 (<=80)
[GATE] G4a: PASS: 无伪 unhealable/伪重映射             [GATE] G4b: PASS: 持续灌流下正确自愈 (repair=76)
[GATE] G5 : PASS: 2602 向量 0 mismatch                 [GATE] G6 : PASS: 0 锁存器; No-latch 269 处
[GATE] G7a: PASS: 不可愈比例=0.1999≈2×rate; 钳位在     [GATE] G7b: PASS: 噪声域 [-32768, +32767]
[GATE] G7c: PASS: ge3>0 → exit=1 显式拦截              [GATE] G7d: PASS: --mode float --quick exit=0
[GATE] G8 : PASS: 战役冒烟 CSV 8 行                     [GATE] G9 : PASS: 码值差 0.072%/0.345% ≤ 0.5%
================ 汇总 ================
PASS 门数: 16      FAIL 门数: 0
```

与申请人提交的 `sim/review_p23/round2_gates_final.log`（16/0）**结论一致**；两份日志 + 本次独立复跑（`round2_gates.log`）共同构成签核证据链。

---

## 1. C1~C12 逐项核验结果

| 条件 | 申请人声明 | 签核人独立核验 | 结论 |
|---|---|---|---|
| C1（F6 ge3 拦截） | 卫语句已加 | `run_resnet18_fault_campaign.py:107-109` 卫语句在（ge3>0 时 `raise ValueError` 并明示"≥3 脉冲将触发 BIST，禁止按 pulses=3 处理"）；G7c 实测 exit=1 + 显式申示 | ✅ 关闭 |
| C2（F7 float 模式） | 已兼容 nn.Linear | `train_cifar10_q115.py:186-190` 按 `linear_cls is Q115Linear` 条件传参；G7d 实测 exit=0 | ✅ 关闭 |
| C3（S_DRAIN 代价） | 确认为必要物理代价 | `bist_controller.sv` S_DRAIN + `drain_wait_cnt` 在；`self_healing_array.sv:134` `valid_in && !busy` 门控在；G2/G4b 实测 repair=75/76、持续灌流 0 误判 | ✅ 关闭 |
| C4（论文 MTTR=77） | 已统一 | 论文 3 处已更新（校准注、§repair 段、ledger 段：251/269/408 行）；**但 Curve 4 表格行残留 "59 cycles"（见 R1）** | ⚠️ 基本关闭，附 R1 |
| C5（瞬态 87/13/0/0 + 2× 余量） | 已统一 | 论文 (7a)~(7d) 式全部更新且注明 "verified under active-edge release"；P(X≤1)=1.00、P(X≥3)=0 表述正确 | ✅ 关闭 |
| C6（脚本注释口径） | 已同步 | campaign 头注释 87/13/0/0 ✓；`eval_q115_torch.py:168` MTTR=77 ✓ | ✅ 关闭 |
| C7（抽象差距书面化） | 称已在交付清单书面化 | **论文 Fig.2 四曲线章节未申明**：元素级置零 vs RTL 行级广播、ABFT 128-ULP 阈/sat_r 豁免未建模、bist_reroute 无备用列容量限制、dual_loop 启发式概率、fault_rate 轴定义（论文写 "PE Fault Injection Rate"，实为每输出元素每前向）——均未见（见 R3） | ❌ 未关闭 |
| C8（QAT 码值差申明） | 称已书面化 | 论文仍称 DNN 评测与 RTL "100% bit-exact equivalence"，而四曲线战役实际跑 QAT 浮点卷积路径（G9 实测 ±1 码差 0.072%/0.345%）——**过度声称**（见 R2） | ❌ 未关闭 |
| C9（n_events / CI） | 称已书面化 | campaign CSV 字段仍为 `[mode, fault_rate, top1, sdc, due, n_samples]`，**无 n_events**；论文称"1,000–10,000 次 Monte Carlo + 95% Wilson CI"，脚本默认 256 样本且不输出事件数/CI——图文不一致（见 R4） | ❌ 未关闭 |
| C10（退出码 0 日志） | 已存盘 | `round2_gates_final.log` 在（16/0）；签核人独立复跑一致 | ✅ 关闭 |
| C11（主回归收官阶段） | 已追加 | `run_sim.sh:22,147-151`：`ORIG_PATH` 保存 + [收官] 阶段还原纯净 OSS PATH 后调用门控脚本，任一 FAIL 即整体失败 | ✅ 关闭 |
| C12（终审会） | — | 本次签核 | ✅ 本次完成 |

---

## 2. 残留修订条件（论文数据录入前必须完成，均为文档级，无需重跑门控）

| # | 位置 | 现状（签核人实测） | 要求修订 |
|---|---|---|---|
| **R1** | `IEEE_TCAD_Paper_Framework_and_Theory.md:384`（Fig.2 Curve 4 行） | 仍写 "`healed = 1` in **59 cycles**"，与论文自身 251/269/408 行的 77 矛盾 | 改为 "77 cycles（detect=2 + repair=75，含 S_DRAIN 排空）"；全仓库已无其他 59/61 残留（已 grep 确认） |
| **R2** | 论文 §V-A（bit-exact 声称段） | 称 PyTorch Q1.15 引擎与 RTL "100% bit-exact equivalence" 并以此覆盖 DNN 评测；实际四曲线跑 QAT 假量化路径 | 二选一：(a) 披露"四曲线战役运行于 QAT 假量化前向，与 40 位精确整数路径存在 ±1 码差（浅层 0.072% / 深层 0.345%，G9 门控实测），对 Top-1 曲线影响低于故障幅值"；(b) 用 `eval_q115_torch.systolic_matmul_q115` 精确路径复跑关键曲线 |
| **R3** | 论文 Fig.2 四曲线定义表 + ISO 26262 表 | Curve 2 按元素级置零给 Frobenius 界（RTL 为行级广播，Python 低估 DUE）；Curve 3 称 Only-BIST "100% accuracy recovery"（无单备用列容量限制）;Curve 4 继承 dual_loop 启发式；横轴标 "PE Fault Injection Rate" 而脚本注入粒度是每输出元素每前向 | 增补"抽象模型与 RTL 的已知差距"小节：① 掩蔽粒度（行级广播 vs 元素级）② ABFT 128-ULP 检测阈与 sat_r 饱和豁免未在 Python 侧建模 ③ bist_reroute 治愈率 100% 无容量限制 ④ dual_loop 不可愈概率 min(1, 2p) 为启发式 ⑤ fault_rate 轴定义（每输出元素每前向）及与"每 PE 故障率"的换算假设 |
| **R4** | `run_resnet18_fault_campaign.py:357` + 论文 Fig.2 轴规格 | CSV 无 n_events；论文承诺 1,000–10,000 次 + 95% Wilson CI | CSV 增加 `n_events` 列（及建议 `ci95` 列）；正式曲线使用样本量与论文声明一致（或按实际样本量修订声明） |

---

## 3. 签核决定

| 事项 | 决定 |
|---|---|
| **Phase 3 神经网络战役（ResNet-18/MobileNetV2 四曲线注入）执行** | **硬条件正式解除**。门控 16/16 经签核人独立复跑通过，误差模型（785 条 RTL 实测码 + 87/13/0/0 瞬态分布）、注入语义、消融四模式、SDC/DUE 记账均验证可信；ge3/float 两缺陷已闭环。战役可开跑。 |
| **IEEE TCAD 论文数据录入 — RTL/微架构/瞬态/Q1.15 全部数据** | **解除**。MTTR=77（detect 2 + repair 75，周期距离口径）、瞬态 87/13/0/0 与 P(X≤1)=1.00、THRESH=3 隔离余量 2×、2602 向量三方逐位一致、0 锁存器、持续灌流 0 误判——均可录入。 |
| **IEEE TCAD 论文数据录入 — DNN 四曲线章节（Fig.2 + ISO 26262 表）** | **暂不解除，附条件**。须完成 R1~R4 修订并提交 diff，由签核人复核（预计 10 分钟文档复核，无需重跑门控）。R1 为事实性错误（TCAD 审稿人一眼可辨），优先处理。 |
| **Round 2 终审会** | 代码侧签核通过；论文侧设 R1~R4 复核关口。 |

---

## 4. 签核记录

| 关口 | 结论 | 条件 / 责任人 |
|---|---|---|
| 16 项终审门控 | **通过**（签核人独立复跑，退出码 0） | 日志三份互为印证：`round2_gates_final.log`（申请人）/ `round2_gates.log`（签核人复跑）/ 本次记录 |
| F0/F1/F2 代码修复 | **通过** | — |
| F6/F7 代码修复 | **通过**（G7c/G7d 实测） | — |
| C1/C2/C3/C5/C6/C10/C11 | **关闭** | — |
| C4 | **基本关闭** | R1（论文组，10 分钟） |
| C7/C8/C9 | **未关闭** | R3/R2/R4（论文组 + sim-verifier 补 n_events） |
| Phase 3 战役执行准入 | **放行** | — |
| 论文数据准入 | **部分放行** | RTL 侧数据立可录入；DNN 章节待 R1~R4 复核 |

**遗留跟踪**：R1 → 论文组（事实修订）；R2/R3 → 论文组（声称与差距披露）；R4 → sim-verifier（CSV 增列）+ 论文组（样本量对齐）。四项完成后提交签核人复核，通过即全文数据录入全放行。

---

*证据文件：`sim/review_p23/round2_gates.log`（签核人 07:43 独立复跑，16/0）、`sim/review_p23/round2_gates_final.log`（申请人存档，16/0）、`sim/review_p23/review_evidence.log`（Round 1 证据）、`sim/review_p23/README.md`（门控库说明）。*

**签字（齐流晶）**：代码侧放行，战役开跑；论文 DNN 章节数据录入押后至 R1~R4 复核通过。R1 是本次唯一的事实性错误残留，建议优先处理——其余三项为披露义务，不涉及已录入 RTL 数据的正确性。
