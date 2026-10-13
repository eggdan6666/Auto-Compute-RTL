# Peer Review 报告：`abft_checksum.sv` × `zero_mask.sv` 交叉审查

| 项 | 内容 |
|---|---|
| 审查对象 | `rtl/abft_checksum.sv`（内循环 ABFT 校验和检测）、`rtl/zero_mask.sv`（列出口零掩蔽单元） |
| 审查人 | 齐流晶（FPGA 交付总监，主理人）；技术细节按 **rtl-designer（葛逻辑）视角**执行并标注 |
| 审查日期 | 2026-10-14 |
| 审查依据 | `systolic_array_spec.md` §4.2、`MAC_microarchitecture_spec.md`、`rtl-designer` 角色规范、Icarus/Verilator/Yosys 工具证据 |
| 审查性质 | 集成前门禁（两模块当前**未实例化、无 TB、未入构建流**，属"刚生成待审"状态） |

---

## 0. 结论摘要

**总判定：Needs Revision（需修改后复审）**

| 模块 | 判定 | 说明 |
|---|---|---|
| `zero_mask.sv` | **Pass（附 1 条 P2 建议）** | 逻辑与 DLY 约定正确；仿真证明其在 abft 修复后可精准同拍掩蔽 |
| `abft_checksum.sv` | **Needs Revision** | 存在 **P0 级 off-by-one 时序漏洞**（斜切延迟比设计意图多 1 拍），另有关键路径（P1）等 |

问题分布：**P0 × 1，P1 × 3，P2 × 5，P3 × 2**。P0 已用仿真实证（原始版 FAIL / 一行修复版 PASS），修复补丁见 §4。

---

## 1. 审查方法与证据链

1. **静态分析**：通读两模块及集成上下文（`self_healing_array.sv` / `systolic_array.sv` / `systolic_pe.sv` / `rne_sat16.sv` / `self_healing_array_sva.sv`），按 spec §4.2 建立拍号推演。
2. **仿真实证**：新建证据 TB `tb/tb_abft_zero_mask_align.sv`（两模块此前的第一个 TB），绕过阵列、按 spec §4.2 列出口拍律直接驱动（逻辑列 c 第 r 行数据于拍 `r+N+c+1`、校验列于拍 `r+2N+1`，复现 (N-c) 拍列间相位差），内嵌逐拍探针。
3. **工具检查**：Verilator 5.053 lint、Yosys `proc; check` 锁存器推断检查。
4. **运行方式**（oss-cad-suite 便携工具链）：
   ```bash
   export PATH="/e/tools/oss-cad-suite/bin:/e/tools/oss-cad-suite/lib:$PATH"
   iverilog -g2012 -o sim/review_abft/tb_align_orig.vvp  rtl/abft_checksum.sv rtl/zero_mask.sv tb/tb_abft_zero_mask_align.sv
   vvp -N sim/review_abft/tb_align_orig.vvp        # 原始版 → FAIL
   # 一行修复版（sim/review_abft/abft_checksum_fixed.sv）→ PASS
   ```

---

## 2. 问题清单（按严重度）

### P0-1　`abft_checksum.sv` 斜切补偿多一级寄存器，检测脉冲比污染数据晚一拍（致命）

**位置**：`rtl/abft_checksum.sv:46-65`（`sr [0:DLY]` / `vr [0:DLY]`，输出取 `sr[DLY]` / `vr[DLY]`）

**拍号推演**（spec 拍号，N=4）：

| 信号 | 拍号 |
|---|---|
| 逻辑列 c 数据 `C[m][c]` 到达 `c_flat`（spec §4.2） | `m+N+c+1` |
| 校验列（物理第 N 列）`c_chk` 到达 | `m+2N+1` |
| **设计意图**：列 c 斜切延迟 (N-c) 拍 → 与校验列同拍汇聚于 | `m+2N+1` |
| **实际实现**：`sr[0:DLY]` 为 **DLY+1 级**寄存器（`sr[0]` 捕获 + DLY 级移位）→ 延迟 (N-c+1) 拍 → 汇聚于 | `m+2N+2`（**晚 1 拍**） |
| `sum_r`/`chk_r` 寄存后 `error_pulse` 产生拍 | `m+2N+3` |
| `zero_mask`（DLY=N-c+1）输出行 m 数据的拍 | `m+2N+2` |

