# 《Phase 2 / Phase 3 同行复审意见报告》— 闭环自愈与瞬态扫查深度审查

| 项 | 内容 |
|---|---|
| 复审对象 | `rtl/fault_arbiter.sv`（32 拍滑动窗故障去抖裁决器）、`rtl/self_healing_array.sv`（闭环触发接回 + MTTR 周期计数）、`tb/tb_transient_sweep.sv`（100 次单拍瞬态注入扫查平台）、`eval_q115_torch.py`（Q1.15 定点硬件感知算子） |
| 复审背景 | Phase 2 外循环闭环自愈与 Phase 3 瞬态扫查代码已在工作区落地，拟将实测数据用于论文辩护（THRESH=3 分离性、MTTR=61 cycles） |
| 复审人 | 齐流晶（FPGA 交付总监）；技术细节按 rtl-designer / sim-verifier 视角执行 |
| 复审日期 | 2026-10-14 |
| 复审方法 | 代码走查 + **全部亲自复跑**：Icarus Verilog（回归复现、全通道 tracing、最小语义探针、对照实验）、Yosys 0.69（锁存器推断检查）、Python/torch 2.14（Q1.15 算子三方逐位比对）。不复述转述结论，证据索引见 §7 |

---

## 0. 签核结论

# ⚠️ 有条件通过（Conditional Pass）— 附 1 项 P0 硬条件

**自愈闭环功能本体无回归**（Phase 2 闭环回归 12/12 PASS，故障定位、热替换、自愈后黄金一致均正确）；本次问题集中在**测量基础设施与论文辩护证据的可信度**，不在功能本体。

| 审查范围 | 结论 |
|---|---|
| `fault_arbiter.sv` 时序/锁存器/running-sum 安全性 | **PASS**（全域参数下无下溢/无溢出，结构性得证；0 锁存器） |
| 闭环触发与抗震荡（busy 屏蔽、二次误触发） | **PASS**（busy 屏蔽无 1 拍空窗；`hard_q` 锁存防重触发） |
| MTTR 计数精度 | **FAIL → 需修复**（detect/repair 双侧毒化实证；口径 off-by-one） |
| `tb_transient_sweep.sv` "单拍瞬态脉冲数 ≤ 2" 结论 | **FAIL → 需重跑**（Icarus force/release 双拍伪迹混淆，真值 ≤ 1） |
| BIST 对在飞任务数据的鲁棒性 | **FAIL → 需修复**（持续任务流下伪 unhealable 实证） |
| `eval_q115_torch.py` Q1.15 算子 | **PASS**（2602 向量三方逐位一致，0 mismatch） |

**进入论文写作/Phase 4 的前置硬条件：F0（P0）+ F1 + F2（P1）修复并复跑通过。** 在修复前，不得将以下数据写入论文：①"单事件瞬态脉冲数严格 ≤ 2"及 P(k) 分布（87/13/0/0 为修正后真值）；②任何非清洁单事件场景下的 MTTR 数值。

---

## 1. 问题分级总表

