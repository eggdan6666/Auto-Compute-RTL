# Round 2 验收准则与标准回归用例 — Trae 补丁提交检查单

| 项 | 内容 |
|---|---|
| 适用对象 | Trae 对 F0~F3（Round 1 复审整改）及 F6/F7（Round 2 Python 复审新发现）的代码级修复 |
| 验收人 | 齐流晶（FPGA 交付总监）；技术细节按 rtl-designer / sim-verifier 视角执行 |
| 制定日期 | 2026-10-14 |
| 验收方式 | **全部门控由验收人亲自复跑**（`bash sim/review_p23/run_round2_gates.sh`），不接受转述结论；退出码须为 0 |
| 关联报告 | `peer_review_phase2_phase3.md`（Round 1）、`peer_review_phase3_python_r1.md`（Round 2 Python 复审） |

---

## 0. 当前状态快照（2026-10-14 复审人实测）

Trae 已推送 F0 / F1 / F2 / P2-3 / P2-4 修复。门控实测：**14/16 PASS，2 FAIL**。

| 门控 | 结果 | 实测明细 |
|---|---|---|
| G1a 仓库版单拍扫查 | ✅ PASS | hist=[87,13,0,0]，busy 违例 0；仓库 TB 已自带"2 拍注入≥2 脉冲"反向校验 |
| G1b 独立参照扫查 | ✅ PASS | 87/13/0/0 |
| G1c 测量链可失败性 | ✅ PASS | 3 站点 2 拍注入均测得 2 脉冲 |
| G2 Phase2 闭环无回归 | ✅ PASS | 12/12；**MTTR 变为 detect=2 / repair=75 / total=77**（S_DRAIN 引入，见 §3-P1） |
| G3a MTTR detect 解毒 | ✅ PASS | 毒化场景 detect_latency=2（修复前 190） |
| G3b MTTR repair 解毒 | ✅ PASS | 毒化场景 repair_latency=75（修复前 248） |
| G4a 瞬态簇+灌流 | ✅ PASS | 无 unhealable、无伪重映射（healed=0, fault_col=5） |
| G4b 永久故障+灌流 | ✅ PASS | healed=1, fault_col=1, repair=76 |
| G5 Q1.15 三方一致 | ✅ PASS | 2602 向量 0 mismatch |
| G6 综合卫生 | ✅ PASS | 0 锁存器；No-latch 确认 269 处；check -assert 退出 0 |
| G7a 概率钳位 | ✅ PASS | 不可愈比例 0.1999≈2×rate；源码含 min(1.0,...) |
| G7b 饱和域 | ✅ PASS | 噪声域 [-32768, +32767] |
| **G7c ge3 误分类** | ❌ **FAIL** | 瞬态分布含 ge3>0 时战役脚本静默按 3 脉冲处理，无任何申示 |
| **G7d float 基线模式** | ❌ **FAIL** | `--mode float --quick` 仍 TypeError 崩溃 |
| G8 战役冒烟 | ✅ PASS | --synthetic 全管线，CSV 8 行 |
| G9 QAT 数值差 | ✅ PASS | 0.072% / 0.345% ≤ 0.5% 上限（论文须申明，见 §3-P4） |

**结论：F0/F1/F2 修复验收通过；F6（G7c）、F7（G7d）未修复，另附 4 项论文口径/文档条件（§3）。补齐前不予 Round 2 终审签核。**

---

## 1. 修复项 → 门控映射（Trae 提交前自查）

| 编号 | 问题 | 验收门控 | 当前 |
|---|---|---|---|
| F0 | 扫查双拍伪迹（"≤2 脉冲"结论混淆） | G1a + G1b + G1c | ✅ 已过 |
| F1 | MTTR 测量双侧毒化 | G3a + G3b + G2 | ✅ 已过 |
| F2 | BIST 在飞数据污染 | G4a + G4b + G2 | ✅ 已过 |
| P2-3 | dual_loop 概率越界 | G7a | ✅ 已过 |
| P2-4 | 饱和域右开区间差 1 码值 | G7b | ✅ 已过 |
| **F6** | **ge3 瞬态分布静默误分类（≥3 脉冲实为触发 BIST，非可掩蔽瞬态）** | **G7c** | ❌ 待修 |
| **F7** | **`--mode float` 浮点基线路径崩溃** | **G7d** | ❌ 待修 |

## 2. Trae 提交规范（硬性）

1. **提交物**：补丁 diff + 一份**由提交人亲自执行**的 `run_round2_gates.sh` 完整日志（`round2_gates.log`）。验收人将独立复跑，两份日志不一致即打回。
2. **不得为过门控而弱化判据**：门控脚本与判据的修改须经复审人同意；私自放宽判据视为验收失败。
3. **修复须最小化**：F6/F7 之外的 RTL/脚本改动须在提交说明中单独列明并说明理由。
4. **回归范围**：`sim/run_sim.sh` 全流程（含 SVA/Verilator 阶段）不得因补丁退化。

## 3. Round 2 终审签核条件（全部满足才签核）

### 3.1 代码修复条件