**两个后果**（均已仿真实证）：

1. **比较器行错配**：`m+2N+3` 拍比较的是 **ΣC[m]（第 m 行逻辑和）vs N·mean(m+1)（第 m+1 行校验均值）**——相邻两行错配。无故障且相邻行均值相差 > ABFT_THRES/N 时**每行误报**。
2. **掩蔽错拍**：`error_pulse`（m+2N+3）比 zero_mask 输出端该行数据（m+2N+2）**晚一拍**——污染行数据**原样泄漏**，脉冲把**下一干净行**置零。

**仿真证据**（`tb_abft_zero_mask_align.sv`，N=4，内部探针实测）：

```
Phase A（无故障、均值精确、相邻行均值不同）——原始 RTL：
  [probe] t=9  c_chk=1000 | align=1000 1000 1000 1000  ← 第0行数据与第0行均值同拍(链末端对齐正确)
  [probe] t=10 c_chk=1200 | align=1200×4 | sum_r=4000(第0行和) chk_r=1200(第1行均值) vld=1 pulse=1 eabs=800
  → Phase A 汇总: pulses=5 false_pos=5 baddata=20 badvalid=0   （第1~5行全部被误置零）

Phase B（单列污染 row2/col1，增量500）——原始 RTL：
  [C3-LEAK]     t=51 污染行2列1 输出=1500（污染值泄漏！该拍无脉冲）
  [C3-OVERMASK] t=52 干净行3 全列输出=0（被误掩蔽）
  → pulse @ t=52，而 out(FR)=51 —— 脉冲比污染数据晚 1 拍
  >>> REVIEW-TB: FAIL — 对齐契约被违反 (off-by-one 证据) <<<
```

**根因**：`sr [0:DLY]`（闭区间）是 DLY+1 级而非注释声称的 DLY 拍；且与 `zero_mask` 的延时约定不一致——`zero_mask` 中 "DLY 拍 = DLY 级寄存器"（`sr[0:DLY-1]`），`abft` 中 "(N-c) 拍" 却写成了 DLY+1 级。`zero_mask` 的 DLY=N-c+1 与"检测拍 m+2N+2"注释自洽，**本身是正确的**；错在 abft 多出的一级。

**修复**（一行级改动，见 §4）：将 align 移位寄存器改为恰好 DLY 级（`sr[0:DLY-1]`、输出 `sr[DLY-1]`、循环 `i < DLY`）。

**修复后仿真**（同一 TB，仅换修复版 RTL）：

```
Phase A: pulses=0 false_pos=0 baddata=0 badvalid=0
Phase B: pulses=1，pulse @ t=51 == out(FR)，污染行整拍掩蔽，leak=0 overmask=0
>>> REVIEW-TB: PASS — 斜切对齐/同拍掩蔽契约满足 <<<
```

---

### P1-1　"1 级流水线求和树"实为 N 级线性链，关键路径随 N 线性恶化

**位置**：`abft_checksum.sv:84-88`（求和）与 `:109-118`（N·c_chk 还原 + 比较）

- `sum_comb` 是 **N 级串行加法**（for 循环累加），不是树；注释自称"求和树…阻断 N=32 规模下多级加法组合级联"——**一级寄存器只把路径切断一次，不降低每一段的组合深度**，注释与实现不符。
- 更长的一段在寄存器**之后**：`chk_r → [N 级串行加] → 减 → 取绝 → 比较 → error_pulse →（广播至 N 个 zero_mask）→ 输出 mux → out_data`，寄存器到输出约 **N+4 级 20 位运算**。N=4 尚可（需 STA 确认），**N=32（注释自称目标规模）不可接受**。
- `error_pulse` 组合广播到 N 个 zero_mask 实例，组合 net 扇出 N。

**建议**：(a) 求和与还原均改**真·对数深度树**（log₂N 级）；(b) 或将 `chk_scaled` 单独寄存一级，把比较器路径切短；(c) 实现侧对 error_pulse 广播 net 做寄存器复制/缓冲。修改后须补跑本 TB 与系统级回归（流水深度变化会影响端到端延迟约定）。