| 编号 | 级别 | 位置 | 问题摘要 | 实证 |
|---|---|---|---|---|
| P0-1 | **P0** | `tb/tb_transient_sweep.sv:210` | Icarus `force@negedge + release@下一negedge` 使下游采到 **2 个连续脏拍**，"严格仅 1 拍"注释不成立；"≤2 脉冲"结论系双拍伪迹 | §4.1 探针 + §4.2 对照实验 |
| P1-1 | **P1** | `self_healing_array.sv:303-308` | `first_err_seen` 仅由 trigger 清除：未触发的早期瞬态使 `detect_cnt` 持续虚涨，毒化后续 `detect_latency` | §5.2 场景 1（190 vs 真值 2） |
| P1-2 | **P1** | `self_healing_array.sv:311-322` | BIST1 无故障通过 / unhealable 路径下 `repair_running` 永卡 1，毒化后续 `repair_latency`；`healed` 为粘滞标志 | §5.2 场景 2（248 vs 真值 59） |
| P1-3 | **P1** | `bist_controller.sv:86-103` + `self_healing_array.sv:131-133` | BIST 无在飞数据隔离：任务流排空数据污染 MISR，持续灌流下**无真实故障也判 unhealable**（或伪重映射健康列） | §5.3 场景 3 + 灌流探针 |
| P2-1 | P2 | `self_healing_array.sv:285-287` | MTTR 注释口径 off-by-one：实测为周期距离（2/59/61），"含首尾"应为 3/60/62 | §5.1 |
| P2-2 | P2 | `tb/tb_transient_sweep.sv` 覆盖度 | 仅覆盖 psum_out stuck-at-0 @ 16 个逻辑列 PE、填充期注入；校验列/激活寄存器/权重寄存器/随机位翻转/稳态排空期均未覆盖 | §4.3 |
| P2-3 | P2 | `eval_q115_torch.py:170` | `fault_rate*2.0` 概率在 fault_rate>0.5 时 >1，概率语义破坏 | §6.2 |
| P2-4 | P2 | `eval_q115_torch.py:151,163,173,251,263` | `randint_like(high=SAT_POS)` 右开区间排除 +32767，与 RTL 饱和域差一个码值 | §6.2 |
| P2-5 | P2 | `eval_q115_torch.py` 故障模型 | 输出元素级伯努利模型与 RTL 行级零掩蔽/物理列级重映射存在抽象差距，fault_rate→PE 故障率映射未定义 | §6.2 |
| P3-1 | P3 | `self_healing_array.sv:232` | `busy` 拉高后 1 拍 `bist_start` 才寄存，该拍阵列输入 mux 仍放行任务数据（无害，建议 `!busy` 门控） | §3.2 |
| P3-2 | P3 | `fault_arbiter.sv:32` | `THRESH_W` 截断无参数断言（THRESH>WINDOW_SIZE 时静默荒谬） | §3.1 |
| P3-3 | P3 | 架构语义 | 裁决器为 32 拍**速率检测器**：32 拍内 3 个独立瞬态即触发伪愈合，论文须书面化"瞬态稀疏/不相关"假设 | §3.2 |
| P3-4 | P3 | `self_healing_array.sv:39-41,320` | `heal_start` 路径不测 MTTR；`total_mttr` 16-bit 加法截断（当前量级无虞） | §5.1 |

---

## 2. 复审已确认正确的部分（先说明什么是稳的）

1. **running-sum 滑动窗计数全域参数安全**（§3.1 给出结构性证明，非"碰巧安全"）。
2. **busy 对裁决器的屏蔽无 1 拍空窗**；`hard_q` 锁存使自愈后排空期间不会二次误触发（§3.2，含实证）。
3. **自愈闭环功能本体正确**：Phase 2 闭环回归 12/12 PASS；自主触发→BIST 定位（fail_col=1）→重映射→复验→自愈后任务输出与黄金一致（§3.2、§5.3 场景 B 复刻）。
4. **MTTR 在"复位后单次干净事件"下测量正确且内部自洽**：detect=2、repair=59、total=61=2+59（§5.1）。
5. **Q1.15 算子与硬件逐位对齐**：`rne_sat16_torch` == 独立纯整数参考模型 == RTL `rne_sat16.sv`，2602 向量（半点 tie、饱和轨、MIN40/MAX40、随机）0 mismatch（§6.1）。

---

## 3. 逐项审查记录

### 3.1 `fault_arbiter.sv` — 时序与锁存器检查（对应用户问题 1）

**结论：PASS。** 逐条回答：

**(a) running-sum 增量计数是否存在下溢/溢出风险？——不存在，且对任意 WINDOW_SIZE 参数成立。**

`count_next = count + err_in − window[MSB]`（CNT_W = $clog2(WINDOW_SIZE+1) 位宽）：

