# Phase 4：AXI4-Stream 算力核 IP 封装规格书

## 1. 概述

`axi_stream_array` 将 Phase 3 的 `self_healing_array`（N=4 逻辑列 + 1 备用列）封装为标准 AXI4-Stream IP：任务数据与权重/命令统一走 64 位流接口，自愈状态以侧带信号暴露（生产 SoC 应映射到 AXI4-Lite CSR）。

## 2. 接口

### 2.1 上行通道 s_axis（宿主 → IP）

| 信号 | 位宽 | 描述 |
|---|---:|---|
| `s_axis_tdata` | 64 | 载荷（含义取决于 tuser） |
| `s_axis_tuser` | 2 | `2'b00`=激活行；`2'b01`=权重字；`2'b10`=命令 |
| `s_axis_tvalid` / `s_axis_tready` / `s_axis_tlast` | 1 | 标准握手；tlast 标记矩阵最后一行 |

拍类型（tuser 编码）：

- **权重字** `01`：`tdata[15:0]` 为一个权重，按行主序连续 N² 拍送满后 IP 自动脉冲 `load_w` 装载。
- **激活行** `00`：`tdata[N*16-1:0]` 为一行 A，`tdata[16*r +: 16] = A[m][r]`；`tlast=1` 表示本矩阵最后一行。
- **命令** `10`：`tdata[0]=1` 触发自愈流程（BIST→重映射→复验）。

### 2.2 下行通道 m_axis（IP → 宿主）

每接受一行激活，最终产生一个 64 位输出拍：`tdata[16*c +: 16] = C[m][c]`（逻辑列，重映射对宿主透明），同矩阵最后一行伴随 `tlast=1`。

### 2.3 侧带自愈状态

`sh_busy` / `fault_detected` / `healed` / `unhealable` / `fault_col[4:0]`。

## 3. 协议约束（硬件保证）

1. **信用节流**：阵列内部不可反压，IP 用信用计数 `in_flight`（已收行 − 已出行）限制 `s_axis_tready`：激活拍仅在 `in_flight < CREDIT_MAX` 时接受，保证每列重组 FIFO（深 32）不溢出。`CREDIT_MAX = 32 − N − 2 = 26`。
2. **静默要求**：权重拍与命令拍仅在 `in_flight == 0`（阵列排空）且非 BIST 期间被接受 —— 防止权重改写/BIST 污染在飞行中的矩阵。
3. **BIST 独占**：自愈流程期间 `s_axis_tready=0`，BIST 输出不会进入重组 FIFO（`sh_busy` 门控写使能）。
4. **BIST 覆盖权重**：自愈完成后宿主必须重新下发 N² 拍权重。
5. 标准 AXI 规则：stall 期间 payload 稳定；`tvalid` 不依赖 `tready` 组合路径。

## 4. 时序

行 m 接受后，输出拍出现于约 `m+N+N+2` 拍之后（各列在内部按列相位差错峰，由重组 FIFO 对齐为整行）。稳态吞吐 1 行/周期。
