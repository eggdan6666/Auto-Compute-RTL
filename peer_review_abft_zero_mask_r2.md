# Round 2 Peer Review 报告：`abft_checksum.sv` × `zero_mask.sv` 整改复审与签核判定

| 项 | 内容 |
|---|---|
| 复审对象 | `rtl/abft_checksum.sv`（P0 修复 + 饱和豁免）、`rtl/self_healing_array.sv`（ABFT + 零掩蔽集成、冗余列时分复用） |
| 复审背景 | Round 1 判定 Needs Revision（P0 off-by-one + P1×3 + P2×5）。下游（TRAE）已完成 P0 修复、内循环集成、仿真实测 |
| 复审人 | 齐流晶（FPGA 交付总监）；技术细节按 rtl-designer 视角执行 |
| 复审日期 | 2026-10-14（Round 2） |
| 复审方法 | 代码走查 + Icarus/Verilator/Yosys/sky130 工具实测复验（**不复述转述结论，全部亲自复跑**） |

---

## 0. 签核结论

# ✅ 有条件通过（Conditional Pass）

| 范围 | 结论 |
|---|---|
| P0 差一拍漏洞闭环 | **确认闭环**（代码级 + 模块级 + 系统级 + 对照实验，四重证据） |
| 集成逻辑（时序/综合死锁隐患） | **无新增隐患**：无锁存器/多驱动/组合环，无握手机制故无死锁路径，使能门控从机制上消除 R1-P1-3 失配风险 |
| 遗留条件 | C1 流程（已代修复待复跑确认）、C2 时序 STA 签核（必须）、C3~C6 跟踪项 |

**进入 Phase 3（约束与综合）的前置条件：C1 复跑通过 + C2 STA 签核。** RTL 功能正确性本身不再构成阻塞。

---

## 1. P0 闭环确认（四重证据）

### 1.1 代码级：修复正确且完整

`rtl/abft_checksum.sv:47-70`：`sr [0:DLY-1]` / `vr [0:DLY-1]`、循环 `i < DLY`、输出 `sr[DLY-1]` / `vr[DLY-1]`——align 链恰为 **DLY=(N-c) 级**，与 `zero_mask` 的"DLY 拍 = DLY 级"约定一致，与 R1 §4 补丁逐行一致。边界情形 c=N-1（DLY=1 → `sr[0:0]` 单级）与 N=1 均合法。

### 1.2 模块级：证据 TB 对整改后 RTL 复跑 PASS（本人执行）

```
$ vvp -N sim/review_abft/tb_align_r2.vvp        # 编译对象 = 现行 rtl/abft_checksum.sv
--- Phase A summary: pulses=0 false_pos=0 baddata=0 badvalid=0 (期望全 0) ---
--- Phase B summary: pulses=1 leak=0 overmask=0 badvalid=0 ---
    pulse @ t=51 (out(FR)=51, out(FR+1)=52)      ← 脉冲精准落在污染行输出拍
>>> REVIEW-TB: PASS — 斜切对齐/同拍掩蔽契约满足 <<<
```

### 1.3 系统级：主回归 71/71 PASS（本人复现，补齐编译清单后）

```
$ iverilog ... rtl/abft_checksum.sv rtl/zero_mask.sv tb/tb_self_healing.sv
Total Checks: 71 / Passed: 71 / Failed: 0   >>> ALL TESTS PASSED <<<
```
健康期任务 32 项黄金比对全过（**无一行被 ABFT 误掩蔽**——若 P0 未闭环，健康期将整行误报置零）；自愈后任务 32 项与无故障时逐拍一致。

### 1.4 对照实验：新增 `sat_r` 饱和豁免确为承重修复

- 健康期任务数据实际包含 **6 个饱和轨单元**（`7fff`/`8000`，如 `healthy col0 row1 Out=7fff`）；
- 将 `error_pulse = vld_r && !sat_r && ...` 的 `!sat_r` 移除后重跑主回归：**健康期第 1/3/4/5 行整行被误置零**（`Expected=7fff Got=0000` …），大批 FAIL。

结论：R1-P3-1（饱和破坏 ABFT 线性等价关系导致确定性误报）在本任务数据集上真实存在，TRAE 的 `sat_comb`/`sat_r` 门控是必要且正确的修复；`sat_r` 与 `sum_r`/`chk_r` 同拍寄存，行对齐正确。

---

## 2. 集成逻辑审查：无新增时序/综合死锁隐患

### 2.1 时分复用与使能门控（正确，且从机制上闭环 R1-P1-3）