### P1-2　集成缺失：两模块无实例、无 TB、未入构建流

- `self_healing_array.sv` / `axi_stream_array.sv` 均**未实例化** `abft_checksum` / `zero_mask`；`sim/run_sim.sh` 未编入。
- `self_healing_array_sva.sv` 头部明确要求实例名 **`u_abft`** 与生成块 **`g_mask`**、内部信号 **`abft_enable` / `abft_err` / `abft_abs`**（当前 bind 语句被注释）——集成时必须按约落地，否则 SVA 无法绑定。
- 集成时需同步定义 `abft_enable` 产生逻辑（健康态 & 非 BIST & 非重映射中，见端口注释）。

### P1-3　校验列预算：自愈重映射后 ABFT 失去参考列；对齐延迟应按物理列号计算

- 健康态：N 逻辑列 + 物理第 N 列作校验列（装行均值权重），刚好用尽 N+1 个物理列。
- 一旦 BIST 定位故障列 f<N 并重映射（`phi(c)=c<f?c:c+1`），**备用列被逻辑列 N-1 占用，校验列消失**——自愈稳态下 ABFT 无参考；若故障列恰为校验列（f=N），同样无列可检。
- 且重映射后逻辑列 c 取物理列 `phi(c)`，数据拍变为 `m+N+phi(c)+1`，当前硬编码的对齐延迟 N-c **仅在恒等映射下正确**。

**建议（需主理人拍板的架构决策）**：(a) 自愈后禁用 ABFT、依赖 BIST+重映射（最简，需在规格中写明）；(b) 增加第二备用列（N+2 物理列）使校验列常驻；(c) 若坚持自愈后保留 ABFT：对齐延迟参数化改为 **DLY = N − phi(c)**（按物理列号），并为校验列故障场景设计降级路径。

---

### P2-1　定点安全：SUM_W 足够，但"N·c_chk 还原"并非精确，固定阈值不随 N 缩放

- **SUM_W = 16 + $clog2(N+1) + 1 对本模块全链路是安全的**（2× 余量，推导见 §3-Q2）：Σ、N·c_chk、diff、abs_diff 均无符号溢出。行均值方案确实避免了校验列自身溢出 16 位。
- **但"彻底消除"不成立**：均值权重 `Σw/N` 若由加载侧截断求得（|δ|≤1 LSB/权重），经校验列放大后在 `N·c_chk` 侧引入**系统性偏置 ≈ N² LSB**（与数据无关、给定权重矩阵后恒定），叠加 RNE 舍入噪声 ≈ N LSB。N=4 时 ≈20 LSB ≪ THRES=128（约 6× 余量，安全）；**N=16 时 ≈264 > 128 → 确定性误报**；N=8 时 ≈68，余量偏小。
- `ABFT_THRES=16'h0080` 为固定值，不随 N 缩放；且 `err_abs` 比较为严格大于。

**建议**：阈值参数化为 `ABFT_THRES ≥ N² + N` 量级（或按 N 推导）；均值加载侧改为四舍五入移位并为校验列保留 guard 位；在集成规格中书面固化"均值计算约定"（当前 `self_healing_array.sv:115-125` 的权重重映射**尚未实现**校验列均值装载——备用列只是复制末逻辑列，abft 注释所引用的"权重重映射"尚不存在）。

### P2-2　`zero_mask.sv` 缺少 DLY ≥ 1 参数保护

`sr [0:DLY-1]` 在 DLY=0 时产生非法空 range（各工具报错信息混乱）。建议加 `initial begin if (DLY < 1) $error(...); end`（与 `systolic_array.sv:34` 的参数自检风格一致）。

### P2-3　Verilator lint：`abft_checksum` 3 处 WIDTHEXPAND（语义正确，严格 lint 门会挂）

- `:22` `parameter int ABFT_THRES = 16'h0080`（16 位常量入 32 位 int 参数）——建议直接写 `128` 或 `32'h0080`；
- `:87` `sum_comb + $signed(align_data[k])`（16→20 位符号扩展，**行为正确**，仿真已验证）；
- `:119` `abs_diff > 16'sd32767`（16 位有符号常量与 20 位比较，常量正值、扩展正确）。

