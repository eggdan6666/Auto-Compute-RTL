# LLM / 多智能体辅助芯片设计系统调研报告（2023–2026）

> 调研方式：基于 WebSearch/WebFetch 对 arXiv、IEEE/ACM、GitHub、公司官方博客/新闻稿的多轮中英文交叉检索。所有关键数字均给出原始出处链接；**凡未找到公开资料者，均明确标注"未找到公开资料"**，未做任何推测性编造。检索时间：2026-10-13。

---

## 目录

1. 六大必查系统详述（AutoChip / ChipNeMo / RTLCoder / ChatEDA / MAGE / AssertLLM）
2. 基准测试体系（VerilogEval / RTLLM / CVDP）
3. 补充覆盖系统（VeriGen / OriGen / CraftRTL / CodeV / Spec2RTL-Agent / VeriDebug / VerilogAssist / CircuitGPT / HDLBug）
4. 工业落地进展（NVIDIA / Synopsys / Cadence / Siemens EDA / Google AlphaChip）
5. 技术路线对比总表
6. 2025–2026 最新进展与工业落地现状
7. 技术范式分类归纳
8. 共性局限与已知批评
9. 参考来源汇总

---

## 一、六大必查系统详述

### 1. AutoChip —— HDL 自动生成 + LLM 仿真反馈闭环

| 项目 | 内容 |
|---|---|
| 出处 | 纽约大学（NYU Tandon）：Shailja Thakur, Jason Blocklove, Hammond Pearce, Benjamin Tan, Siddharth Garg, Ramesh Karri |
| 时间/载体 | arXiv:2311.04887（v1: 2023-11-08；v2: 2024-06-04），cs.PL |
| 关键词 | HDL 自动生成、correctness、反馈闭环 |

**核心流程（技术实现链路）**：
1. 输入：设计 prompt（模块的自然语言描述）；
2. LLM 生成初始 Verilog 模块；
3. **反馈闭环**：将 Icarus Verilog 编译器的报错信息 + 仿真调试信息（**期望输出与实际输出的差异**）作为上下文回注给 LLM，驱动其修正代码，实现"无人工干预"的自动调试循环；
4. 评测集为 HDLBits 教学题目（覆盖多种难度与题型类别）。

**反馈解析与回注方式**：以编译错误文本与仿真输出差异（expected vs actual）的文本化描述注入 prompt，依赖 LLM 的多轮交互能力解析。

**PPA 优化**：**不涉及**。AutoChip 的目标仅是"功能正确的 HDL"，无综合器/物理反馈、无 DSE、无 cost model（这是该文与 ChipGPT 等后续工作的主要区别）。

**关键实验数据**：
- 引入编译器工具上下文（Icarus Verilog）后，Verilog 生成的准确率**提升 24.20%**（论文摘要原文："improves the effectiveness, yielding 24.20% more accurate Verilog"）；
- 论文对多个 LLM、多个问题类别做了系统对比（具体数值见论文表格）。

**局限性**：仅面向 HDLBits 式小规模教学模块；无 PPA 维度；反馈仅为文本级编译/仿真信息，无信号级/时序级诊断；未处理大规模真实设计。

**开源**：是。GitHub: https://github.com/shailja-thakur/AutoChip （论文摘要中给出）
**来源**：https://arxiv.org/abs/2311.04887

---

### 2. ChipNeMo（NVIDIA）—— 领域自适应 LLM + RAG 的工业级芯片设计助手

| 项目 | 内容 |
|---|---|
| 出处 | NVIDIA（42 位作者；共同一作 Mingjie Liu, Teodor-Dumitru Ene, Robert Kirby, Chris Cheng, Nathaniel Pinckney, Rongjian Liang；由 NVIDIA 设计自动化研究总监 Haoxing Ren 领衔） |
| 时间/载体 | arXiv:2311.00176（v1: 2023-10-31；最新 v4: 2024-04） |
| 关键词 | 领域自适应（DAPT）、tokenizer、RAG、内部部署 |

**核心流程（四步领域自适应技术链）**：
1. **领域自适应 tokenizer**：在 LLaMA2 通用 tokenizer 基础上，仅在领域数据上训练新 tokenizer、挑选通用词表中缺失且通用文本中罕见的 token（如 RTL 关键词）加入，嵌入用通用 tokenizer 引导初始化；域数据 token 数最多减少 **3.3%** 且不损害下游效果；
2. **领域自适应继续预训练（DAPT）**：以 LLaMA2-7B/13B/70B 为基座，在 NVIDIA 内部芯片设计语料（软件/RTL/验证 testbench/规范文档，领域集约 **23.1B tokens**，混合公开数据后总量约 **24B tokens**）上继续预训练，使用 NeMo 框架；**DAPT 算力仅为从头预训练的约 1.5%**；
3. **模型对齐（SFT / SteerLM）**：通用 chat 指令数据（OASST/FLAN/P3 等）+ **1,430 条专家构建的领域指令**（设计知识 302、EDA 脚本生成 480、bug 摘要 648）；
4. **领域自适应检索 + RAG**：在领域数据上微调检索模型（相比预训练 retriever 命中率提升约 **30%**，约为无监督 e5-small 基线的 2 倍），用于工程助手聊天机器人的"开卷"回答。

**三个工业用例与关键实验数据**：
- 工程助手 chatbot（GPU ASIC/架构工程师）：88 题内部基准（规范/testbench/基础设施三类），文档库约 1,800 篇、约 67,000 个 passage；专家评估 **6.0/7** 分；
- EDA 脚本生成（VLSI 时序分析的 Python 与 Tcl 工具）：简单脚本正确率 **>70%**；**ChipNeMo-70B 在该任务上超过 GPT-4**；在领域指令上 SFT 使脚本生成正确率 **+18%**；SteerLM 相比 SFT 使 chatbot 人工评估分 **+0.62**（7 分制）；
- Bug 摘要与分析（层级摘要）：专家评分 **>5/7**；
- 总体结论：**ChipNeMo-70B 在 chatbot 与 EDA 脚本生成两个用例上超过 GPT-4**，bug 摘要上具有竞争力；v3 摘要称"以相近或更好性能实现最高 **5 倍模型规模缩减**"。

**PPA 优化**：不直接面向 PPA（定位是语言类任务：问答/脚本/bug 摘要），但其 EDA 脚本生成能力间接服务于时序分析等 PPA 相关流程。

**局限性与已知批评**：
- 语料为 NVIDIA 私有数据，**外部无法复现**；
- bug 摘要类任务上领域知识增益相对较小（作者自述）；
- 领域数据参与 SteerLM 训练有时反而降低 chatbot 质量（作者自述）；
- 后续工作（如 ChipAlign 等）指出领域继续预训可能削弱指令遵循能力；
- 模型知识可能过期，需依赖实时检索约束。

**开源情况**：模型权重**未开源**；论文与技术路线公开（CC-BY）。
**工业部署数据**（官方博客，2025-09-04）：ChipNeMo 已演化为"构建于开源 NVIDIA Nemotron 模型之上、用 NVIDIA 内部芯片设计数据训练的**专业智能体团队**"，**服务约 5,000 名工程师（设计/验证/文档），一年内节省 4,000 个工程人天数**，回答准确率 **>85%**，将技术答案检索从数小时缩短到数秒，并加速验证周期（验证通常占开发周期的 30–50%）。
**来源**：
- 论文：https://arxiv.org/abs/2311.00176 （v4 HTML：https://arxiv.org/html/2311.00176v4 ）
- 部署数据：https://blogs.nvidia.com/blog/ways-ai-agents-are-raising-team-performance/

---

### 3. RTLCoder —— 开源 LLM + 代码质量反馈训练，超越 GPT-3.5 的 RTL 生成

| 项目 | 内容 |
|---|---|
| 出处 | 香港科技大学（HKUST）：Shang Liu, Wenji Fang, Yao Lu, Qijun Zhang, Hongce Zhang, Zhiyao Xie |
| 时间/载体 | arXiv:2312.08617（2023-12） |
| 关键词 | 开源数据集、代码质量反馈、轻量级 |