- **权重复用**（`:128-153`）：健康态（`fault_col_r == NO_FAULT`）物理第 N 列装载行均值校验权重 `chk_w[r] = (Σ_c W[r][c]) >>> $clog2(N)`；故障态切回热替换列映射。与 R1 建议的架构决策一致。
- **`abft_enable = (fault_col_r == NO_FAULT) && !busy`**（`:173`）：ABFT 仅在健康且非 BIST/自愈流程期使能。这从根本上消除了 R1-P1-3 的两个隐患：① 自愈后备用列被逻辑列占用、校验列消失 → 直接禁用 ABFT；② 重映射后逻辑列物理相位变化导致 align 失配 → 恒等映射下才使能，物理列号恒等于逻辑列号。
- `chk_valid`/`c_chk` 直接取物理第 N 列（`:181-182`），逻辑列取重映射后的 `raw_*`（`:179-180`）——健康期即恒等映射，与 align 的 (N-c) 拍延迟精确匹配（已被 1.2/1.3 仿真证明）。
- **BIST 互斥未破坏**：`bist_active` 仍独占阵列输入；BIST 期间 `abft_enable=0`，`vld_r=0`，不可能误报；零掩蔽纯前馈，BIST 结果拍不受掩蔽影响（AXI 侧原有 `!sh_busy` 门控依旧成立）。

### 2.2 综合与死锁专项（工具实测）

| 检查 | 工具 | 结果 |
|---|---|---|
| 锁存器推断（全层级 8 文件） | Yosys `proc; check` | **无 latch**；另有 **153 处 `No latch inferred` 明确确认**（含 `error_pulse`、`sum_comb`、`chk_scaled`、`chk_w`、`w_phys`、`raw_*` 全部 always_comb） |
| 多驱动 / 组合环 | Yosys `hierarchy -check; check` | 无 |
| Verilator lint 全层级 | Verilator 5.053 | 7 告警，全部为良性位宽扩展/有意截断（见 §3-C4），无 LATCH/CASEINCOMPLETE/UNOPTFLAT |
| sky130 综合（generic 流程） | Yosys | **完整通过（exit 0）**；`zero_mask` 实例参数实测为 DLY=2/3/4/5（即 N-c+1，c=3..0）✓，`abft_checksum N=4` ✓ |
| 死锁 | 审查 | 零掩蔽与 ABFT 均为无握手指令级前馈；唯一反馈环为原有自愈 FSM（未改动）。**无新死锁路径** |

### 2.3 `chk_w` 均值数学核验

- `row_sum` 宽度 `16+$clog2(N)`：N=4 → 18 位（±131072），最坏 |Σ4 个 Q1.15| = 131068 ✓ 恰好覆盖；
- 算术右移 `$clog2(N)` 后必落 [-32768, 32767]：极值验证（全 -32768 → -32768 ✓；全 +32767 → +32767 ✓），16 位截断**构造上安全**；
- 截断式（非四舍五入）均值 → R1-P2-1 所述 ≈N² LSB 系统偏置，N=4 时 ≈20 LSB ≪ ABFT_THRES=128（约 6× 余量）✓ 当前参数安全；N≥16 需重新标定阈值（跟踪项 C5）。

---

## 3. 遗留项与条件（签核附带）

### C1【P1-流程，已代修复，待复跑确认】一键回归/综合脚本在集成后全部失效

- `sim/run_sim.sh`：两个 Icarus 编译清单与两个 Verilator 清单缺少 `rtl/abft_checksum.sv rtl/zero_mask.sv` → elaboration 直接报 `Unknown module type`（实测确认）；
- `sim/run_sky130_synth.sh`：① 同样缺文件；② `abc -fast -liberty` 中 `-fast` 非本版本 Yosys 合法选项（实测报错），旧 self_healing 综合日志本身亦是失败中断的；
- **我已修复**：5 处编译清单补齐、证据 TB 增补为回归第 `[7/11]` 阶段、abc 语法改为 `abc -liberty`。修复后全回归正在端到端复跑（`sim/review_abft/run_sim_r2_full.log`），通过则本条件关闭。

### C2【P1-时序，必须】R1-P1-1 关键路径未整改，且现已进入模块输出通路

`abft_checksum.sv` 的 `sum_comb`（N 级串行加）与 `chk_scaled`（N 级串行加）仍为线性链；头注释“求和树…阻断 N=32 关键路径”与实现不符（R1-P2-5 依旧）。更关键：`error_pulse` 现经 `g_mask` 的 `mask` 输入组合进入 `out_data` mux，**寄存器→输出 pad 路径 ≈ chk_r → [N 级 20 位加] → 减 → 取绝 → 比较 → 16 位 mux → c_flat**，约 N+4 级 20 位运算。本工具链无 STA（仅 icebox_stat，不适用）→ **必须由 impl-engineer 在商用流程对 `chk_r→error_pulse→zero_mask→c_flat` 路径做 STA 签核**；若 WNS<0，按 R1 建议：求和与还原改对数深度树 + `chk_scaled` 单独寄存 + error_pulse 广播 net 寄存器复制。N=32 场景此路径不可接受，需与参数化目标频率一并评估。

### C3【P2】SVA 断言未接入回归，且两条 bind 语句均不可编译

- `self_healing_array_sva.sv` 不在任何编译清单中 → Z0/Z1/Z2/N1/N2 与 ZM1/ZM2/ZM3 **当前均不执行**；
- `bind self_healing_array ... #(.N(N))`（`:72`，已注释）中 `N` 在文件作用域不可解析；`bind zero_mask ... #(.DLY(DLY))`（`:116`）对 DLY=2/3/4/5 四个实例只能绑定单一 DLY，ZM1 会误报；
- Icarus 也无法解析该文件的 `default clocking`（仅 Verilator 可用）。
- **建议**（交 sim-verifier）：sha bind 去掉参数依赖默认 N=4（或按实例路径 bind），zm 断言改为按实例路径 bind 或移入 `zero_mask.sv` 并以 `translate_off` 包裹；修复后纳入 run_sim.sh 第 [10/11]/[11/11] 阶段。