工程 Verilator 流程用 `-Wno-fatal` 不受影响；若将来上严格 lint 门需处理。`zero_mask.sv` lint **零告警**。

### P2-4　`abft_checksum.sv:21` 注释失效

"N 为 2 的幂（校验均值按 $clog2(N) 移位）"——本模块不做任何移位，均值由加载侧计算。注释误导，应删改。

### P2-5　头注释与实现不符（除 P1-1 外）

"1 级流水线寄存器锁存 Σc_logic…校验列数据/有效位同拍寄存"——实际 `sum_r`/`chk_r`/`vld_r` 确为同拍寄存（这一点属实），但"求和树""阻断 N=32 关键路径"的表述不成立（见 P1-1）。另：`vld_r` 的 `all_aligned` 防御性检查**无法发现行错配**（valid 与数据被同样多延迟了一级，错配被"一致地"掩盖）——修复 off-by-one 后此点自然消解；更稳健的做法是引入行标签/序号交叉核对。

### P3-1　观测项：饱和列会破坏 ABFT 等价关系

若某列 `C[m][c]` 被 `rne_sat16` 饱和（|值|=32768），则 ΣC 与 N·mean 的等价关系被破坏，必然超限误报。对故障容忍设计而言"报异常"可接受，但建议在交付说明中写明该语义（饱和=可检测异常），避免后续误判为 ABFT 缺陷。

### P3-2　项目级备注（非本两模块缺陷）

两模块用异步复位（`negedge rst_n`），与全工程一致；复位释放同步器应在芯片顶层统一处理（提impl-engineer 在约束/集成阶段确认）。

---

## 3. 对四个审查问题的直接回答

### Q1　斜切补偿与时序对齐：是否存在差一拍的致命漏洞？

**存在，且已实证。根因在 `abft_checksum.sv` 的斜切移位寄存器（P0-1），不在 `zero_mask`。**

- `zero_mask` 的 DLY=N-c+1 与"检测拍 m+2N+2"设计意图**自洽且正确**（修复版仿真证明其精准同拍掩蔽）；
- `abft_checksum` 的 align 链实际延迟 (N-c+1) 拍而非注释声称的 (N-c) 拍（`sr[0:DLY]` 闭区间 = DLY+1 级），导致：① 比较器把第 m 行逻辑和与第 m+1 行均值配对（无故障也误报，实测 5/5 行误报、20 个输出单元被错置零）；② `error_pulse` 比污染数据晚一拍到达 zero_mask 输出端——**污染值泄漏、下一干净行被置零**（实测：污染值 1500 在 out(FR) 拍原样流出，脉冲在 out(FR)+1 拍置零了干净行）。
- 修复（align 链减为恰好 DLY 级）后同一 TB **全约通过**（0 误报、脉冲精准落在污染行输出拍、整拍掩蔽）。

### Q2　Q1.15 溢出防护：SUM_W 是否足够？行均值方案是否彻底消除溢出风险？

**位宽安全；"彻底消除"不成立（精度偏置，非溢出）。**

- SUM_W = 16+clog2(N+1)+1 的余量推导（最坏情况）：|Σ| ≤ N·2¹⁵；|N·c_chk| ≤ N·2¹⁵；|diff| ≤ 2N·2¹⁵；|−diff| 同阶。SUM_W 位有符号范围 ±2^(SUM_W−1) ≈ ±2^(16+clog2(N+1)) ≥ 2·N·2¹⁵·2（N≥1 时 clog2(N+1) ≥ log2N），**全程约 2× 余量，无符号溢出**。N=4：20 位（±524288）vs 最坏 |diff|=262140 ✓；N=8/16/32 同样成立。
- 行均值方案确实消除了**校验列 16 位输出溢出**（均值幅值不超 Q1.15 范围）与**权重求和溢出**。
- 但 `N·c_chk` 还原存在**均值权重量化偏置 ≈ N² LSB**（截断除法时）+ RNE 噪声 ≈ N LSB，属**精度误差而非溢出**：N=4 安全（≈20 ≪ 128），N≥16 会超出固定阈值导致确定性误报（见 P2-1）。

### Q3　可综合性与关键路径：锁存器风险？关键路径？