**核心流程（技术实现链路）**：
1. **自动化数据集生成流**（三阶段）：① RTL 领域关键词准备（树状结构多轮 prompting GPT 得到数百个设计关键词）→ ② 指令生成（GPT-3.5 将关键词组合成设计描述指令）→ ③ 参考代码生成（GPT-3.5 生成对应 Verilog）；共生成 **>10,000** 对"指令–参考 RTL"样本；
2. **基于代码质量反馈的训练方案**：在训练中引入代码质量反馈信号，进一步把性能推到超过 GPT-3.5、逼近 GPT-4；
3. 轻量化：**仅 7B 参数**，4 张消费级 GPU 可完成训练；4-bit 量化后仅占 4GB 内存，可在笔记本上本地运行（隐私友好）。

**反馈闭环**：训练层面的"代码质量反馈"（数据筛选/质量信号），**推理时无仿真器闭环**（属于"微调+数据工程"范式，而非 agent 闭环）。

**PPA 优化**：无显式 PPA 优化；评测以语法/功能正确率为主（依托 VerilogEval 与 RTLLM 基准）。

**关键实验数据**：
- 作者称为"首个在设计 RTL 生成上明确超越 GPT-3.5 的非商业方案"，性能与 GPT-4 可比；
- 第三方复现表中（VeriSeek 论文 Table 1，同尺寸 7B 模型对比）：RTLCoder 在 VerilogEval 上 pass@1 **66.9%** / pass@5 **88.4%**；在 RTLLM v1.1 上语法 pass@1 **96.6%**（同期对比：GPT-3.5 53.3/84.6、GPT-4 73.4/89.2、ChipGPT-13B 57.4/68.2）。

**局限性**：数据为 GPT 合成，存在"合成数据脱离真实 RTL 分布"的问题（后续 CodeV、CraftRTL 等对此有专门批评与改进）；无 PPA 评估；单次生成无自我纠错。

**开源**：是，完全开源（数据生成流程、数据集、训练算法、微调模型）。GitHub: https://github.com/hkust-zhiyao/RTLCoder
**来源**：https://arxiv.org/abs/2312.08617 ；复现数据引自 https://ar5iv.arxiv.org/html/2407.18271

---

### 4. ChatEDA —— 自主 LLM Agent，RTL-to-GDSII 全流程敏捷设计

| 项目 | 内容 |
|---|---|
| 出处 | 香港中文大学（CUHK）：Zhuolun He, Haoyuan Wu, Xinyun Zhang, Xufeng Yao, Su Zheng, Haisheng Zheng, Bei Yu |
| 时间/载体 | arXiv:2308.10204（2023-08，后续有 v4） |
| 关键词 | 自主 Agent、EDA 工具编排、RTL-to-GDSII |

**核心流程（三阶段）**：
1. **任务规划（Task Planning）**：控制器模型 AutoMage 将用户的复杂自然语言需求分解为有序子任务（逻辑综合、floorplan、placement 等）；
2. **脚本生成（Script Generation）**：输入"外部工具 API 规范 + 用户需求 + 分解后的子任务"的结构化文本，AutoMage 生成可直接执行的 Python 脚本（调用 OpenROAD 等 EDA 工具 API）；
3. **任务执行（Task Execution）**：ChatEDA 设置环境变量、以子进程执行脚本，由 OpenROAD 等工具实际完成 RTL-to-GDSII 流程，支持架构探索、设计空间评估、早期 QoR 估计。

**控制器训练**：AutoMage 为 **LLaMA2 微调**（选用 Llama2 而非 CodeLlama，因后者代码增量预训练会损失 EDA 通用知识）；用 self-instruct 范式经 GPT 模型构建指令池，**QLoRA** 高效微调。

**反馈闭环**：属于"**LLM 编排 EDA 工具**"范式——LLM 是控制器（CPU），EDA 工具是执行器；闭环体现在"自然语言 → 脚本 → 工具执行 → 结果"的编排层面，而非 RTL 代码级的仿真纠错。

**PPA 优化**：间接支持——通过编排完整 RTL-to-GDSII 流程获得物理实现结果，实现早期 QoR（PPA）估计与设计空间评估；但无显式 cost model 驱动的优化循环。

**关键实验数据**：论文称 ChatEDA/AutoMage 在多任务上**超过 GPT-4 及其他知名 LLM**（详见论文实验章节；未检索到统一的单一 pass@k 头条数字）。

**局限性**：不直接生成 RTL，而是生成调用 EDA 工具的脚本；任务成功率依赖工具 API 文档质量；后续学界出现大量跟进（AutoEDA、IICPilot、EDA Corpus、ORAssistant 等，见第六节）。

**开源**：是。GitHub: https://github.com/wuhy68/ChatEDAv1
**来源**：https://arxiv.org/abs/2308.10204

---

### 5. MAGE —— 多智能体 RTL 生成引擎（高温采样 + 状态检查点调试）

| 项目 | 内容 |
|---|---|
| 出处 | **加州大学圣地亚哥分校（UC San Diego）**：Yujie Zhao, Hejia Zhang, Hanxian Huang, Zhongming Yu, Jishen Zhao |
| 时间/载体 | arXiv:2412.07822（2024-12-10）；**DAC 2025 正式发表**（IEEE Xplore 文献号 11133191） |
| 关键词 | 多智能体、高温采样、Verilog 状态检查点 |

> **出处核实说明**：用户提示"注意确认其真实出处"。经核实，MAGE 的真实出处是 **UCSD（Jishen Zhao 组）**，而非任何公司产品；其 DAC 2025 版本已在 IEEE Xplore 可查（https://ieeexplore.ieee.org/document/11133191 ）。论文自称"**首个开源的**多智能体 RTL 代码生成系统"。

**核心流程（四类智能体 + 递归框架）**：
1. **Testbench 生成智能体**：从规格生成验证环境（含状态检查点）；
2. **RTL 生成智能体**：从规格生成可综合 Verilog；
3. **Judge 智能体**：仿真评估、校验 testbench 质量并打分决策；
4. **Debug 智能体**：分析仿真结果、定位错误并迭代修复。

**两大核心机制**：
- **高温 RTL 候选采样与调试系统**：以高温度（论文示例 T=0.85）采样 n 个（如 20 个）候选，经**仿真评分排序**，挑选 top-K 进入调试阶段——利用高随机性扩展候选空间、提高高质量候选出现概率；
- **Verilog 状态检查点检查（Verilog-state checkpointing）**：与传统方法只反馈"最终输出不匹配"不同，MAGE **逐时钟沿将内部状态值与期望输出比对**，从而**早期发现功能错误并定位第一个不匹配的时钟周期**（含当时输入值、实际/期望输出差异），为 debug 智能体提供精准修复线索。

**PPA 优化**：无（聚焦语法+功能正确性）。

**关键实验数据**：
- 在 **VerilogEval-Human v2** 上达到 **95.7% 语法与功能双正确率**，**超越 Claude-3.5-sonnet 达 23.3 个百分点**；
- 论文引用：单个 Claude-3.5-sonnet agent 在简单设计任务上功能通过率仅 75.0%。

**局限性（含后续文献的批评）**：
- 依赖闭源 LLM API，推理成本高；
- 状态检查点只暴露**输出级信号值**，不暴露实现类、时钟域、guarded-command 逻辑等深层语义（2026 年综述性论文的批评）；
- 无 PPA 维度；未在复杂大规模设计上验证。

**开源**：是。GitHub: https://github.com/stable-lab/MAGE-A-Multi-Agent-Engine-for-Automated-RTL-Code-Generation
**来源**：https://arxiv.org/abs/2412.07822 ；https://ieeexplore.ieee.org/document/11133191

---

### 6. AssertLLM —— 从完整设计规范自动生成 SVA 断言的 Multi-LLM 框架

| 项目 | 内容 |
|---|---|
| 出处 | 香港科技大学（广州）：Wenji Fang, Mengming Li, Min Li, Zhiyuan Yan, Shang Liu, Hongce Zhang, Zhiyao Xie |
| 时间/载体 | arXiv:2402.00386（v4 最新）；**ASP-DAC 2025 正式发表**（pp. 614–621） |
| 关键词 | SVA、ABV、多 LLM 分工、RAG |