### C4【P2】lint 卫生（不阻塞，随下一轮清理）

- `self_healing_array.sv:134` `chk_w[r] = row_sum >>> $clog2(N)`：WIDTHTRUNC——构造安全（§2.3）但应显式 `16'(...)` 转换；
- `:146`/`:163` `p < fault_col_r` 等 5→32 位扩展（fault_col_r 为无符号 5 位、比较量非负，行为正确；建议 `int'(fault_col_r)` 显式化）；
- `abft_checksum.sv:22/89/135` 三处 WIDTHEXPAND（R1-P2-3 依旧）；
- `zero_mask.sv` 仍无 `DLY >= 1` 参数保护（R1-P2-2 依旧；当前实例 DLY≥2 不受影响）。

### C5【P3-已接受】`sat_r` 的检测覆盖代价（需书面固化语义）

任务运行期若故障恰好把某列钉在饱和轨（0x7FFF/0x8000），该拍 ABFT 判读被豁免——此类“卡轨”故障在任务期不会被 ABFT 发现，兜底依赖周期性 BIST。建议：① 交付说明中写明该语义；② 后续将 `sat_r` 以 `sat_event` 形式引出作可观测信号（届时可区分“健康饱和”与“故障卡轨”）。

### C6【P3】`chk_w` 静态配置路径

`w_flat`（输入 pad）→ 行均值加法树 → `w_phys` mux → PE `w_reg`：组合逻辑位于加载控制路径（`load_w` 打拍锁存），非周期路径，但实现侧应施加 input delay 约束，避免被误设为关键路径。

---

## 4. Round 1 问题销项看板

| R1 编号 | 问题 | R2 状态 |
|---|---|---|
| P0-1 | abft 斜切延迟 off-by-one | ✅ **已闭环**（四重证据，§1） |
| P1-1 | “求和树”实为线性链/关键路径 | ⚠️ **未整改** → 升级为签核条件 C2（STA + 树形化） |
| P1-2 | 两模块未集成/未入构建流 | ✅ 已集成；构建流缺陷 → C1（已代修复） |
| P1-3 | 自愈后校验列消失/相位失配 | ✅ **已闭环**（时分复用 + 使能门控，§2.1） |
| P2-1 | THRES 固定不随 N 缩放 | 🔵 部分闭环（N=4 安全余量 6×；N≥16 需重标定 → C5 跟踪） |
| P2-2 | zero_mask 无 DLY≥1 保护 | ❌ 未处理 → C4 |
| P2-3 | Verilator WIDTHEXPAND ×3 | ❌ 未处理 → C4 |
| P2-4 | abft 注释失效（$clog2 移位） | ❌ 未处理 → C4 |
| P2-5 | 头注释与实现不符（求和树） | ❌ 未处理 → C4/C2 |
| P3-1 | 饱和破坏 ABFT 等价关系 | ✅ **已闭环**（sat_r 门控，且经对照实验证明承重） |
| P3-2 | 复位释放同步（项目级） | ➖ 维持原判（顶层事项，非本模块） |

---

## 5. 签核记录

| 关口 | 结论 | 条件 / 责任人 |
|---|---|---|
| Round 2 模块级复审（本次） | **有条件通过** | P0 闭环确认；集成无新增隐患 |
| 回归准入 | **通过（附条件）** | C1 全回归复跑结果确认后关闭（复审人代修复脚本） |
| 约束与综合（Phase 3）准入 | **准入，但附硬条件** | C2：impl-engineer 完成 `chk_r→error_pulse→zero_mask→c_flat` 路径 STA 签核；违例则树形化改造后重跑本证据 TB 与主回归 |
| 验证覆盖 | **有条件** | C3：SVA 修复并纳入回归（sim-verifier） |
| 文档/代码卫生 | **跟踪** | C4/C5/C6 随下一迭代清理 |

**遗留问题跟踪**：C2 → impl-engineer（尚收敛）；C3 → sim-verifier（魏波形）；C4 → rtl-designer（葛逻辑）；C5 → rtl-designer + 交付说明；C6 → impl-engineer 约束阶段。

---

*证据文件：`tb/tb_abft_zero_mask_align.sv`（证据 TB，已入回归 [7/11]）、`sim/review_abft/tb_align_r2.vvp` / `tb_self_healing_r2.vvp`（复跑二进制）、`sim/review_abft/abft_nosat.sv`（对照实验件）、`sim/review_abft/synth_r2_generic.log`（综合日志，153 处 no-latch 确认）、`sim/review_abft/run_sim_r2_full.log`（修复后全回归）、`sim/run_sim.sh.bak` / `run_sky130_synth.sh.bak`（脚本修复前备份）。*