- **下溢不可能**：`count` 是当前窗全部 WINDOW_SIZE 位的汉明权重，恒有 `count ≥ window[MSB]`，故 `count − window[MSB] ≥ 0`（精确算术与模 2^CNT_W 算术下均成立）。
- **上溢不可能**：可达最大值 `max(count − window[MSB]) = W−1`（count=W 时 MSB 必为 1；count=W−1 且 MSB=0 可达），加 `err_in ≤ 1` 得 `count_next ≤ W`。而 `2^CNT_W = 2^clog2(W+1) ≥ W+1 > W`，恒可容纳。
- 当前实例 WINDOW_SIZE=32：CNT_W=6（表示域 0..63），`count_next ≤ 32` ✓；WINDOW_SIZE=31/15/7/3 等同理安全（该公式恰好充分，无需加宽）。

**(b) 是否有未意图推断的 Latch？——无。**

- 模块内**没有任何 `always_comb`**；全部组合逻辑为连续赋值（`count_next`/`hard_cur`/`trigger_heal`/`err_in`），全部时序状态集中于单一 `always_ff @(posedge clk or negedge rst_n)` 且复位全覆盖（`window`/`count`/`hard_q`/`transient_cnt`）。
- Yosys 0.69 全栈（9 个 RTL 文件）`hierarchy -check; proc; opt` 后 **$dlatch/$dlatchsr 单元数 = 0**；`check -assert` 退出码 0（无 latch / 组合环 / 多驱动问题）；proc 阶段逐信号 "No latch inferred" 确认（含 `window`/`count`/`hard_q`/`transient_cnt`）。

**(c) 附带确认**：`trigger_heal = hard_cur && !hard_q` 为严格的 1 拍边沿脉冲；`hard_cur` 基于 `count_next`（含当拍入窗脉冲、出窗最旧位），**跨阈当拍即触发**——场景 B 实证：3 个连续错误脉冲在第 3 拍准确拉起 trigger（§5.3 trace：c=16/17/18 脉冲 → c=18 触发）。

**(d) P3 建议**：`THRESH_W = THRESH`（`:32`）按 CNT_W 截断，THRESH>WINDOW_SIZE 时静默变为不可达/荒谬值，建议加 `initial if (THRESH > WINDOW_SIZE) $error(...)` 参数断言。

### 3.2 闭环控制与抗震荡（对应用户问题 2）

**结论：PASS（附 1 项 P3 假设说明）。**

**(a) busy 对裁决器的屏蔽时序是否存在 1 拍空窗期？——不存在。** 实证时序（`tb_heal_probe` trace，场景 B 复刻）：

| 拍 | c=18（触发拍） | c=19 | c=20 |
|---|---|---|---|
| `trigger_heal` | 1 | 0 | 0 |
| `busy` | 0 | **1** | 1 |
| `bist_busy` | 0 | 0 | **1** |

- `busy = (hstate != H_IDLE)` 由寄存器 hstate 驱动，在触发拍的下 1 拍（T+1）精确拉高；`err_in = err_pulse && !busy` 从 T+1 起全屏蔽，触发拍 T 本身不屏蔽（该拍脉冲正是触发源，应计入）——**无未屏蔽空窗**。
- 双保险：`abft_enable = (fault_col_r == NO_FAULT) && !busy`，自愈期间 ABFT 源头关闭，`abft_err` 恒 0。
- **P3（间隙在另一处）**：`busy` 拉高后 1 拍 `bist_start` 才寄存、`bist_busy` 再晚 1 拍，故 T+1 拍阵列输入 mux 仍放行任务数据（`bist_active=0`）。无害（该拍 ABFT 已关），但建议 `phy_valid_in` 增加 `!busy` 门控一并收敛（与 P1-3 修复合并）。

**(b) 是否存在二次误触发风险？——设计上已防，残留 1 项须书面化的假设。**