**核心流程（三阶段 Multi-LLM）**：
1. **自然语言分析器（LLM①）**：处理完整 PDF 规范文件（含文本、表格），按统一模板抽取信号的定义（位宽/类型）、功能、互连关系、相关信号等结构化信息；
2. **波形分析器（LLM②）**：解析规范中的**时序波形图**（先生成模板描述观测行为，再从图中抽取行为描述）——这是相对前人工作的关键增量（此前方法忽略波形图）；
3. **SVA 生成器（LLM③）**：基于前两阶段结构化信息 + 设计架构图，用 **RAG**（SVA/形式化验证教程与教科书知识库）生成 SystemVerilog Assertions；断言分为 **width / connectivity / function** 三类。

**反馈闭环**：通过 **Cadence JasperGold FPV（形式化属性验证）** 对生成 SVA 做功能校验（语法检查 + FPV）；失败案例用于分析，但系统本身**未实现基于 FPV 反例的自动自愈闭环**（作者列为 future work）。

**PPA 优化**：不涉及（属验证环节，衡量指标为断言正确率与 COI 覆盖率）。

**关键实验数据**：
- 在含 **23 个 I/O 信号**的 I2C 完整设计上：**89% 的生成断言语法与功能均正确**（论文摘要头条数字）；
- 细粒度结果：I2C 生成 65 条 property，86% 正确；多设计平均 **88%** 通过语法+FPV；**COI（影响锥）覆盖率平均 97%**；ECG 与 Pairing 设计上正确率 91–93%、覆盖率 97–100%；
- **基线对比：GPT-4o 直接生成仅 36% 正确率、11% 覆盖率；GPT-3.5 无法直接处理多模态规范**。

**局限性与已知批评**：
- 高度依赖输入规范文档的质量与详细程度（规格差 → 断言少且差）；
- 从波形推断未明确画出的隐式行为能力有限，导致部分断言不完整；
- 仅在接口级/文档化架构层面有效，深层内部逻辑断言是 future work；
- 无自动纠错闭环。

**开源**：**未找到公开代码仓库**（论文公开，代码未检索到）。
**来源**：https://arxiv.org/abs/2402.00386 ；ASP-DAC 2025 论文页：https://www.aspdac.com/aspdac2025/archive/pdf/4D-1.pdf

---

## 二、基准测试体系

### 1. VerilogEval（NVIDIA）—— 领域内引用最广的 RTL 生成基准

- **出处/载体**：NVIDIA（Mingjie Liu, Nathaniel Pinckney, Brucek Khailany, Haoxing Ren），arXiv:2309.07544，**ICCAD 2023**；GitHub: https://github.com/NVlabs/verilog-eval
- **构成**：**156 道 HDLBits 题目**，从简单组合逻辑到复杂 FSM；以瞬态仿真输出与黄金解比对自动判定功能正确性；提出用 LLM 合成的"问题–代码"对做 SFT 的引导方法。
- **关键数据**：v2 修正结果中，**GPT-4 在 spec-to-RTL 任务上 pass@1 43.5%（10 次内 58.9%）；GPT-3.5 为 26.7%**。
- **一年后复测**（Revisiting VerilogEval，arXiv:2408.11053，TODAES 2025）：**GPT-4o 达 63%**；开源 **Llama-3.1-405B 达 58%**（几乎追平 GPT-4o）；领域专用小模型 RTL-Coder-6.7B 达 **34%**；并指出 prompt engineering 对通过率影响巨大、需要失败自动分类基础设施。
- **现状**：VerilogEval v2 已趋于**饱和**（多模型高分），2025 年起 NVIDIA 主推 CVDP 作为下一代基准。

### 2. RTLLM（HKUST）—— 首个把"设计质量（PPA）"纳入目标的 RTL 生成基准

- **出处/载体**：HKUST（Yao Lu, Shang Liu, Qijun Zhang, Zhiyao Xie），arXiv:2308.05345，**ASP-DAC 2024**；GitHub: https://github.com/hkust-zhiyao/rtllm
- **构成**：30 个设计（v1.1 为 29 个较大规模设计），规模显著大于此前自建小样例；提出**三级渐进目标：syntax goal → functionality goal → design quality goal**（后者即 PPA 定量评估）；提出 **self-planning** 提示技术，显著提升 GPT-3.5 表现。
- **意义**：是早期少数在"正确性之外"明确衡量生成 RTL **设计质量**的基准，后续 CraftRTL/CodeV 等均以 RTLLM 为三大评测基准之一。

### 3. CVDP（NVIDIA，2025）—— 下一代"LLM+智能体"基准

- **出处/载体**：NVIDIA（Nathaniel Pinckney, Chenhui Deng, Chia-Tung Ho, Yun-Da Tsai, Mingjie Liu, Wenfei Zhou, Brucek Khailany, Haoxing Ren），arXiv:2506.14074（2025-06-17）；GitHub: https://github.com/NVlabs/cvdp_benchmark
- **构成**：**783 道由资深硬件工程师撰写的问题、13 类任务**（RTL 生成/修改/复用/改进、testbench 激励与检查器、断言生成、调试修复、规范对齐、RTL/TB 问答等），同时提供 **non-agentic 与 agentic（多轮、工具调用）两种格式**；基于 Docker 提供开源 EDA 工具访问。
- **关键数据**：**SOTA 模型在代码生成上 pass@1 不超过 34%**（对比 VerilogEval/RTLLM 上 63%/94% 的高分已饱和）；agentic 任务（尤其 RTL 复用与验证）尤其困难。
- **生态**：Si2（Silicon Integration Initiative）**LLM Benchmarking Coalition** 基于 CVDP 推出开源公共排行榜 **LBC-bench**（https://si2.org/?p=1467/ ）。

---

## 三、补充覆盖系统

### 3.1 VeriGen —— 首个 Verilog 专用微调模型（NYU）

- **出处/载体**：NYU + UNSW + Calgary（Shailja Thakur, Baleegh Ahmad, Hammond Pearce, Benjamin Tan, Brendan Dolan-Gavitt, Ramesh Karri, Siddharth Garg），arXiv:2308.00708，**DATE 2023**；期刊版获 **ACM TODAES 2024 最佳论文奖**。
- **技术链路**：从 280 万+ GitHub 仓库筛选约 **5 万个 Verilog 文件（约 300MB）**，辅以 **70 本 Verilog 教科书**（合计约 400MB 语料）；微调 **CodeGen-16B**（3×A100、约 250GB GPU 显存、DeepSpeed/ZeRO）。
- **关键数据**：微调版 CodeGen-16B **超过 GPT-3.5-turbo 1.1%**；语法正确率较预训练基座**提升 41%**；在更难问题集上功能正确率 **41.9% vs code-davinci-002 的 35.4%**；微调使复杂问题集准确率从 **1.09% 提升至 27%**。
- **开源**：训练/评测脚本与模型 checkpoints 开源。
- **来源**：https://arxiv.org/pdf/2308.00708 ；https://techxplore.com/news/2025-06-ai-specialized-chip-language.pdf

### 3.2 OriGen —— 代码到代码增强 + 编译器反馈自反思（PKU，ICCAD 2024）

- **出处/载体**：北京大学等（Fan Cui, Chenyang Yin, Kexing Zhou, Youwei Xiao, …, Qipeng Guo, Yun Liang, Xingcheng Zhang, Demin Song, Dahua Lin），arXiv:2407.16237，**ICCAD 2024**（DOI: 10.1145/3676536.3676830）。
- **技术链路**：基座 **DeepSeek-Coder-7B + LoRA**；**code-to-code augmentation**（以商业 LLM 为 teacher 精炼开源 RTL 数据）；**self-reflection 机制**：生成代码未通过编译时，将错误代码 + 编译器错误信息回注模型修复（配套构建 **VerilogFixEval** 基准：源自 VerilogEval 的 **221 个编译失败案例**；错误–修复数据集由 Claude3-Haiku 等生成）。
- **关键数据**：超过此前最佳开源 LLM **12.8%**；在 **VerilogEval-Human 上 pass@1 超过 GPT-4 Turbo**；在自反思基准上**超过 GPT-4 19.9%**。
- **开源**：GitHub https://github.com/pku-liang/OriGen ；HuggingFace henryen/OriGen。
- **来源**：https://dlnext.acm.org/doi/10.1145/3676536.3676830

