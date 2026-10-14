# `sim/review_p23/` — Phase 2/3 同行复审探针与 Round 2 门控库

本目录由复审人（齐流晶）建立并维护，收录 Round 1 复审的全部实证探针，以及规范化后的
**Round 2 终审门控套件**。Trae 的 F0~F3 补丁（及后续 F6/F7）以此套件为验收基准。

## 一键运行

```bash
bash sim/review_p23/run_round2_gates.sh      # 仓库根或任意目录执行均可
```

- 输出：`sim/review_p23/round2_gates.log`（含每个门控的 PASS/FAIL 与明细）
- 退出码 = FAIL 门数；全部通过时退出 0
- 依赖：Icarus Verilog / Yosys（`E:\tools\oss-cad-suite\bin`）、Python 3.11 + torch

## Round 2 门控清单（16 项）

| 门控 | 对象 | 通过准则 | 对应修复项 |
|---|---|---|---|
| G1a | 仓库版 `tb/tb_transient_sweep.sv` | hist_2==0 ∧ ge3==0 ∧ busy 违例==0 ∧ hist_1>0 ∧ 总数=100 | F0 |
| G1b | 独立参照 `tb_sweep_1beat.sv` | 同 G1a 准则（期望 87/13/0/0） | F0 参照 |
| G1c | `tb_gate_sweep_canfail.sv` | 已知 2 拍注入必须测得 ≥2 脉冲（测量链可失败性） | F0 防假过 |
| G2 | 仓库版 `tb/tb_phase2_closed_loop.sv` | 12/12 PASS ∧ detect==2 ∧ repair∈[50,80] ∧ total=detect+repair | F1/F2 无回归 |
| G3a | `tb_round2_gates.sv` 场景 A | 毒化场景 detect_latency ≤ 10（修复前实测 190） | F1 |
| G3b | `tb_round2_gates.sv` 场景 B | 毒化场景 repair_latency ≤ 80（修复前实测 248） | F1 |
| G4a | `tb_round2_gates.sv` 场景 C | 瞬态簇+持续灌流：无 unhealable、无伪重映射 | F2 |
| G4b | `tb_round2_gates.sv` 场景 D | 永久故障+持续灌流：healed=1 ∧ fault_col=1 | F2 |
| G5 | `tb_rne_check.sv` + `gen_rne_vectors.py` | 2602 向量 RTL==torch==参考模型（0 mismatch） | 回归保障 |
| G6 | Yosys 全栈 | 0 个 `$dlatch` ∧ `check -assert` 退出 0 | F1/F2 综合卫生 |
| G7a | `py_gate_checks.py` | dual_loop 不可愈比例 ≈2×rate ∧ 源码含 `min(1.0,...)` 钳位 | P2-3 |
| G7b | `py_gate_checks.py` | `none` 模式噪声域覆盖 [-32768, +32767] | P2-4 |
| G7c | `py_gate_checks.py` | 瞬态分布含 ge3>0 时战役脚本必须报错或告警申示 | F6（新发现） |
| G7d | `py_gate_checks.py` | `train_cifar10_q115.py --mode float --quick` 退出 0 | F7（新发现） |
| G8 | `py_gate_checks.py` | 战役 `--synthetic` 冒烟 exit 0 ∧ CSV 8 行 | 回归保障 |
| G9 | `py_gate_checks.py` | QAT 浮点卷积 vs 硬件精确路径码值差 ≤ 0.5%（论文须申明） | F8（新发现） |

## 文件清单

| 文件 | 用途 |
|---|---|
| `run_round2_gates.sh` | Round 2 门控主运行器（16 门控 + 汇总 + 退出码） |
| `tb_round2_gates.sv` | G3a/G3b/G4a/G4b 四场景门控 TB（MTTR 毒化 + BIST 灌流韧性） |
| `tb_gate_sweep_canfail.sv` | G1c 测量链可失败性（2 拍注入必须测得 ≥2 脉冲） |
| `tb_sweep_1beat.sv` | G1b 独立参照：真单拍注入扫查（posedge 当拍 release） |
| `tb_rne_check.sv` / `gen_rne_vectors.py` | G5 Q1.15 算子三方逐位比对（含半点/饱和轨/边界角点） |
| `py_gate_checks.py` | G7/G8/G9 Python 侧门控 |
| `py_numerics_gap.py` | G9 数值差测量（QAT 浮点卷积 vs 硬件精确整数路径） |
| `tb_force_probe2.sv` | Round 1 证据：Icarus force/release 双拍语义探针（+V=1/2/3） |
| `tb_mttr_poison2.sv` | Round 1/2 证据：MTTR 毒化三场景原始 TB（+SCEN=1/2/3） |
| `review_evidence.log` | Round 1 复审证据汇总（探针/对照实验/毒化/回归） |
| `round2_gates.log` | Round 2 门控最近一次完整运行日志 |
| `transient_distribution_1beat.csv` | 真单拍参照分布（87/13/0/0） |
| `rne_in.hex` / `rne_exp_ref.hex` / `rne_out_rtl.hex` | G5 向量与双方输出 |
| `yosys_latch_check.log` | Yosys 锁存器检查日志 |

## 设计原则（sim-verifier 铁律）

1. **测试必须能失败**：G1c 以确定性 2 拍注入反向校验测量链；若注入机制被改坏，
   G1a 的 "hist_2==0" 会假通过，G1c 兜底。
2. **门控判据与修复项一一对应**：每个 F 编号有且仅有对应门控，修复不得只过部分门控。
3. **不复述转述**：全部结论由本目录脚本亲自跑出，日志留痕可复跑。