- **自愈期间**：err_in 屏蔽 + ABFT 关闭 → 零脉冲（实证：场景 B `transient_cnt` 自愈前后恒为 2，故障持续存在也未计数）。
- **自愈后残留窗**：若 MTTR < 32（如 BIST1 无故障通过约 30 拍），busy 落片时窗内可能仍有旧脉冲；此时 `hard_q` 已锁存为 1，`trigger_heal` 在窗内计数排空至 <THRESH 前不会重发 ✓。当前 MTTR=59>32，窗口必已排空，双保险。
- **P3 假设（须写入论文）**：裁决器本质是 32 拍**速率检测器**——32 拍窗内任意 3 个脉冲（含 3 个相互独立的单拍瞬态）都会触发一次（伪）愈合。THRESH=3 的分离性成立于"瞬态稀疏/不相关"假设之下，该假设目前未在任何文档中书面化。
- `heal_start` 在 busy 期间被 FSM 忽略（仅 H_IDLE 响应）✓ 无双触发路径。

### 3.3 MTTR 计数精度（对应用户问题 3）— **FAIL**

**(a) 口径 off-by-one（P2-1）**：实测 detect_latency=2 / repair_latency=59 / total_mttr=61 均为**周期距离**（T_trigger−T_first_err、T_healed−T_trigger）；而代码注释（`:285-287`）写"含首尾"，按该口径应为 3 / 60 / 62。`total = detect + repair` 内部自洽 ✓。论文必须统一口径（建议保留周期距离、修订注释）。

**(b) P1-1：detect 侧毒化（实证）**。`first_err_seen` 只由 `arb_trigger_heal` 清除（`:302`）；一次未达阈的早期瞬态会使其常置、`detect_cnt` 无限虚涨（16-bit 回绕）。后续真实硬故障的 `detect_latency` 从"历史首个错误"起算：

```
[S1] 瞬态注入后: pulse_delta=1, fes=1, dcnt=7
[S1] 静默 200 拍后: fes=1, dcnt=177            ← 无错误仍在虚涨
[S1] >>> healed=1 fault_col=1 detect_latency=190 (真值=2, 虚高 188 拍), repair=60, total=250
```

**(c) P1-2：repair 侧毒化（实证）**。BIST1 无故障通过（瞬态误触发）或 unhealable 路径下 `healed` 不置位 → `repair_running` 永卡 1、`repair_cnt` 永动；后续真实自愈完成时取到毒化值：

```
[S2] BIST1 无故障通过后: busy=0 healed=0 prun=1 (卡死!) pcnt=35
[S2] >>> healed=1 fault_col=1 detect_latency=2 (正确) repair_latency=248 (真值~59, 虚高 189 拍)
```

另：`healed` 为**粘滞标志**（永不清零）——在当前单故障架构下第二事件不可达（`fault_col_r != NO_FAULT` 后 ABFT 永关），但属定时炸弹，建议随修复一并处理（改脉冲或 heal 完成即清）。

**(d) 修复建议**：① detect 侧：`first_err_seen` 置位后连续 N 拍（如 40 > 窗深）无新 `abft_err` 则清哨复位；② repair 侧：hstate 回 H_IDLE 且 `!healed` 时清 `repair_running`；③ `healed` 改脉冲语义；④ 修复后以 §5.2 两场景作回归。

### 3.4 `tb_transient_sweep.sv` — 瞬态实测可信度（对应用户问题 4）— **FAIL（P0-1）**

**(a) 原结论复现**（亲自复跑，与 `sim/transient_distribution.csv` 一致）：

```
0 脉冲: 70 次 (70.0%)   1 脉冲: 21 次 (21.0%)   2 脉冲: 9 次 (9.0%)   ≥3: 0   误停机: 0
```

**(b) 根因实证——Icarus force/release 的双拍语义**。最小探针（`tb_force_probe2.sv`，对 flop 输出 force 0 后观察下游消费者采样）：

| 变体 | 下游 `sampled_q` 采到 0 的 posedge | 含义 |
|---|---|---|
| V1 = 现行 TB 模式（force@negedge，release@下一 negedge） | **posedge#8 和 #9 两个** | 实际 2 拍注入 |
| V2 = posedge 当拍（active）release | 仅 posedge#8 | 真单拍注入 |