- **锁存器：无。** Yosys `proc; check` 对两模块均无 latch 告警（仅"memory→寄存器 list"正常提示）；Verilator 无 LATCH/CASEINCOMPLETE/UNOPTFLAT 告警。`zero_mask` 为纯连续赋值 + 完整 always_ff；`abft` 所有 always_comb 分支全赋值（含 '0 初始化累加）。lint 零/三处告警详情见 P2-3。
- **关键路径：有问题（P1-1）。** 所谓"1 级流水线求和树"实为 N 级串行加法；`N·c_chk` 还原链位于寄存器之后并与 error_pulse 组合输出、再经 zero_mask 输出 mux 串联，寄存器→输出约 N+4 级 20 位运算。N=4 需 STA 确认（大概率成为本数据通路关键路径），N=32 不可接受。建议树形化 + 拆分寄存。

### Q4　审查结论与改进建议

**判定：Needs Revision。** `zero_mask.sv`：Pass（仅 P2-2 参数保护建议）。`abft_checksum.sv`：须修复 P0-1 后方可进入集成；P1-1 关键路径改造建议与集成（P1-2/P1-3）同步决策；P2 项随修复轮次一并处理。

---

## 4. 修复补丁（P0-1，一行级改动）

`rtl/abft_checksum.sv` generate 块内（L46-47、L51、L58、L64-65）：

```diff
-            logic signed [15:0] sr [0:DLY];
-            logic               vr [0:DLY];
+            logic signed [15:0] sr [0:DLY-1];   // 恰好 DLY=(N-c) 级, 与 zero_mask 延时约定一致
+            logic               vr [0:DLY-1];
                 always_ff @(posedge clk or negedge rst_n) begin
                     if (!rst_n) begin
-                        for (i = 0; i <= DLY; i++) begin
+                        for (i = 0; i < DLY; i++) begin
                             sr[i] <= '0;
                             vr[i] <= 1'b0;
                         end
                     end else begin
                         sr[0] <= $signed(c_flat[16*c +: 16]);
                         vr[0] <= valid[c];
-                        for (i = 1; i <= DLY; i++) begin
+                        for (i = 1; i < DLY; i++) begin
                             sr[i] <= sr[i-1];
                             vr[i] <= vr[i-1];
                         end
                     end
                 end
-            assign align_data[c]    = sr[DLY];
-            assign align_valid_c[c] = vr[DLY];
+            assign align_data[c]    = sr[DLY-1];
+            assign align_valid_c[c] = vr[DLY-1];
```

已验证版本：`sim/review_abft/abft_checksum_fixed.sv`（由 `sed` 从原文件生成，diff 仅上述 6 行）。回归结果：`tb_abft_zero_mask_align` Phase A/B **全约 PASS**。

**注意**：c=N-1 时 DLY=1 → `sr[0:0]` 单级，合法；N=1 时同理。

---

## 5. 签核记录

| 关口 | 结论 | 条件 / 后续 |
|---|---|---|
| 模块级代码审查（本次） | **FAIL → Needs Revision** | abft_checksum 修复 P0-1 并由 rtl-designer 提交后，重跑 `tb_abft_zero_mask_align`（已就绪）全约通过，方可复审 |
| 集成准入 | **未通过** | P1-2/P1-3 需主理人拍板校验列预算与自愈后 ABFT 策略；集成时按 `self_healing_array_sva.sv` 约定落地 `u_abft`/`g_mask`/`abft_enable`/`abft_err`/`abft_abs` 并解除 bind |
| 时序（STA） | **待验证** | P1-1 改造后由 impl-engineer 出时序报告；N=32 场景须专项评估 |

**遗留跟踪**：P1-3 架构决策（自愈后 ABFT 去留）→ 主理人+用户；P2-1 阈值标定 → rtl-designer + sim-verifier 补参数化回归；`tb_abft_zero_mask_align.sv` 建议并入 `sim/run_sim.sh` 常规回归。

---

*证据文件：`tb/tb_abft_zero_mask_align.sv`（证据 TB）、`sim/review_abft/tb_align_orig.vvp` / `tb_align_fixed.vvp`（可复现仿真）、`sim/review_abft/abft_checksum_fixed.sv`（已验证修复版）。*