### 3.3 CraftRTL —— correct-by-construction 合成数据 + 定向代码修复（NVIDIA）

- **出处/载体**：NVIDIA（Mingjie Liu, Yun-Da Tsai, Wenfei Zhou, Haoxing Ren），arXiv:2409.12993（v2: 2025-02）。
- **技术链路**：先分析指出此前合成数据两大缺陷——① 难以处理**非文本表示**（卡诺图、状态转移图、波形图）；② 训练中模型随机犯"小错"导致方差大。对策：① 针对非文本表示的 **correct-by-construction 数据**（KMap 12.5k + FSM 8.0k + Waveform 8.0k = 28.5k，SDG 总量 80.1k）；② 自动**错误报告生成框架**：从多个模型 checkpoint 收集错误、把错误**注入开源代码**以构造定向 code-repair 数据；微调 **Starcoder2-15B**。
- **关键数据**：pass@1 较此前 SOTA **+3.8%（VerilogEval-Machine）/ +10.9%（Human）/ +6.6%（RTLLM）**；Starcoder2-SDG 73.7% vs GPT-4o 63.7%（VerilogEval-Machine，T=0.8）。
- **开源**：GitHub https://github.com/NVlabs/CraftRTL （NVIDIA Source Code License，非标准 OSI 许可证）。
- **来源**：https://arxiv.org/abs/2409.12993

### 3.4 CodeV —— 多级摘要数据合成 + Chat-FIM-Tag 微调（中科院计算所，TCAD 2025）

- **出处/载体**：中科院计算所（ICT-CAS）+ 寒武纪等（Yang Zhao, Di Huang, Chongxiao Li, …, Qi Guo, Xing Hu），arXiv:2407.10424；**IEEE TCAD 2025**（45(4): 1893-1906）。
- **技术链路**：核心观察"**LLM 擅长总结代码而非生成代码**、真实代码质量高于 LLM 合成代码"；据此提出**反向指令生成**：爬取 150 万+ Verilog 模块，过滤得 **16.5 万高质量 Verilog 模块 + 1.87 万 Chisel 模块**；用 GPT-3.5 做**多级摘要**（先细粒度功能描述、再高层规格）构造约 **18.5 万 description-code 对**；提出 **Chat-FIM-Tag SFT**（同一模型支持 Chat 与 fill-in-the-middle、Verilog 与 Chisel、显式语言标签）。
- **关键数据**：CodeV-Verilog-QC **VerilogEval-Machine pass@1 80.1% / Human 59.2%**（超过 GPT-4 与 BetterV）；RTLLM v1.1 语法 93.1–96.6%、功能 51.7–62.1%（不同版本报告略有差异）；在 VerilogEval-Machine 上超 GPT-3.5 **35.2%**、超此前开源 SOTA（BetterV）**14.4%**。
- **开源**：2025-09 起模型与数据集在 HuggingFace 全面开源（Di Huang 主页：https://huangdi95.github.io/ ）。
- **后续**：**QiMeng-CodeV-R1**（NeurIPS 2025）：规则化 testbench 生成器 + round-trip 数据合成 + adaptive DAPO，"先蒸馏后 RL"，VerilogEval v2 / RTLLM v2 pass@1 **68.6% / 72.9%**，超此前 SOTA 12–21%，匹配或超越 671B 的 DeepSeek-R1（arXiv:2506.05007、arXiv:2505.24183）。
- **来源**：https://arxiv.org/abs/2407.10424

### 3.5 Spec2RTL-Agent —— 从复杂规范文档直接生成 RTL 的多智能体系统（NVIDIA，2025）

- **出处/载体**：NVIDIA（Zhongzhi Yu, Mingjie Liu, Michael Zimmer, Yingyan Celine Lin, Yong Liu, Haoxing Ren），arXiv:2506.13905（2025-06）。
- **技术链路**：直接处理**非结构化规范文档**（NIST 发布的规范，如 AES/SHA 类）；三阶段：**Understanding**（抽取约束、形成结构化实施计划）→ **Coding**（先生成结构化高层代码，再转为可综合 C++ 交给 HLS，而非直接从自然语言生成 RTL）→ **Reflection**（自适应反思模块，追踪错误源头、迭代精化中间输出）。
- **关键数据**：在 3 份 NIST 规范上，**人工干预较现有方法最多减少 75%**；自称"首个从非结构化规范生成 RTL 的完全自动化多智能体系统"。
- **局限**：受 HLS 语义约束，许多实用 RTL 模式无法表达（后续综述批评）；依赖 HLS 工具链。
- **来源**：https://arxiv.org/abs/2506.13905

### 3.6 VeriDebug —— 对比嵌入 + 引导纠正的统一 Verilog 调试 LLM（2025）

- **出处/载体**：Ning Wang, Bingkun Yao, Jie Zhou, Yuchen Hu, Xi Wang, Nan Guan, Zhe Jiang，arXiv:2504.19099（2025-04-27）。
- **技术链路**：单一模型统一"bug 检测 + 修复"：输入 = 规格 + 错误程序；用**对比嵌入**学习错误模式（bug 位置与类型检索，正负样本对比），再经相似度检索内部信息（候选错误行/类型）驱动**生成式修复**；训练中混合 ground truth 与随机采样的候选以模拟真实场景。
- **关键数据**：VeriDebug(Loc,Type) 修复准确率 **Acc1 = 64.7**，对比开源 SOTA 11.3、**GPT-3.5-turbo 36.6**；发布 **8,000 条 Verilog 片段（错误+修正）开源数据集**。
- **局限**：未讨论极大设计上的可扩展性；未按 bug 类型细分性能。
- **来源**：https://arxiv.org/abs/2504.19099

### 3.7 VerilogAssist / VeriAssist —— 自我验证 + 自我纠正的 RTL 助手（UCSD + Berkeley + Intel）

- **出处/载体**：UC San Diego + UC Berkeley + Intel Applied ML（Hanxian Huang, Zhenghan Lin, Zixuan Wang, Xin Chen, Ke Ding, Jishen Zhao），arXiv:2406.00115（2024-05-31）。
- **技术链路**：自动 prompt 系统 + **RTL 仿真器入环**：① 生成初始 RTL + testbench；② **self-verification**：带测试用例逐时间步推演代码行为；③ **self-correction**：读取编译与仿真结果修复语法/功能错误；迭代至时间预算用完；充分利用多轮交互与 CoT。
- **关键数据**：GPT-4 + VeriAssist 在 RTLLM 上**语法通过率 100%、功能通过率 75.9%（+10.4% vs 原生 GPT-4）**；在 Intel Agilex-7 FPGA 上资源利用最多改善 **45%**。
- **局限（作者自述）**：尚无综合阶段反馈，不能针对功耗/面积/Fmax 优化；若 LLM 生成的 testbench 本身错误，可能把 RTL"自我纠正"到功能错误但能通过错误 testbench 的状态（"自评自判"问题）。
- **来源**：https://arxiv.org/abs/2406.00115

### 3.8 CircuitGPT —— 未找到公开资料（重要排除性结论）

- **结论：未找到芯片设计/HDL 领域名为 "CircuitGPT" 的公开论文或系统。** 多轮中英文检索（含 arXiv、Google 学术索引、GitHub）仅命中：
  1. **ChipGPT**（arXiv:2305.14019，中科院计算所 Kaiyan Chang 等）——四阶段零代码框架：prompt 生成 → ChatGPT 生成 Verilog → output manager 纠正优化 → 设计空间搜索选优；**这是最容易被误称为 "CircuitGPT" 的真实系统**；
  2. 若干同名但无关项目：量子电路生成（如 arXiv:2504.11109 等）、变电站设备维护助手（GitHub: Makaan-Mates/CircuitGPT）。
- 建议报告中如需引用，请改用 **ChipGPT（arXiv:2305.14019）**。

### 3.9 HDLBug —— 未找到公开资料（重要排除性结论）