Icarus 中 `release` 直到下一个 posedge 的 NBA 才生效——TB 注释"严格仅 1 拍单事件"（`:210`）**在 Icarus 下不成立**。

**(c) 机制链条（全通道 trace 实证，`tb_dbg_sweep2.sv`）**：2 拍注入 → 列 0 出口两个连续脏拍（`p_v[4][0]` = `fffc800000` / `0002800000`，对照基线 `fffd000000` / `0001800000`）→ 两个被腐化的 aligned 拍（`align_data[0]` = `f900` / `0500`，对照 `fa00` / `0300`）→ **2 个 error_pulse**（diff 分别为 −256 / +512）→ `transient_cnt` 增量 2。第一拍缺项 `A[1][0]·W[0][0]=2^23`、第二拍混入杂项 `2^24`，与两拍注入的流水线行为精确吻合。

**(d) 对照实验（决定性）**：同一仿真器、**同一随机场景序列**（默认种子确定性），仅将 release 改到 posedge 当拍：

```
真单拍注入 100 例： 0 脉冲: 87 (87.0%)   1 脉冲: 13 (13.0%)   2 脉冲: 0 (0.0%)   ≥3: 0   误停机: 0
```

→ **单拍瞬态的脉冲数真值为 ≤ 1**；原"≤2"及 9% 的 2 脉冲占比是双拍伪迹的产物。

**(e) 结论评估**：
- THRESH=3 的分离性**依然成立且余量更大**：单事件瞬态真值 ≤1（修正后实测）< 3，余量由 1× 变为 2×；永久硬故障每 aligned 拍必错、3 拍内越阈，分离稳健 ✓。
- 但论文若写"实测 P(pulses=k)，max=2"则**与机制不符**；且该数字是仿真器相关的（按 LRM 合规实现 release 的仿真器，如 Verilator，会直接给出 max=1）——**可复现性风险**，同行一质疑即崩。
- **P2-2 覆盖度缺口**：仅 psum_out stuck-at-0 @ 16 个逻辑列 PE；未覆盖：校验列（物理列 4）注入、`a_out`/`av_out`/`w_reg`/出口寄存器 upset、随机位翻转值（仅试过 0）、稳态/排空期注入（`inj_delay∈1..4` 仅覆盖管线填充期）、真实多事件（≥2 个独立瞬态）。
- **修复建议（F0）**：① release 改 posedge 当拍（active region）或 `@(posedge clk); #0; release;`；② 修正后重跑并**以 ≤1 / 87-13-0-0 更新论文数字**，同时把"2 拍注入→2 脉冲"作为机制说明写入附录（反而是更好的物理论证：脉冲数 = 脏拍数 ≤ 注入宽度）；③ 补充站点/相位/故障值覆盖；④ PASS 准则增加反向校验（对已知 2 拍注入必须报 ≥2，否则准则本身失效）。

### 3.5 BIST 对在飞任务数据的鲁棒性（复审新发现，P1-3）

**(a) 实证一（伪 unhealable）**：持续任务流 + 瞬态簇触发自愈：

```
[S3] 无任何真实故障 >>> healed=0 unhealable=1 fault_col=0   ← BIST 被排空任务数据污染
```

灌流探针 trace：触发时管线满载，任务排空数据在 BIST RUN 早期继续流出（`rcvd` 计数 17 > MB=16，MISR 混入任务值）→ BIST1 误判 fail_col=0 → 复验 BIST2 同样被 BIST1 尾部数据污染 → unhealable。

**(b) 实证二（现行回归为何没抓住）**：场景 B 时序复刻 trace 显示 BIST1 实际 `pass=0`（排空数据污染列 3/4），**仅因 `fail_col` 取最低故障列 = 1（恰为真实故障列）而被掩盖**，BIST2（管线已干净）通过 → healed=1。即：现行回归的 B.1~B.7 全绿与该中间失败并存，检查不到。

**(c) 风险评估**：论文"任务期间透明自愈"的卖点在持续灌流下不成立；且存在**伪重映射变体**（无真实故障但健康列被永久旁路、备用列耗尽、ABFT 永久失效）。campaign 回归因每场景复位+静默灌流而未触及。