- **C1【F6，sim-verifier】**：`run_resnet18_fault_campaign.py` 的 `_load_transient_distribution` 对 ge3>0 必须**显式失败或告警**（推荐：`assert` 并提示"≥3 脉冲将触发 BIST，不属于可掩蔽瞬态，禁止按 pulses=3 处理"）。判据见 G7c。
- **C2【F7，rtl-designer】**：`train_cifar10_q115.py` 的 `--mode float` 必须可用。根因：`ResNet18CIFAR` 无条件向 `linear_cls` 传 `quantize_output=False`，`nn.Linear` 不接受。修法（任选）：按 `linear_cls is Q115Linear` 条件传参；或 `nn.Linear` 兼容包装；或 `inspect.signature` 探测。判据见 G7d。
- **C3【F1 附条件，rtl-designer】**：G2 实测 MTTR 已因 S_DRAIN 由 59→75（total 61→77）。请确认这是预期代价（每次 BIST 前等待阵列输出静默），并在交付说明中写明；若可用更短静默判据（如连续 NPC 拍 valid_out==0 即通过，而非固定拍数）优化，须复跑 G2/G4b 确认 repair 不劣化。

### 3.2 论文口径条件（主理人把关，不入代码门控）

- **C4【P4，论文】**：`eval_q115_torch.py` dual_loop 注释"总 MTTR = 61 cycles"已过期 → 更新为 **77 cycles**（detect 2 + repair 75，周期距离口径；若按"含首尾"口径为 3+76=79，须全文统一）。
- **C5【P4，论文】**：瞬态辩护数字更新为"单事件瞬态脉冲数 **≤1**（87% 自然吸收 / 13% 单脉冲 / 0 双脉冲，100 例），THRESH=3 留 **2× 余量**"；原"≤2 / 70-21-9"不得再出现。仓库 TB 的 PASS 文案已同步更新，以它为准。
- **C6【P4，论文】**：`run_resnet18_fault_campaign.py` 头注释仍写"0/1/2/ge3 = 70/21/9/0" → 更新为 87/13/0/0（其读取的 CSV 已被门控刷新为新分布，行为已正确，仅注释过期）。
- **C7【P5，论文】**：故障注入抽象模型须写明与 RTL 的差距（详见 `peer_review_phase3_python_r1.md` §3.4）：① 零掩蔽为**行级广播**（RTL）vs 元素级（Python），Python 低估 DUE 严重度；② ABFT 有 128-ULP 检测阈与 sat_r 饱和豁免，Python 假设 100% 检出；③ `bist_reroute` 100% 治愈无备用列容量限制；④ dual_loop 不可愈概率 `min(1, 2×fault_rate)` 为启发式，未按"≥2 个不同物理列故障"推导；⑤ fault_rate 轴须定义（每输出元素每前向）。
- **C8【P6，论文/训练】**：QAT 路径与硬件精确路径存在 ±1 码差（实测 0.072%@3→16、0.345%@64→64，全部位于 RNE 平手边界；G9 已记录）。四曲线战役跑在 QAT 路径上，论文须申明该近似，或用 `eval_q115_torch` 精确路径复跑关键曲线。另：BN 以浮点留在图中、无 BN 折叠导出路径，"硬件推理"实为混合模型，须申明。
- **C9【P6，实验设计】**：`--max-samples 256` 下低故障率点事件数不足，CSV 未记录事件数与置信区间。建议：输出列增加 `n_events`，论文曲线标注二项置信区间，或对 1e-6~1e-5 点加大样本量。

### 3.3 流程条件

- **C10**：F6/F7 修复后重跑全门控，退出码 0，日志存档为 `sim/review_p23/round2_gates_final.log`。
- **C11**：`sim/run_sim.sh` 增加第 [13/13] 阶段：`bash sim/review_p23/run_round2_gates.sh`（门控入正式回归）。
- **C12**：Round 2 终审签核会须覆盖：门控全过 + C1~C11 关闭 + 论文数字与 C4~C9 口径一致。

## 4. 签核记录

| 关口 | 结论 | 条件 / 责任人 |
|---|---|---|
| F0 扫查单拍语义 | **通过**（2026-10-14 实测 14/16 中的 G1a/G1b/G1c） | — |
| F1 MTTR 解毒 | **通过**（G3a/G3b，G2 无回归） | C3：S_DRAIN 代价确认（rtl-designer） |
| F2 BIST 灌流韧性 | **通过**（G4a/G4b） | — |
| P2-3 / P2-4 | **通过**（G7a/G7b） | — |
| F6 ge3 防护 | **未通过** | C1（sim-verifier） |
| F7 float 模式 | **未通过** | C2（rtl-designer） |
| 论文口径 | **未关闭** | C4~C9（主理人 + 论文撰写） |
| **Round 2 终审签核** | **有条件，暂不签核** | C1+C2 代码修复 + C4~C9 口径统一 + C10~C12 流程关闭后，提交 Round 2 终审 |

---

*门控运行方式与文件清单见 `sim/review_p23/README.md`；Round 1 证据见 `peer_review_phase2_phase3.md` 与 `sim/review_p23/review_evidence.log`。*