- **结论：未找到名为 "HDLBug" 的公开数据集/论文。** 最接近的真实工作为：
  - **HDLdebugger**（ACM TODAES 2025，DOI: 10.1145/3735638）：逆向工程流水线把行业 HDL 文档中的错误模式编码为"修改函数"、注入正确代码生成 **92,143 条训练实例**，CodeLlama-13b + RAG 搜索引擎；
  - **From Bugs to Fixes**（IEEE LAD 2024，DFKI/Bremen，RAG+LLM，OpenTitan，除常量类 bug 外均可修复）；
  - **LiK**（arXiv:2409.15186）：功能 bug 定位，RTLLM 上 pass@1 **93.3%**（超 GPT-4 77.9%、Claude-3.5 90.8%），并使 GPT-3.5 修复率从 40.39% → 58.92%；
  - **LLM4SecHW / OSHD 数据集**（Kansas State 等，AsianHOST 2023，arXiv:2401.16448）。
- 建议报告中以 "HDLdebugger / From Bugs to Fixes / LiK" 替代 "HDLBug" 表述。

---

## 四、工业落地进展

### 4.1 NVIDIA ChipNeMo —— 从论文到内部大规模部署

- 2023-10 论文发布（arXiv:2311.00176）；
- 2024 年 IA Summit 等场合确认内部持续部署（unit test verification、工程师生产力）；
- **2025-09-04 官方博客披露部署数据**：基于开源 Nemotron 模型 + 内部芯片设计数据训练的"专业智能体团队"；**5,000 名工程师**（设计/验证/文档）使用；**一年节省 4,000 工程人天数**；回答准确率 **>85%**；技术答案检索从数小时→数秒；加速验证周期（验证占典型开发周期 30–50%）。
- 定位：内部生产力工具，未作为商品对外销售。
- 来源：https://blogs.nvidia.com/blog/ways-ai-agents-are-raising-team-performance/

### 4.2 Synopsys.ai Copilot —— 商用 GenAI EDA 助手矩阵

- **2023-11** 发布（与微软合作，集成 Azure OpenAI 服务）：业界首个贯穿设计栈的 GenAI EDA 技术；
- **2025-09 官方数据**（五个商用 Copilot 应用 + 早期客户反馈）：
  - **Knowledge Assistant**（商业+开源 LLM + 专有 RAG 流水线）：答疑时间缩短最高 **70%**；新开发者上手时间缩短 **30%**；
  - **Workflow Assistant**（PrimeTime / Fusion Compiler，零代码脚本生成与优化）：脚本方案生成效率平均 **2 倍**；配 PrimeTime 时**快 10–20 倍**；
  - **Code Advisor**（自然语言→RTL，集成 Euclide 实时 lint 与正确性检查）：初期客户生产力提升最高 **30%**（富士通：RTL 代码生成生产力 **+10–30%**，高效生成 SVA、wrapper 与参数化模块）；
  - **Formal Advisor**（VC Formal 内，从规范生成形式化 testbench + helper property 迭代）：初期客户生产力 **4–5 倍**；
  - **Lint Advisor**（VC SpyGlass + Verdi，自然语言 lint 规则与自动修复）；
  - 整体：数千名用户在头部半导体公司使用，初期反馈 **2–5 倍**芯片设计生产力提升；官方另称信息检索减少 40%、time-to-solution 降低 10–20 倍（产品页口径）。
- **智能体化下一步**：**AgentEngineer** 技术（专门构建并训练的多智能体系统，自主执行工程工作流）；Synopsys Converge 2026 展示"业界首个 L4 级编排、多智能体、自适应学习的芯片设计工作流"。
- 来源：https://synopsys.com/blogs/chip-design/synopsys-ai-copilots-chip-design.html ；https://synopsys.com/ai/generative-ai.html ；中文稿：https://cn.news.synopsys.com/2025-09-11-EDA

### 4.3 Cadence —— Cerebrus / JedAI / Verisium → ChipStack AI Super Agent

- **Cerebrus**（Intelligent Chip Explorer）：基于 RL/ML 的实现工具，自动探索设计空间优化 PPA；官方称相关 AI 优化与 AI assistant 方案**已用于 1,000+ 次流片**；
- **JedAI 平台**（2022-09 发布）：统一大数据分析基础设施，打通 Verisium（验证）、Cerebrus（实现）、Optimality（系统优化）及第三方数据，管理设计/工作负载/工作流三类数据，提供 Python/Jupyter/REST 自定义分析；
- **Verisium AI-Driven Verification Platform**：AutoTriage（回归失败自动分诊）、SemanticDiff（代码修订语义比较）、WaveMiner（波形根因分析）、PinDown（预测引入失败的提交）、Debug、Manager（验证管理 + AI 测试套件优化）；
- **ChipStack AI Super Agent（2026-02 发布）**："业界首个前端设计与验证智能体工作流"：编排多个虚拟工程师（RTL 生成、testbench 创建、回归编排、调试），基于**共享 Mental Model（设计意图/层级/规范/历史上下文的知识库）**，支持云上与本地前沿模型（含开源 NVIDIA Nemotron，可经 NeMo 定制）；官方称设计与 testbench 编码、测试计划、回归编排、调试与自动修复**生产力最高 10 倍**；**早期部署客户：Altera、NVIDIA、Qualcomm、Tenstorrent**；Altera 称部分领域验证工作量降低约 **10 倍**；Tenstorrent 三个月三关键设计块评估中验证时间最多缩短 **4 倍**；
- **2026-06-01（Computex 2026）**：ChipStack 扩展至 **Level-5 自主性**的"完全自主虚拟芯片设计工程师"（NVIDIA Nemotron 模型 + NVIDIA OpenShell 安全运行时）；NVIDIA 内部实践：每位工程师借助 ChipStack 智能体自动执行数百项 Xcelium 仿真 + Jasper 形式化验证，**RTL 验证循环加速 40 倍以上**（原约 5 周的验证循环缩短到 1 天以内）；
- **2026-09 官方稿**：新增 **RTL Generation Agent**（spec-to-RTL 生成 + 既有 RTL 升级），早期评估中相较纯基础模型代码生成实现**平均面积减少 24%、功耗减少 18%，且 100% 功能正确**；本田（Honda R&D）在汽车 SoC 上验证；2026 Q4 面向早期客户开放；同时发布更广域的 **AgentStack**（扩展至物理设计、定制/模拟设计与迁移、系统级设计；NVIDIA 为早期合作伙伴）。
- 来源：https://cadence.com/en_US/home/company/newsroom/press-releases/pr/2026/cadence-unleashes-chipstack-ai-super-agent-pioneering-a-new.html.html ；https://cadence.com/en_US/home/company/newsroom/press-releases/pr/2026/cadence-expands-chipstack-ai-super-agent-with-a-new-agent-for.html ；https://www.businesswire.com/news/home/20220913005383/en/ ；https://www.eenewseurope.com/en/soc-design-verification-platform-leverages-big-data-ai/

### 4.4 Siemens EDA —— EDA AI System（DAC 2025）

- **2025-07-18（DAC 2025）**发布 **Siemens EDA AI system**：横跨 Questra、Tessent、Xpedition、Veloce、Catapult、Calibre、Aprisa、Solido 全半导体与 PCB 设计套件，融合 ML、RL、生成式 AI 与智能体 AI；**开放、可定制、支持多 LLM、支持本地微调**（数据仅用户可控）；
- 三大支柱：Faster Engines（加速工程任务）/ Smarter Execution（生成式 AI 与专用智能体、自然语言交互）/ Trusted Outcomes（AI 输出仍由 EDA 工具与传统验证把关）；
- **Aprisa AI**（RTL-to-GDS）：自然语言接口 + **AI Design Explorer**（按 PPA 目标自适应优化）；官方称生产力提升 **10 倍**、流片周期缩短 **3 倍**、数字设计 PPA 改善 **10%**（全工艺节点）；
- **Calibre Vision AI**：DRC 违例智能聚类，定位与修复时间减半；
- **Tessent**：ATPG Expert 自动平衡故障覆盖率与 pattern 数；制造端用无监督 ML/统计诊断 AI 做良率学习（官方另称良率提升 8–15%，见行业报道）；
- **Solido**：定制 IC 全流程生成式 + 智能体 AI；
- 与 NVIDIA 深度合作（NeMo/NIM、cuLitho 等 CUDA-X 库、PhysicsNeMo）。
- 来源：https://www.electronicsweekly.com/?p=875785 ；https://semiwiki.com/events/360812-siemens-eda-and-nvidia-pioneering-ai-driven-chip-design/