**(d) 修复建议（F2）**：① `phy_valid_in = bist_active ? bist_valid : (valid_in && !busy)`——自愈起始即停止新任务流入；② BIST 控制器增排空等待：S_RUN 前等待 `arr_valid_out` 连续 NPC 拍为 0（或 MISR 使能增加"阵列输出静默后"条件）；③ 回归增加持续灌流场景，期待"正确自愈"或明确的"任务未静默"指示，而非伪 unhealable。

### 3.6 `eval_q115_torch.py` — Q1.15 硬件感知算子（对应用户问题 4 之一）

**(a) 三方逐位一致性：PASS。** 构造 2602 个 40-bit 向量（半点 tie、饱和轨两侧、MIN40/MAX40、密集饱和区、随机），`rne_sat16_torch` == 独立纯整数参考模型 == RTL `rne_sat16.sv`（逐拍灌入 `tb_rne_check.sv`），**0 mismatch**。Guard/Sticky/LSB 提取、RNE 判定、先舍入后 SAT16、40-bit clamp（规格内为恒等，N≤511 累加器不溢出）全部对齐。

**(b) P2-3**：`unhealable_mask` 概率 `fault_rate*2.0` 在 fault_rate>0.5 时 >1 → `rand < p` 恒真 → 全部故障判不可愈，概率语义破坏。建议 `min(1.0, fault_rate*2.0)`。

**(c) P2-4**：`torch.randint_like(res, low=SAT_NEG, high=SAT_POS)` 右开区间排除 +32767，与 RTL 饱和域 [−32768, +32767] 差一个码值；建议 `high=SAT_POS+1`。

**(d) P2-5**：故障模型为输出元素级伯努利，与 RTL 的行级零掩蔽 / 物理列级重映射存在抽象差距；`fault_rate`（每输出元素）到每 PE 故障率的映射未定义。作为 ISO 26262 式抽象模型可用，但论文须写明这是**抽象模型**并给出映射假设，不得暗示与 RTL 注入实验等价。

**(e) P3**：硬编码"总 MTTR = 61 cycles"与实测一致（周期距离口径）✓，保持同步即可；`systolic_matmul_q115` 无 40-bit 截断（与 RTL `s41[39:0]` 截断的差异在规格内不可达），建议注释说明。

---

## 4. 关键实证数据索引（论文可用口径）

| 指标 | 原 TB（双拍伪迹） | 修正后（真单拍） | 论文建议采用 |
|---|---|---|---|
| 单事件瞬态脉冲数上界 | ≤ 2（9% 占 2 脉冲） | **≤ 1** | **≤ 1**（机制：脉冲数 = 腐化出口拍数 = 注入宽度） |
| 分布 P(k) | 70/21/9/0 | **87/13/0/0** | 87/13/0/0（100 例，Icarus） |
| THRESH=3 分离余量 | 1× | **2×** | 2× |
| 误停机（busy 违例） | 0 | 0 | 0 |
| MTTR（周期距离口径） | detect=2, repair=59, **total=61** | 同左 | 61（注明口径；"含首尾"口径为 3/60/62） |
| Q1.15 算子一致性 | — | 2602 向量 0 mismatch | PASS |

---

## 5. 签核记录

| 关口 | 结论 | 条件 / 责任人 |
|---|---|---|
| 模块级复审（`fault_arbiter.sv`） | **PASS** | P3-2 参数断言（rtl-designer，葛逻辑） |
| 子系统级复审（`self_healing_array.sv` 闭环） | **有条件通过** | P1-1/P1-2 MTTR 测量状态机修复（rtl-designer）；P1-3 BIST 隔离（rtl-designer + impl 评估） |
| 验证平台复审（`tb_transient_sweep.sv`） | **FAIL** | F0：release 时序修正 + 重跑 + 覆盖度补充（sim-verifier，魏波形） |
| Q1.15 算子一致性 | **PASS** | P2-3/P2-4/P2-5 随下一迭代清理 |
| 论文数据准入 | **附硬条件** | F0/F1/F2 修复并复跑通过前，§4 右列以外数据不得入论文 |