### 4.5 Google DeepMind AlphaChip —— 非 LLM 路线的 RL 物理层设计（对照组）

- **载体**：**Nature 2024**（"A graph placement methodology for fast chip design"，DOI: 10.1038/s41586-024-08032-5；前作为 Nature 2021）；开源仓库 https://github.com/google-research/circuit_training （含 20 个 TPU 块的预训练 checkpoint）；
- **方法**：把 floorplanning 当"游戏"的**强化学习** + **基于边的图神经网络**，从空白网格逐元件放置，以最终布局质量（线长/密度等）为奖励；预训练→迁移到新芯片模块（zero-shot 放置开源 Ariane RISC-V CPU）；
- **落地**：已用于 **Google 三代 TPU（v5e、v5p、Trillium）**的超人布局；联发科（MediaTek）已采用；布局生成从数周/数月人力缩短到**数小时**。
- 意义：与 LLM 路线互补——AlphaChip 优化物理层 PPA，LLM/智能体路线目前主攻前端代码与验证；二者共同构成"AI for Design"的完整图景。
- 来源：https://www.nature.com/articles/s41586-024-08032-5 ；https://deepmind.google/discover/blog/alphachip-ai-driven-engineering-for-next-generation-chip-design/ （相关报道：https://www.ithome.com/0/799/018.htm ）

---

## 五、技术路线对比总表

| 系统 | 机构 | 年份/载体 | 代码生成方式 | 反馈闭环 | PPA 优化 | 关键结果 | 主要局限 | 开源 |
|---|---|---|---|---|---|---|---|---|
| **AutoChip** | NYU | 2023, arXiv:2311.04887 | LLM 直接生成 Verilog（HDLBits） | **有**：Icarus Verilog 编译+仿真差异文本回注，多轮自纠错 | 无 | 编译器上下文使准确率 **+24.20%** | 小规模教学题；无 PPA；文本级反馈 | 是（GitHub） |
| **ChipNeMo** | NVIDIA | 2023, arXiv:2311.00176 | 领域自适应 LLaMA2（7B/13B/70B）：tokenizer+DAPT(24B tokens)+SFT/SteerLM+RAG | 无代码级闭环（RAG 属检索增强） | 无（语言类任务） | **ChipNeMo-70B 超 GPT-4**（chatbot/EDA 脚本）； chatbot 6.0/7；EDA 脚本 >70%；5× 规模缩减；部署：5000 工程师/年省 4000 人天 | 私有数据不可复现；bug 摘要增益小 | 模型未开源 |
| **RTLCoder** | HKUST | 2023, arXiv:2312.08617 | GPT 合成 >1 万指令-RTL 对 + 代码质量反馈微调 7B | 无（训练期质量反馈） | 无 | 超 GPT-3.5、可比 GPT-4；VerilogEval pass@1 66.9% | 合成数据分布失真；无自纠错 | 是（GitHub，全栈开源） |
| **ChatEDA** | CUHK | 2023, arXiv:2308.10204 | AutoMage（LLaMA2+QLoRA）生成调用 OpenROAD 的 Python 脚本 | **有**（工具编排层）：任务分解→脚本→子进程执行 | 间接（完整 RTL-to-GDSII 流程 + 早期 QoR 估计） | 多任务超 GPT-4（论文自评） | 不直接生成 RTL；依赖 API 文档 | 是（GitHub） |
| **MAGE** | UCSD | 2024, arXiv:2412.07822；DAC 2025 | 四智能体（RTL/TB/Judge/Debug）+ 高温采样(T=0.85,n=20)+仿真评分 | **有**：仿真评分 + **Verilog 状态检查点逐时钟沿比对**，定位首个错误周期 | 无 | VerilogEval-Human v2 **95.7%**，超 Claude-3.5-sonnet **23.3%** | 依赖闭源 API；检查点仅输出级信号；无 PPA | 是（GitHub） |
| **AssertLLM** | HKUST(GZ) | 2024, arXiv:2402.00386；ASP-DAC 2025 | 三 LLM 分工：规范抽取→波形分析→RAG 生成 SVA | 半闭环：JasperGold FPV 校验，无反例自动修复 | 无 | I2C（23 信号）**89%** 语法+功能正确；GPT-4o 仅 36% | 依赖规范质量；波形隐式行为推断弱 | 未找到公开代码 |
| **VerilogEval** | NVIDIA | 2023, arXiv:2309.07544；ICCAD'23 | （基准）156 题 HDLBits，仿真自动判分 | — | — | GPT-4 43.5% pass@1；GPT-4o 63%（2024 复测） | 已饱和、题目小 | 是（GitHub） |
| **RTLLM** | HKUST | 2023, arXiv:2308.05345；ASP-DAC'24 | （基准）30/29 设计 + self-planning | — | **有**（design quality goal） | 首次系统评估生成 RTA 质量 | 规模仍有限 | 是（GitHub） |
| **CVDP** | NVIDIA | 2025, arXiv:2506.14074 | （基准）783 题/13 类，agentic+non-agentic | — | 部分（功耗/面积优化类） | SOTA 代码生成 **≤34%** pass@1 | 新基准，生态初建 | 是（GitHub + LBC-bench） |
| **VeriGen** | NYU 等 | 2023, arXiv:2308.00708；DATE'23 | 5 万 GitHub 文件+70 教材微调 CodeGen-16B | 无 | 无 | 超 GPT-3.5-turbo 1.1%；语法 +41% | 数据未充分过滤（后续批评） | 是 |
| **OriGen** | PKU 等 | 2024, arXiv:2407.16237；ICCAD'24 | DeepSeek-Coder-7B+LoRA；code-to-code 增强 | **有（自反思）**：编译错误信息回注修复（VerilogFixEval 221 例） | 无 | 超前最佳开源 12.8%；pass@1 超 GPT-4 Turbo；自反思超 GPT-4 19.9% | 仅语法级修复；无功能级闭环 | 是（GitHub+HF） |
| **CraftRTL** | NVIDIA | 2024, arXiv:2409.12993 | correct-by-construction 非文本数据 + 错误注入修复数据；Starcoder2-15B | 无（数据层面） | 无 | pass@1 +3.8/+10.9/+6.6%（Machine/Human/RTLLM） | 仍需下游评测基准 | 是（GitHub，受限许可证） |
| **CodeV** | ICT-CAS+寒武纪 | 2024, arXiv:2407.10424；TCAD 2025 | 多级摘要反向合成 18.5 万对 + Chat-FIM-Tag SFT | 无 | 无 | Machine/Human pass@1 **80.1%/59.2%**；超 GPT-3.5 35.2% | 摘要质量上限；无 PPA | 是（HF 开源） |
| **Spec2RTL-Agent** | NVIDIA | 2025, arXiv:2506.13905 | 规范→高层代码→可综合 C++→HLS→RTL；三阶段多智能体 | **有（Reflection）**：迭代验证/调试/精化 | 间接（HLS QoR） | NIST 规范人工干预 **-75%** | 受 HLS 语义限制；排除多种 RTL 模式 | 论文公开 |
| **VeriDebug** | （Ning Wang 等） | 2025, arXiv:2504.19099 | 统一检测+修复：对比嵌入+引导纠正 | 有（内部信息检索替代外部工具） | 无 | Acc1 **64.7**（开源 SOTA 11.3；GPT-3.5 36.6） | 可扩展性与分类别性能未讨论 | 数据集开源 |
| **VerilogAssist** | UCSD+Berkeley+Intel | 2024, arXiv:2406.00115 | LLM 生成 RTL+TB，仿真器入环 | **有**：self-verification（逐时间步推演）+self-correction（读编译/仿真结果） | 部分（FPGA 资源最多 -45%） | RTLLM 语法 100%、功能 75.9%（+10.4%） | 无综合反馈；错误 TB 会固化错误 RTL | 论文公开 |
| **Synopsys.ai Copilot** | Synopsys | 2023 发布；2025-09 扩充 | 商用 LLM+RAG：知识/工作流/代码/形式化/lint 五类 Copilot | 有（工具内嵌：Euclide lint、VC Formal、SpyGlass） | 间接（验证加速） | 2–5× 生产力；知识问答 -70%；Formal 4–5×；Code Advisor +30% | 闭源商用；指标为客户口径 | 否 |
| **Cadence ChipStack** | Cadence | 2026-02 发布；2026-06 L5 | 超级智能体编排多虚拟工程师（RTL/TB/回归/调试），Mental Model + Nemotron/GPT | **有（深度）**：Xcelium 仿真+Jasper 形式化入环，自动修复 | **有（早期 PPA）**：RTL Generation Agent 面积 -24%/功耗 -18% | 生产力最高 10×；Altera 验证 -10×；NVIDIA 内部验证循环 **>40×** | 早期访问阶段；指标为厂商口径 | 否 |
| **Siemens EDA AI** | Siemens EDA | 2025-07（DAC） | 全栈 AI 系统：ML+RL+GenAI+Agentic，多 LLM、可本地微调 | 有（AI 输出由 EDA 工具验证） | **有**：Aprisa AI Design Explorer PPA +10% | 生产力 10×、流片周期 3×、DRC 调试时间减半 | 早期访问；厂商口径 | 否 |
| **AlphaChip** | Google DeepMind | Nature 2024 | RL+边式 GNN 布局（非 LLM） | 奖励=布局质量（RL 内循环） | **有（物理层核心）** | 三代 TPU 超人布局；MediaTek 采用；数小时 vs 数周 | 仅物理层；与前端 LLM 路线割裂 | 是（GitHub+checkpoint） |

---

## 六、2025–2026 最新进展与工业落地现状

### 6.1 学术前沿（2025–2026）

1. **基准换代**：VerilogEval v2 饱和（63%+）→ **CVDP**（783 题、13 类、agentic 格式，SOTA ≤34% pass@1）；Si2 推出 **LBC-bench** 开源排行榜（基于 CVDP），并成立 LLM Benchmarking Coalition。
2. **RL / 推理增强成为主线**：
   - **QiMeng-CodeV-R1**（ICT-CAS，NeurIPS 2025）：规则化 testbench 生成器 + round-trip 数据合成 + adaptive DAPO，"蒸馏→RL"两阶段；VerilogEval v2 / RTLLM v2 pass@1 68.6%/72.9%，超此前 SOTA 12–21%，匹配/超越 DeepSeek-R1-671B；
   - **VeriReason**（arXiv:2505.11849）：GRPO + testbench 反馈，pass@5 83.1%（被指出存在 reward hacking 风险）；
   - **ScaleRTL**：从 DeepSeek-R1 蒸馏长 CoT，超 18 个基线 18.4%；
   - **ChipSeek**：把 EDA 综合反馈折叠进 PPA 感知的层级奖励（被批评：每样本全综合耗时以分钟计，训练成本过高）。
3. **多智能体系统深化**：VerilogCoder（规划+编码+**AST 波形追踪**，pass@1 94.2%）、MAGE（DAC 2025）、Spec2RTL-Agent、VeriOpt（Planner/Programmer/Reviewer/Evaluator，PPA 感知迭代）、RTLSquad、VFlow 等；共同问题是"智能体间以非结构化自然语言传递上下文，结构意图易丢失"。
4. **PPA 与验证缺口被量化**（2026 年 arXiv 预印本，引用时需注意为预印本）：
   - GRADE-RTL（arXiv:2609.25335）等报告：**44.2% 通过 testbench 的设计未通过形式等价性检查**；相对人工优化设计仍有 **1.11–1.38 倍面积开销**；约 55% 的 LLM 生成 Verilog 语法问题可自动修复；
   - "Revolution or Hype? Seeking the Limits of Large Models in Hardware Design"（ICCAD 2025 panel 论文，arXiv:2509.04905）：领域正从"能否辅助"转向"如何工程化为可靠工具、持续达成高 PPA 而不损害生产可靠性"；提出 **Large Circuit Models（LCM）** 概念（多模态电路基础模型，如 DeepGate3、CircuitFusion）。
5. **RTL 修复/调试专题繁荣**：RTLFixer（NVIDIA，DAC 2024）、VeriDebug（2025）、LiK（2024，定位 93.3% pass@1）、HDLdebugger（TODAES 2025）、"From Bugs to Fixes"（LAD 2024）。

### 6.2 工业落地现状（截至 2026 年中）

| 厂商 | 产品/系统 | 状态与关键指标 |
|---|---|---|
| NVIDIA | ChipNeMo（内部）→ Nemotron 智能体团队 | **内部大规模部署**：5,000 工程师、年省 4,000 工程人天、准确率 >85% |
| Synopsys | Synopsys.ai Copilot（5 个应用）+ **AgentEngineer** | 数千用户；2–5× 生产力；Converge 2026 展示 **L4 级多智能体编排工作流** |
| Cadence | Cerebrus/JedAI/Verisium → **ChipStack AI Super Agent** → **AgentStack** | 2026-02 发布；早期客户 Altera/NVIDIA/Qualcomm/Tenstorrent；最高 10× 生产力；NVIDIA 内部验证循环 >40×；2026-09 新增 RTL Generation Agent（面积 -24%、功耗 -18%）；2026 Q4 早期开放 |
| Siemens EDA | **EDA AI System**（Aprisa/Solido/Tessent/Calibre Vision） | 2025-07 DAC 发布；早期访问；10× 设计效率、3× 流片周期、PPA +10%、DRC 调试时间减半 |
| Google DeepMind | **AlphaChip** | Nature 2024；三代 TPU + MediaTek 生产采用；开源（circuit_training） |

**总体判断**：工业界已从"单点 Copilot（问答/脚本/lint）"进入"**超级智能体编排 + 可信 EDA 工具把关**"阶段（Synopsys AgentEngineer、Cadence ChipStack/AgentStack、Siemens EDA AI System 三家的架构趋同）；前端 RTL 生成首次出现厂商披露的 PPA 收益（Cadence -24% 面积/-18% 功耗）。学术侧则从"pass@k 正确率"转向 RL 推理、agentic 基准（CVDP）与"PPA/形式验证/物理正确性"全链路评估。

---

## 七、技术范式分类归纳

**范式 A：微调 + 数据工程（Fine-tuning + Data Engineering）**
代表：VeriGen、RTLCoder、OriGen、CodeV、CraftRTL
共性：核心竞争在"高质量 描述–代码 对"的构造（真实代码爬取/LLM 合成/反向多级摘要/correct-by-construction/错误注入修复数据），基座选 CodeGen/DeepSeek-Coder/Starcoder2 等 7B–16B 开源模型；推理期单次前向、无工具闭环；评测依赖 VerilogEval/RTLLM。

**范式 B：反馈闭环 / 自我纠错（Tool-in-the-loop Self-Correction）**
代表：AutoChip（编译+仿真差异回注）、VerilogAssist（自验证+自纠正）、OriGen_Fix（编译错误自反思）、RTLFixer、VeriDebug（嵌入检索+引导纠正）
共性：把编译器/仿真器的文本输出解析回注 LLM，多轮迭代至通过；关键是"反馈的粒度"——从 AutoChip 的文本级差异，到 MAGE 的信号级状态检查点，到 VerilogCoder 的 AST 波形追踪，粒度越细修复越准。

**范式 C：多智能体工具调用（Agentic Tool Orchestration）**
代表：ChatEDA（编排 EDA 工具跑 RTL-to-GDSII）、MAGE（四智能体递归）、VerilogCoder、Spec2RTL-Agent、VeriOpt、RTLSquad
共性：任务分解为专门角色（生成/验证/评判/调试/规划），各角色独立上下文；EDA 工具（仿真器/综合器/HLS）作为执行器入环；工业界对应形态为 Synopsys AgentEngineer、Cadence ChipStack、Siemens EDA AI System 的"超级智能体 + 子智能体 + 可信工具"层级架构。

**范式 D：领域自适应 + RAG（Domain Adaptation + Retrieval）**
代表：ChipNeMo（DAPT+领域 tokenizer+SteerLM+微调检索器）、Synopsys.ai Copilot（专有 RAG 流水线）、Siemens EDA AI（EDA 专用 RAG+向量库+本地微调）、HDLdebugger（RAG 检索错误–方案库）、Ask-EDA/ORAssistant（学术侧）
共性：面向**私有语料与安全部署**（数据不出域），用继续预训练+领域检索把通用 LLM 变为"懂行"的助手；ChipNeMo 证明 DAPT 是主要增益来源、RAG 显著提升可追溯性；这是目前工业界最成熟的商用形态。