**遗留问题跟踪**：F0 → sim-verifier；F1/F2 → rtl-designer；F3（毒化与灌流回归入 `run_sim.sh`）→ sim-verifier；F4/F5 → rtl-designer + 论文撰写口径统一（主理人把关）。

---

## 6. 修复清单（按优先级）

- **F0【P0】** `tb_transient_sweep.sv`：release 改 posedge 当拍；重跑 100 例；论文数字更新为 ≤1 / 87-13-0-0；补站点×相位×故障值覆盖；PASS 准则加反向校验。（sim-verifier）
- **F1【P1】** `self_healing_array.sv` MTTR 测量状态机：detect 侧加超时清哨；repair 侧在非 healed 收场时清 `repair_running`；`healed` 改脉冲。修复后跑 §5.2 两场景回归。（rtl-designer）
- **F2【P1】** BIST 在飞数据隔离：`phy_valid_in` 增 `!busy` 门控；BIST 控制器增排空等待；补持续灌流回归场景。（rtl-designer + sim-verifier）
- **F3【P1】** 将毒化两场景 + 灌流场景 + 修正版扫查纳入 `sim/run_sim.sh` 回归阶段。（sim-verifier）
- **F4【P2】** MTTR 注释口径统一（周期距离，删"含首尾"）；`eval_q115_torch.py` 概率/区间两处修正 + 抽象模型文档化。（rtl-designer / 论文）
- **F5【P3】** `fault_arbiter` 参数断言；rate-detector 假设书面化；1 拍 `bist_active` 间隙门控；`total_mttr` 位宽注释。（rtl-designer）

---

## 7. 复审证据索引（全部可复跑）

| 证据 | 文件 |
|---|---|
| 复审证据汇总日志 | `sim/review_p23/review_evidence.log` |
| force/release 语义探针（V1/V2/V3） | `sim/review_p23/tb_force_probe2.sv`（+V=1/2/3） |
| 原版扫查复现（双拍伪迹） | `sim/review_p23/tb_sweep_orig.vvp` → `sim/transient_distribution.csv`（70/21/9） |
| 修正版真单拍扫查 | `sim/review_p23/tb_sweep_1beat.sv` → `sim/review_p23/transient_distribution_1beat.csv`（87/13/0） |
| 2 脉冲机制全通道 trace | `sim/review_p23/tb_dbg_sweep2.sv`（基线 vs 注入对照） |
| MTTR 毒化三场景 | `sim/review_p23/tb_mttr_poison2.sv`（+SCEN=1/2/3） |
| BIST 污染探针（灌流 / 场景B复刻） | `sim/review_p23/tb_heal_probe.sv`、`tb_heal_probe_b.sv` |
| Q1.15 三方比对（2602 向量） | `sim/review_p23/gen_rne_vectors.py`、`tb_rne_check.sv`、`rne_in.hex`/`rne_exp_ref.hex`/`rne_out_rtl.hex` |
| Yosys 锁存器检查 | `sim/review_p23/yosys_latch_check.log`（proc+opt 后 $dlatch=0；`check -assert` 通过） |
| Phase 2 闭环回归复跑 | `vvp -N /tmp/tb_phase2.vvp`（12/12 PASS，detect=2/repair=59/total=61） |

*工具链：Icarus Verilog（oss-cad-suite）、Yosys 0.69+272、Python 3.11 + torch 2.14.1+cpu；工程路径 `E:\项目集合\...\Auto-Compute-RTL`。*

---

**复审人签字（齐流晶）**：本次复审功能本体放行、测量与证据基础设施打回。F0 不修复，论文的瞬态辩护章节不具可复现性；F1/F2 不修复，MTTR 与"透明自愈"两项核心卖点均存在被同行实验复现推翻的路径。三处修复均有现成回归场景（§7），预计改动量小，建议立即执行后提交 Round 2 复审。