**范式 E：RL / 推理增强（2025 年起的主流增量）**
代表：QiMeng-CodeV-R1（distill-then-RL + adaptive DAPO）、VeriReason（GRPO+testbench 奖励）、ScaleRTL（CoT 蒸馏）、VeriRL、ChipSeek（PPA 感知层级奖励）
共性：把 testbench 通过率（乃至综合 QoR）作为可验证奖励做 RLVR；瓶颈是奖励稀疏、reward hacking（表面模式骗过 testbench）与综合奖励的计算成本。

**范式 F（对照组）：非 LLM 的 AI-for-EDA**
代表：AlphaChip（RL+边式 GNN 布局）、DeepGate 系列（电路表征学习）、MaskPlace/CircuitFusion（LCM）
共性：不经过文本，直接学习电路结构/几何表征，在物理层 PPA 优化上已实现生产级收益（TPU/MediaTek），与 LLM 前端路线呈互补关系，被视为"Large Circuit Models"方向。

---

## 八、共性局限与已知批评（学术报告可直接引用的批判点）

1. **正确性幻觉三层陷阱**（2026 年 arXiv 预印本 GRADE-RTL 等）：语法层（约 55% 问题可自动修复，说明 token 级合理≠语法有效）；功能层（**44.2% 通过 testbench 的设计未通过形式等价性检查**——仿真通过≠功能正确）；物理层（功能正确的设计相对人工优化仍有 **1.11–1.38 倍面积开销**）。
2. **"自评自判"验证缺口**：LLM 生成 testbench 又用自己的 testbench 评分（AutoChip/MAGE/VeriAssist 均有此风险；VeriAssist 作者明确承认），需要独立 oracle（golden 参考/形式验证）把关。
3. **pass@k 指标的误导性**：pass@k 主导早期评测，排除物理正确性与大规模真实设计，导致基准饱和（VerilogEval v2 63%+）后失去区分度。
4. **数据瓶颈与分布失真**：公开语料中 Verilog 极少（The Stack v2 中 Verilog 1.91M vs Python 80.6M，CodeV 论文数据）；合成数据存在"描述–电路非法对应"（CodeV 论文图 2 实例：组合环路）。
5. **多智能体的上下文传递损耗**：角色间以非结构化自然语言传递设计意图，层级/时钟域/结构信息易丢失（2026 年综述 arXiv:2607.18519 的批评）。
6. **PPA 优化仍是最大短板**：绝大多数系统只到"功能正确"；少数引入综合反馈者（ChipSeek 等）面临训练成本爆炸；工业界只有 Cadence/Siemens 在编排层给出可核查的 PPA 数字。
7. **可复现性**：ChipNeMo 依赖私有语料无法复现；商用 Copilot 指标均为厂商/客户口径，无独立第三方审计。
8. **安全与隐私**：硬件设计 IP 高度敏感，推动"本地部署 + 开源模型"路线（ChipNeMo 动机、VeriDebug 动机、Siemens 本地微调），与云端通用 LLM 存在长期张力。

---

## 九、参考来源汇总（按系统）

**AutoChip**：https://arxiv.org/abs/2311.04887 ；代码 https://github.com/shailja-thakur/AutoChip
**ChipNeMo**：https://arxiv.org/abs/2311.00176 （v4: https://arxiv.org/html/2311.00176v4 ）；部署 https://blogs.nvidia.com/blog/ways-ai-agents-are-raising-team-performance/
**RTLCoder**：https://arxiv.org/abs/2312.08617 ；代码 https://github.com/hkust-zhiyao/RTLCoder
**ChatEDA**：https://arxiv.org/abs/2308.10204 ；代码 https://github.com/wuhy68/ChatEDAv1
**MAGE**：https://arxiv.org/abs/2412.07822 ；DAC 2025: https://ieeexplore.ieee.org/document/11133191 ；代码 https://github.com/stable-lab/MAGE-A-Multi-Agent-Engine-for-Automated-RTL-Code-Generation
**AssertLLM**：https://arxiv.org/abs/2402.00386 ；ASP-DAC 2025: https://www.aspdac.com/aspdac2025/archive/pdf/4D-1.pdf
**VerilogEval**：https://arxiv.org/abs/2309.07544 ；复测 https://arxiv.org/pdf/2408.11053v2 ；代码 https://github.com/NVlabs/verilog-eval
**RTLLM**：https://arxiv.org/abs/2308.05345 ；代码 https://github.com/hkust-zhiyao/rtllm
**CVDP**：https://arxiv.org/abs/2506.14074 ；代码 https://github.com/NVlabs/cvdp_benchmark ；LBC-bench https://si2.org/?p=1467/
**VeriGen**：https://arxiv.org/pdf/2308.00708 ；NYU 报道 https://techxplore.com/news/2025-06-ai-specialized-chip-language.pdf
**OriGen**：https://arxiv.org/abs/2407.16237 ；ICCAD'24 https://dlnext.acm.org/doi/10.1145/3676536.3676830 ；代码 https://github.com/pku-liang/OriGen
**CraftRTL**：https://arxiv.org/abs/2409.12993 ；代码 https://github.com/NVlabs/CraftRTL
**CodeV**：https://arxiv.org/abs/2407.10424 ；TCAD 2025（作者主页 https://huangdi95.github.io/ ）；CodeV-R1/QiMeng https://arxiv.org/abs/2506.05007
**Spec2RTL-Agent**：https://arxiv.org/abs/2506.13905
**VeriDebug**：https://arxiv.org/abs/2504.19099
**VerilogAssist**：https://arxiv.org/abs/2406.00115
**CircuitGPT（未找到，替代：ChipGPT）**：https://arxiv.org/abs/2305.14019
**HDLBug（未找到，替代：HDLdebugger）**：https://dl.acm.org/doi/10.1145/3735638 ；From Bugs to Fixes https://anan.math.uni-bremen.de/doc/konf/LAD2024_KQ.pdf ；LiK https://arxiv.org/abs/2409.15186
**Synopsys**：https://synopsys.com/blogs/chip-design/synopsys-ai-copilots-chip-design.html ；https://cn.news.synopsys.com/2025-09-11-EDA
**Cadence**：https://cadence.com/en_US/home/company/newsroom/press-releases/pr/2026/cadence-unleashes-chipstack-ai-super-agent-pioneering-a-new.html.html ；https://cadence.com/en_US/home/company/newsroom/press-releases/pr/2026/cadence-expands-chipstack-ai-super-agent-with-a-new-agent-for.html ；https://www.businesswire.com/news/home/20220913005383/en/
**Siemens EDA**：https://www.electronicsweekly.com/?p=875785 ；https://semiwiki.com/events/360812-siemens-eda-and-nvidia-pioneering-ai-driven-chip-design/
**AlphaChip**：https://www.nature.com/articles/s41586-024-08032-5 ；代码 https://github.com/google-research/circuit_training
**批判性文献**："Revolution or Hype?" https://arxiv.org/abs/2509.04905 ；GRADE-RTL https://arxiv-vanity.com/papers/2609.25335 （预印本，引用需注明）；多智能体综述 https://arxiv-vanity.com/papers/2607.18519 （预印本，引用需注明）

---

### 撰写说明（给报告作者的三条提醒）

1. **两处"查无实据"**：CircuitGPT 与 HDLBug 在公开渠道均无对应原始资料（详见 3.8/3.9 节的排除过程与替代建议），建议在正式报告中改写为 ChipGPT 与 HDLdebugger/LiK 等真实系统，或明确注释"未找到公开资料"。
2. **数字口径**：商业厂商指标（Synopsys/Cadence/Siemens/NVIDIA 部署数据）均为**厂商或客户自述口径**，非独立第三方审计，建议引用时注明；学术 pass@k 数字来自各论文自评，跨论文比较需注意测试集版本（如 VerilogEval v1/v2、RTLLM v1.0/v1.1）差异。
3. **2026 年预印本**（GRADE-RTL、2607.18519 等）提供了关键的批判性量化证据（44.2% 形式验证失败率、1.11–1.38× 面积开销），但尚未经同行评审，正式报告建议以"预印本"标注引用。
