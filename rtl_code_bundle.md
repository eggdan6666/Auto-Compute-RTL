# Auto-Compute-RTL 完整硬件源代码合集 (SystemVerilog Source Code - R2 Final Verified)

## 文件: rtl/mac_unit.sv

``systemverilog
`timescale 1ns / 1ps

module mac_unit (
    input  logic        clk,
    input  logic        rst_n,
    input  logic        valid_in,
    input  logic signed [15:0] A,
    input  logic signed [15:0] B,
    input  logic signed [39:0] Acc_in,
    output logic signed [15:0] Out,
    output logic        Valid_out
);

    // =========================================================================
    // Stage 1 (PR1): Multiply + sign-extend + latch Acc_in
    // =========================================================================
    
    // Internal signals for Stage 1 combinational logic
    logic signed [31:0] p_s1;
    logic signed [39:0] p_ext_s1;
    
    // Pipeline registers for Stage 1
    logic signed [39:0] prod_ext_s1_reg;
    logic signed [39:0] acc_s1_reg;
    logic               valid_s1_reg;
    
    always_comb begin
        p_s1 = 32'(A) * 32'(B); // Explicit 32-bit signed context before multiply
        p_ext_s1 = {{8{p_s1[31]}}, p_s1[31:0]}; // Sign-extend to 40 bits
    end
    
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            prod_ext_s1_reg <= '0;
            acc_s1_reg      <= '0;
            valid_s1_reg    <= 1'b0;
        end else begin
            prod_ext_s1_reg <= p_ext_s1;
            acc_s1_reg      <= Acc_in;
            valid_s1_reg    <= valid_in;
        end
    end

    // =========================================================================
    // Stage 2 (PR2): Accumulate + RNE round + saturate
    // =========================================================================
    
    // Internal signals for Stage 2 combinational logic
    logic signed [40:0] s_41;
    logic signed [39:0] s_40;
    
    // Rounding signals
    logic g_bit;
    logic st_bit;
    logic l_bit;
    logic inc;
    logic signed [40:0] q_floor;
    logic signed [40:0] q_round;
    
    // Saturation signals
    logic pos_ovf;
    logic neg_ovf;
    logic signed [15:0] out_next;
    
    always_comb begin
        // 1. 41-bit signed addition: {acc_s1[39], acc_s1} + {prod_ext_s1[39], prod_ext_s1}
        s_41 = {acc_s1_reg[39], acc_s1_reg} + {prod_ext_s1_reg[39], prod_ext_s1_reg};
        
        // 2. 40-bit overflow clamp
        if (s_41[40:39] == 2'b01) begin
            s_40 = 40'sh7F_FFFF_FFFF;
        end else if (s_41[40:39] == 2'b10) begin
            s_40 = 40'sh80_0000_0000;
        end else begin
            s_40 = s_41[39:0];
        end
        
        // 3. Round-to-nearest-even (RNE) by right-shifting 15 bits
        g_bit = s_40[14];
        st_bit = |s_40[13:0];
        l_bit = s_40[15];
        inc = g_bit & (st_bit | l_bit);
        
        // Q_floor = sign_extend to 41 bits of S_40 >>> 15
        q_floor = 41'($signed(s_40) >>> 15);
        q_round = q_floor + 41'(inc);
        
        // 4. 16-bit saturation
        pos_ovf = !q_round[40] & (|q_round[39:15]);
        neg_ovf = q_round[40] & ~( &q_round[39:15] );
        
        if (pos_ovf) begin
            out_next = 16'sh7FFF;
        end else if (neg_ovf) begin
            out_next = 16'sh8000;
        end else begin
            out_next = q_round[15:0];
        end
    end
    
    // Output registers
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            Out <= 16'h0000;
            Valid_out <= 1'b0;
        end else begin
            Valid_out <= valid_s1_reg;
            // 5. & 6. Valid_out logic and holding previous result
            if (valid_s1_reg) begin
                Out <= out_next;
            end
        end
    end

endmodule

``n
---

## 文件: rtl/rne_sat16.sv

``systemverilog
`timescale 1ns / 1ps

// =============================================================================
// rne_sat16.sv — 脉动阵列列出口: 40 位部分和 -> RNE 舍入 -> 16 位饱和
// 与 mac_unit Stage 2 的舍入/饱和逻辑逐位等价 (MAC_microarchitecture_spec.md
// 4.2/5 节)。in_valid=0 时 out_data 保持上一有效值。
// =============================================================================

module rne_sat16 (
    input  logic               clk,
    input  logic               rst_n,
    input  logic               in_valid,
    input  logic signed [39:0] in_psum,
    output logic               out_valid,
    output logic signed [15:0] out_data
);

    logic g_bit, st_bit, l_bit, inc;
    logic signed [40:0] q_floor, q_round;
    logic pos_ovf, neg_ovf;
    logic signed [15:0] out_next;

    always_comb begin
        g_bit = in_psum[14];
        st_bit = |in_psum[13:0];
        l_bit = in_psum[15];
        inc = g_bit & (st_bit | l_bit);

        q_floor = 41'(in_psum >>> 15);
        q_round = q_floor + 41'(inc);

        pos_ovf = !q_round[40] & (|q_round[39:15]);
        neg_ovf = q_round[40] & ~(&q_round[39:15]);

        if (pos_ovf)      out_next = 16'sh7FFF;
        else if (neg_ovf) out_next = 16'sh8000;
        else              out_next = q_round[15:0];
    end

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            out_valid <= 1'b0;
            out_data  <= 16'h0000;
        end else begin
            out_valid <= in_valid;
            if (in_valid) out_data <= out_next;
        end
    end

endmodule

``n
---

## 文件: rtl/systolic_pe.sv

``systemverilog
`timescale 1ns / 1ps

// =============================================================================
// systolic_pe.sv — 权重固定脉动阵列处理单元 (PE)
// 依据 systolic_array_spec.md 4.3: 乘加保留 40 位部分和, 不做舍入/饱和;
// pv_in 门控累加 (操作数隔离), 无效拍部分和自清零。
// =============================================================================

module systolic_pe (
    input  logic               clk,
    input  logic               rst_n,
    input  logic               load_w,
    input  logic signed [15:0] w_in,      // 待加载权重
    input  logic               av_in,     // 左侧激活有效
    input  logic signed [15:0] a_in,      // 左侧激活
    output logic               av_out,    // 右传激活有效
    output logic signed [15:0] a_out,     // 右传激活
    input  logic               pv_in,     // 上方部分和有效
    input  logic signed [39:0] psum_in,   // 上方部分和
    output logic               pv_out,    // 下传部分和有效
    output logic signed [39:0] psum_out   // 下传部分和
);

    logic signed [15:0] w_reg;
    logic signed [31:0] prod;
    logic signed [40:0] s41;

    always_comb begin
        prod = 32'(a_in) * 32'(w_reg);
        s41  = 41'(psum_in) + 41'(prod); // Spec 2: N<=511 时 40 位不溢出
    end

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            w_reg    <= '0;
            a_out    <= '0;
            av_out   <= 1'b0;
            psum_out <= '0;
            pv_out   <= 1'b0;
        end else begin
            if (load_w) w_reg <= w_in;
            a_out  <= a_in;
            av_out <= av_in;
            if (pv_in) begin
                psum_out <= s41[39:0];
                pv_out   <= 1'b1;
            end else begin
                psum_out <= '0;
                pv_out   <= 1'b0;
            end
        end
    end

endmodule

``n
---

## 文件: rtl/systolic_array.sv

``systemverilog
`timescale 1ns / 1ps

// =============================================================================
// systolic_array.sv — NR×NC 权重固定脉动阵列 (systolic_array_spec.md)
//
// 数据流: 激活左->右, 部分和上->下, 权重驻留 PE。
// 内置三角斜切缓冲: 第 r 行激活延迟 r 拍, 宿主按自然行序馈入。
// 列出口: rne_sat16 (RNE + 16 位饱和), C[m][c] 于第 m+NR+c+1 拍输出。
//
// NR = 行数 (激活/权重行), NC = 列数 (权重列/输出列)。
// 任务模式 NR==NC; NC>NR 时多余物理列可作自愈冗余 (见 self_healing_array)。
//
// 端口为扁平化打包总线 (兼容 Icarus/Verilator 及综合工具):
//   w_flat[16*(r*NC+c) +: 16] = W[r][c]   (物理列寻址)
//   a_flat[16*r +: 16]        = 当前激活行 A[m][r]
//   c_flat[16*c +: 16]        = 列 c 的输出 C[m][c]
// =============================================================================

module systolic_array #(
    parameter int NR = 4,
    parameter int NC = 4
) (
    input  logic                 clk,
    input  logic                 rst_n,
    input  logic                 load_w,
    input  logic [NR*NC*16-1:0]  w_flat,
    input  logic                 valid_in,
    input  logic [NR*16-1:0]     a_flat,
    output logic [NC-1:0]        valid_out,
    output logic [NC*16-1:0]     c_flat
);

    // Spec 2: N<=511 时 40 位累加不溢出 (NR 为每列累加深度)
    initial begin
        if (!(NR <= 511 && NR >= 1 && NC >= 1))
            $error("systolic_array: NR=%0d 超出 40 位 Guard Bits 覆盖范围", NR);
    end

    genvar r, c;

    // -------------------------------------------------------------------------
    // 三角斜切缓冲: 第 r 行延迟 r 拍 (Spec 4.1)
    // -------------------------------------------------------------------------
    logic signed [15:0] a_skew [NR];
    logic               v_skew [NR];

    generate
        for (r = 0; r < NR; r++) begin : g_skew
            if (r == 0) begin : g_passthrough
                assign a_skew[0] = a_flat[15:0];
                assign v_skew[0] = valid_in;
            end else begin : g_delay
                logic signed [15:0] dly [r];
                logic               vly [r];
                always_ff @(posedge clk or negedge rst_n) begin
                    if (!rst_n) begin
                        for (int i = 0; i < r; i++) begin
                            dly[i] <= '0;
                            vly[i] <= 1'b0;
                        end
                    end else begin
                        dly[0] <= a_flat[16*r +: 16];
                        vly[0] <= valid_in;
                        for (int i = 1; i < r; i++) begin
                            dly[i] <= dly[i-1];
                            vly[i] <= vly[i-1];
                        end
                    end
                end
                assign a_skew[r] = dly[r-1];
                assign v_skew[r] = vly[r-1];
            end
        end
    endgenerate

    // -------------------------------------------------------------------------
    // PE 阵列互连
    //   a_h[r][c]  : 进入 PE(r,c) 的激活      (c = 0..NC, [0] 为斜切缓冲输出)
    //   av_h[r][c] : 进入 PE(r,c) 的激活有效位
    //   p_v[r][c]  : 进入 PE(r,c) 的部分和     (r = 0..NR, [0] 为列顶)
    //   pv_v[r][c] : 进入 PE(r,c) 的部分和有效位
    // -------------------------------------------------------------------------
    logic signed [15:0] a_h  [NR][NC+1];
    logic               av_h [NR][NC+1];
    logic signed [39:0] p_v  [NR+1][NC];
    logic               pv_v [NR+1][NC];

    generate
        for (r = 0; r < NR; r++) begin : g_feed
            assign a_h[r][0]  = a_skew[r];
            assign av_h[r][0] = v_skew[r];
        end
        for (c = 0; c < NC; c++) begin : g_col_top
            // 列顶: 部分和为 0, 有效位取自到达 PE(0,c) 的激活有效位 (波前对齐)
            assign p_v[0][c]  = 40'sd0;
            assign pv_v[0][c] = av_h[0][c];
        end
    endgenerate

    generate
        for (r = 0; r < NR; r++) begin : g_row
            for (c = 0; c < NC; c++) begin : g_pe
                systolic_pe u_pe (
                    .clk      (clk),
                    .rst_n    (rst_n),
                    .load_w   (load_w),
                    .w_in     (w_flat[16*(r*NC+c) +: 16]),
                    .av_in    (av_h[r][c]),
                    .a_in     (a_h[r][c]),
                    .av_out   (av_h[r][c+1]),
                    .a_out    (a_h[r][c+1]),
                    .pv_in    (pv_v[r][c]),
                    .psum_in  (p_v[r][c]),
                    .pv_out   (pv_v[r+1][c]),
                    .psum_out (p_v[r+1][c])
                );
            end
        end
    endgenerate

    // -------------------------------------------------------------------------
    // 列出口: RNE + 16 位饱和
    // -------------------------------------------------------------------------
    generate
        for (c = 0; c < NC; c++) begin : g_exit
            rne_sat16 u_exit (
                .clk       (clk),
                .rst_n     (rst_n),
                .in_valid  (pv_v[NR][c]),
                .in_psum   (p_v[NR][c]),
                .out_valid (valid_out[c]),
                .out_data  (c_flat[16*c +: 16])
            );
        end
    endgenerate

endmodule

``n
---

## 文件: rtl/misr16.sv

``systemverilog
`timescale 1ns / 1ps

// =============================================================================
// misr16.sv — 16 位多输入特征寄存器 (MISR, Fibonacci LFSR)
// 与 golden_bist.py 的 misr16() 逐位一致:
//   fb = s[15]; s' = {s[14:0],1'b0} ^ (fb ? 16'h002D : 0) ^ data
// =============================================================================

module misr16 (
    input  logic        clk,
    input  logic        rst_n,
    input  logic        clear,      // 同步清零到初值 16'hACE1
    input  logic        en,         // 压缩使能
    input  logic [15:0] data,
    output logic [15:0] state
);

    logic [15:0] next_state;

    always_comb begin
        next_state = {state[14:0], 1'b0} ^ (state[15] ? 16'h002D : 16'h0000) ^ data;
    end

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n)   state <= 16'hACE1;
        else if (clear) state <= 16'hACE1;
        else if (en)  state <= next_state;
    end

endmodule

``n
---

## 文件: rtl/bist_controller.sv

``systemverilog
`timescale 1ns / 1ps

// =============================================================================
// bist_controller.sv — 脉动阵列内建自测试 (BIST) 控制器
//
// 流程: IDLE -> LOAD_W (装载 BIST 物理权重 ROM) -> RUN (逐拍送入 MB 行激活)
//       -> COLLECT (等待全部物理列回齐 MB 个结果) -> EVAL -> DONE
//
// 每物理列一个 MISR16 在线压缩输出; EVAL 时与 golden_bist.py 生成的
// 黄金签名比对 (skip_col 指定的已知坏列除外), 输出最低故障物理列号。
// 结果为*物理列*索引 —— 自愈重映射直接以物理列为单位。
//
// bist_load_w / bist_valid / bist_a_flat 为状态与行号的纯组合函数,
// 保证 valid 与数据严格同拍对齐。
// =============================================================================

module bist_controller #(
    parameter int N  = 4,    // 逻辑列数 (= 行数), 物理列数 = N+1
    parameter int MB = 16    // BIST 激活行数
) (
    input  logic                  clk,
    input  logic                  rst_n,
    input  logic                  start,          // 脉冲启动一次 BIST
    input  logic [4:0]            skip_col,       // 比对时排除的物理列 (N+1 表示无)
    // 阵列物理接口 (N+1 列)
    output logic                  bist_load_w,
    output logic [N*(N+1)*16-1:0] bist_w_flat,
    output logic                  bist_valid,
    output logic [N*16-1:0]       bist_a_flat,
    input  logic [N:0]            arr_valid_out,
    input  logic [(N+1)*16-1:0]   arr_c_flat,
    // 结果
    output logic                  busy,
    output logic                  done,
    output logic                  pass,
    output logic [4:0]            fail_col
);

    localparam int NPC       = N + 1;            // 物理列数
    localparam int DRAIN_MAX = 8 * N + 16;       // 排空超时上限

    // -------------------------------------------------------------------------
    // ROM (golden_bist.py 生成)
    // -------------------------------------------------------------------------
    logic [15:0]     w_rom    [0:N*NPC-1];
    logic [N*16-1:0] row_rom  [0:MB-1];
    logic [15:0]     gold_rom [0:NPC-1];

    initial begin
        $readmemh("bist_weights.hex", w_rom);
        $readmemh("bist_rows.hex",    row_rom);
        $readmemh("bist_golden.hex",  gold_rom);
    end

    // -------------------------------------------------------------------------
    // FSM
    // -------------------------------------------------------------------------
    typedef enum logic [2:0] {S_IDLE, S_LOAD, S_RUN, S_COLLECT, S_EVAL} state_t;
    state_t state;

    logic [7:0] row_cnt;     // RUN: 当前发送行号
    logic [7:0] drain_cnt;   // COLLECT: 排空计时
    logic [7:0] rcvd [NPC];  // 每列已收结果数
    logic       misr_clear;

    // 阵列驱动: 纯组合, valid 与数据同拍
    always_comb begin
        for (int i = 0; i < N*NPC; i++)
            bist_w_flat[16*i +: 16] = w_rom[i];
        bist_a_flat = row_rom[row_cnt[$clog2(MB)-1:0]];
        bist_load_w = (state == S_LOAD);
        bist_valid  = (state == S_RUN);
    end

    // -------------------------------------------------------------------------
    // 每列 MISR16 + 接收计数 (busy 期间常开)
    // -------------------------------------------------------------------------
    logic [15:0] misr_state [NPC];

    generate
        for (genvar c = 0; c < NPC; c++) begin : g_misr
            misr16 u_misr (
                .clk   (clk),
                .rst_n (rst_n),
                .clear (misr_clear),
                .en    (busy && arr_valid_out[c]),
                .data  (arr_c_flat[16*c +: 16]),
                .state (misr_state[c])
            );
        end
    endgenerate

    integer k;
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            for (k = 0; k < NPC; k++) rcvd[k] <= 8'd0;
        end else if (misr_clear) begin
            for (k = 0; k < NPC; k++) rcvd[k] <= 8'd0;
        end else if (busy) begin
            for (k = 0; k < NPC; k++)
                if (arr_valid_out[k]) rcvd[k] <= rcvd[k] + 8'd1;
        end
    end

    // -------------------------------------------------------------------------
    // 评估组合逻辑: 跳过已知坏列, 找最低故障物理列
    // -------------------------------------------------------------------------
    logic       all_received;
    logic       eval_pass;
    logic [4:0] eval_col;

    always_comb begin
        all_received = 1'b1;
        eval_pass    = 1'b1;
        eval_col     = 5'd0;
        for (int j = 0; j < NPC; j++) begin
            if (rcvd[j] < MB[7:0]) all_received = 1'b0;
            if (j != skip_col) begin
                if (rcvd[j] != MB[7:0] || misr_state[j] !== gold_rom[j]) begin
                    if (eval_pass) eval_col = j[4:0];
                    eval_pass = 1'b0;
                end
            end
        end
    end

    // -------------------------------------------------------------------------
    // 主状态机
    // -------------------------------------------------------------------------
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state      <= S_IDLE;
            busy       <= 1'b0;
            done       <= 1'b0;
            pass       <= 1'b0;
            fail_col   <= 5'd0;
            row_cnt    <= 8'd0;
            drain_cnt  <= 8'd0;
            misr_clear <= 1'b0;
        end else begin
            misr_clear <= 1'b0;

            case (state)
                S_IDLE: begin
                    done <= 1'b0;
                    if (start) begin
                        busy       <= 1'b1;
                        misr_clear <= 1'b1;
                        row_cnt    <= 8'd0;
                        drain_cnt  <= 8'd0;
                        state      <= S_LOAD;
                    end
                end

                S_LOAD: begin
                    state <= S_RUN;   // bist_load_w 本拍组合拉高, PE 锁存权重
                end

                S_RUN: begin
                    if (row_cnt == MB[7:0] - 8'd1) begin
                        state <= S_COLLECT;
                    end else begin
                        row_cnt <= row_cnt + 8'd1;
                    end
                end

                S_COLLECT: begin
                    drain_cnt <= drain_cnt + 8'd1;
                    if (all_received || drain_cnt == DRAIN_MAX[7:0]) begin
                        state <= S_EVAL;
                    end
                end

                S_EVAL: begin
                    pass     <= eval_pass;
                    fail_col <= eval_col;
                    busy     <= 1'b0;
                    done     <= 1'b1;
                    state    <= S_IDLE;
                end

                default: state <= S_IDLE;
            endcase
        end
    end

endmodule

``n
---

## 文件: rtl/abft_checksum.sv

``systemverilog
`timescale 1ns / 1ps

// =============================================================================
// abft_checksum.sv — 内循环 ABFT 校验和检测 (Phase 1: 双循环自愈内圈轻量检)
//
// 功能: 对 N 个逻辑列输出求和, 再与第 N 物理列 (校验列) 的输出比较,
//       |Σ c_logic - N·c_chk| > ABFT_THRES 时拉高 error_pulse (1 拍脉冲)。
//
// 关键设计:
//   1) 斜切对齐 (Skew Compensation): 脉动阵列第 c 列波前比校验列 (物理第 N 列)
//      早 (N-c) 拍 (见 systolic_array_spec.md §4.2), 故列 c 前置 (N-c) 拍移位
//      寄存器, 使同一矩阵行 m 的 N 个逻辑列与校验列在同一拍汇聚。
//   2) 求和树 (Sum Tree): 汇聚后以 1 级流水线寄存器锁存 Σc_logic, 阻断 N=32
//      规模下多级加法组合级联破坏关键路径时序; 校验列数据/有效位同拍寄存。
//   3) 定点安全: 校验列权重取"行均值" Σw[r][:]/N (见 self_healing_array 权重重
//      映射), 故校验列输出 c_chk ≈ (Σc_out)/N; 本模块以 N·c_chk 还原"理论列和",
//      避免 Q1.15 下权重求和溢出 16 位 (详见交付说明第 3 节的取舍备注)。
// =============================================================================

module abft_checksum #(
    parameter int N          = 4,        // 逻辑列数 (校验均值按 $clog2(N) 移位, N 为 2 的幂)
    parameter int ABFT_THRES = 16'h0080  // 容限阈值 (Q1.15 ULP, 正幅值, 可调)
) (
    input  logic               clk,
    input  logic               rst_n,
    input  logic               enable,       // 健康态 ABFT 使能 (故障重映射 / BIST 期间关闭)
    input  logic [N-1:0]       valid,        // N 个逻辑列有效位 (斜切相位)
    input  logic [N*16-1:0]    c_flat,       // N 个逻辑列输出 C[m][c] (斜切相位, Q1.15)
    input  logic               chk_valid,    // 校验列 (物理第 N 列) 有效位 (最晚到达)
    input  logic signed [15:0] c_chk,        // 校验列输出 (≈ 行均值)
    output logic               error_pulse,  // 1 拍脉冲, 与校验列拍对齐 (检测到超限)
    output logic signed [15:0] err_abs       // |Σc_logic - N·c_chk| 饱和幅值 (调试观测)
);

    localparam int SUM_W = 16 + $clog2(N+1) + 1;

    // -------------------------------------------------------------------------
    // 斜切补偿: 逻辑列 c 的数据与有效位均延迟 (N-c) 拍, 与校验列同拍汇聚
    // -------------------------------------------------------------------------
    logic signed [15:0] align_data [N];
    logic               align_valid_c [N];
    genvar c;
    generate
        for (c = 0; c < N; c++) begin : g_align
            localparam int DLY = N - c;
            logic signed [15:0] sr [0:DLY-1];   // 恰好 DLY=(N-c) 级, 与 zero_mask 延时约定一致
            logic               vr [0:DLY-1];
            integer i;
            always_ff @(posedge clk or negedge rst_n) begin
                if (!rst_n) begin
                    for (i = 0; i < DLY; i++) begin
                        sr[i] <= '0;
                        vr[i] <= 1'b0;
                    end
                end else begin
                    sr[0] <= $signed(c_flat[16*c +: 16]);
                    vr[0] <= valid[c];
                    for (i = 1; i < DLY; i++) begin
                        sr[i] <= sr[i-1];
                        vr[i] <= vr[i-1];
                    end
                end
            end
            assign align_data[c]    = sr[DLY-1];
            assign align_valid_c[c] = vr[DLY-1];
        end
    endgenerate

    // 汇聚拍 = 校验列有效拍, 且全部逻辑列对齐有效 (防御性 &&)
    logic all_aligned;
    always_comb begin
        all_aligned = 1'b1;
        for (int k = 0; k < N; k++) all_aligned = all_aligned & align_valid_c[k];
    end

    // -------------------------------------------------------------------------
    // 求和树 + 1 级流水线寄存
    // -------------------------------------------------------------------------
    logic signed [SUM_W-1:0] sum_comb;
    logic signed [SUM_W-1:0] sum_r;
    logic signed [15:0]      chk_r;
    logic                    vld_r;
    logic                    sat_comb;
    logic                    sat_r;

    always_comb begin
        sum_comb = '0;
        for (int k = 0; k < N; k++)
            sum_comb = sum_comb + $signed(align_data[k]);
    end

    // 饱和检测: 任一逻辑列/校验列输出到达饱和轨 (0x7FFF/0x8000) 时, RNE+SAT
    // 的非线性破坏 ABFT 线性等价关系 (ΣC = N·c_chk), 该拍不作判读, 避免对健康
    // 饱和行的确定性误报 (peer_review P3-1)。
    always_comb begin
        sat_comb = 1'b0;
        for (int k = 0; k < N; k++)
            if (align_data[k] == 16'sh7FFF || align_data[k] == 16'sh8000)
                sat_comb = 1'b1;
        if (c_chk == 16'sh7FFF || c_chk == 16'sh8000)
            sat_comb = 1'b1;
    end

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            sum_r <= '0;
            chk_r <= '0;
            vld_r <= 1'b0;
            sat_r <= 1'b0;
        end else begin
            sum_r   <= sum_comb;
            chk_r   <= c_chk;
            vld_r   <= (enable & chk_valid & all_aligned);
            sat_r   <= sat_comb;
        end
    end

    // -------------------------------------------------------------------------
    // 比较: |Σc_logic - N·c_chk| > ABFT_THRES ?
    // -------------------------------------------------------------------------
    logic signed [SUM_W-1:0] chk_scaled;
    logic signed [SUM_W-1:0] diff;
    logic signed [SUM_W-1:0] abs_diff;

    always_comb begin
        // N·c_chk 还原"理论列和" (校验列权重为行均值 Σw/N)
        chk_scaled = '0;
        for (int k = 0; k < N; k++)
            chk_scaled = chk_scaled + $signed({{(SUM_W-16){chk_r[15]}}, chk_r});

        diff     = sum_r - chk_scaled;
        abs_diff = (diff < 0) ? -diff : diff;

        error_pulse = vld_r && !sat_r && (abs_diff > $signed({{(SUM_W-16){1'b0}}, ABFT_THRES[15:0]}));
        err_abs     = (abs_diff > 16'sd32767) ? 16'sd32767 : abs_diff[15:0];
    end

endmodule
``n
---

## 文件: rtl/zero_mask.sv

``systemverilog
`timescale 1ns / 1ps

// =============================================================================
// zero_mask.sv — 列出口零掩蔽单元 (优雅降级)
//
// 位于 rne_sat16 之后 (即脉动阵列列出口之后), 对每列独立作 DLY 拍移位寄存,
// 使各列的同一矩阵行输出与 ABFT 检测生成的 error_pulse 严格同拍汇聚;
// mask 有效拍将该列输出置零 (零掩蔽), valid 随数据同步移位。
//
//   out_valid = in_valid 延迟 DLY 拍 (1:1 前馈, 不丢失/不凭空产生)
//   out_data  = mask ? 0 : (in_data 延迟 DLY 拍)   (掩蔽与 valid 同拍生效)
//
// DLY 兼作跨列斜切均衡: 实例 c 使用 DLY = (N - c + 1), 使全部列对齐到
// 校验列检测拍 (m+2N+2), 从而 error_pulse 可对"整拍污染"广播置零。
// =============================================================================

module zero_mask #(
    parameter int DLY = 1   // 输出延迟拍数 (>= 1), 兼作斜切均衡深度
) (
    input  logic               clk,
    input  logic               rst_n,
    input  logic               in_valid,
    input  logic signed [15:0] in_data,
    input  logic               mask,       // 有效拍置零使能 (与输出拍对齐)
    output logic               out_valid,
    output logic signed [15:0] out_data
);

    logic signed [15:0] sr [0:DLY-1];
    logic               vr [0:DLY-1];
    integer i;

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            for (i = 0; i < DLY; i++) begin
                sr[i] <= '0;
                vr[i] <= 1'b0;
            end
        end else begin
            sr[0] <= in_data;
            vr[0] <= in_valid;
            for (i = 1; i < DLY; i++) begin
                sr[i] <= sr[i-1];
                vr[i] <= vr[i-1];
            end
        end
    end

    // 组合掩蔽: 掩蔽与输出拍同拍生效 (不在数据通路上追加寄存器)
    assign out_valid = vr[DLY-1];
    assign out_data  = mask ? 16'sd0 : sr[DLY-1];

endmodule
``n
---

## 文件: rtl/self_healing_array.sv

``systemverilog
`timescale 1ns / 1ps

// =============================================================================
// self_healing_array.sv — 带 BIST 自检与备用列热替换的自愈脉动阵列
//
// 结构: N 逻辑列映射到 N+1 物理列 (systolic_array #(.NR(N), .NC(N+1)))。
// 自愈闭环:
//   heal_start -> BIST 全物理列扫描 -> 发现故障物理列 f
//   -> 重映射: 逻辑列 c<f 用物理列 c, c>=f 用物理列 c+1 (物理列 f 被旁路)
//   -> 确认 BIST (跳过已知坏列) -> healed=1
//
// 权重重映射 (组合): 物理列 p 装载逻辑列 L(p) = (p<=fault_col) ? p : p-1
// 输出重映射 (组合): 逻辑列 c 取物理列 phi(c) = (c<fault_col) ? c : c+1
// 约束: 支持单物理列故障自愈; 二次故障置 unhealable。
// 注意: BIST 会覆盖驻留权重, 自愈完成后任务权重必须重新加载。
// =============================================================================

module self_healing_array #(
    parameter int N  = 4,    // 逻辑列数 (= 行数)
    parameter int MB = 16    // BIST 行数 (与 golden_bist.py 一致)
) (
    input  logic              clk,
    input  logic              rst_n,
    // 任务模式接口 (逻辑列视图)
    input  logic              load_w,
    input  logic [N*N*16-1:0] w_flat,
    input  logic              valid_in,
    input  logic [N*16-1:0]   a_flat,
    output logic [N-1:0]      valid_out,
    output logic [N*16-1:0]   c_flat,
    // 自愈控制/状态
    input  logic              heal_start,     // 脉冲: 启动 自检->修复->复验
    output logic              busy,           // 自愈流程进行中
    output logic              fault_detected, // 曾检测到故障
    output logic              healed,         // 故障已通过重映射隔离
    output logic              unhealable,     // 二次故障, 备用列耗尽
    output logic [4:0]        fault_col       // 故障物理列号 (N+1 表示无)
);

    localparam int NPC      = N + 1;
    localparam int NO_FAULT = NPC;   // fault_col_r 的"无故障"取值

    // -------------------------------------------------------------------------
    // 自愈 FSM
    // -------------------------------------------------------------------------
    typedef enum logic [1:0] {H_IDLE, H_BIST1, H_BIST2} hstate_t;
    hstate_t hstate;

    logic [4:0] fault_col_r;
    logic       bist_start;
    logic       bist_busy, bist_done, bist_pass;
    logic [4:0] bist_fail_col;

    // -------------------------------------------------------------------------
    // 物理阵列 (N 行 x N+1 列)
    // -------------------------------------------------------------------------
    logic                  phy_load_w;
    logic [N*NPC*16-1:0]   phy_w_flat;
    logic                  phy_valid_in;
    logic [N*16-1:0]       phy_a_flat;
    logic [NPC-1:0]        phy_valid_out;
    logic [NPC*16-1:0]     phy_c_flat;

    // -------------------------------------------------------------------------
    // Phase 1 内循环: ABFT 校验列 (时分复用) + 零掩蔽
    // -------------------------------------------------------------------------
    logic               abft_enable;            // 健康态 & 非 BIST 使能
    logic               abft_err;               // u_abft.error_pulse (整拍污染脉冲)
    logic signed [15:0] abft_abs;               // u_abft.err_abs (调试观测)
    logic signed [15:0] chk_w [N];              // 每行校验权重 = 行均值 ΣW/N
    logic [N-1:0]       raw_valid_out;          // 重映射后的逻辑列原始有效位
    logic [N*16-1:0]    raw_c_flat;             // 重映射后的逻辑列原始输出

    systolic_array #(.NR(N), .NC(NPC)) u_phy (
        .clk       (clk),
        .rst_n     (rst_n),
        .load_w    (phy_load_w),
        .w_flat    (phy_w_flat),
        .valid_in  (phy_valid_in),
        .a_flat    (phy_a_flat),
        .valid_out (phy_valid_out),
        .c_flat    (phy_c_flat)
    );

    // -------------------------------------------------------------------------
    // BIST 控制器
    // -------------------------------------------------------------------------
    logic                  bist_load_w;
    logic [N*NPC*16-1:0]   bist_w_flat;
    logic                  bist_valid;
    logic [N*16-1:0]       bist_a_flat;

    bist_controller #(.N(N), .MB(MB)) u_bist (
        .clk           (clk),
        .rst_n         (rst_n),
        .start         (bist_start),
        .skip_col      (fault_col_r),
        .bist_load_w   (bist_load_w),
        .bist_w_flat   (bist_w_flat),
        .bist_valid    (bist_valid),
        .bist_a_flat   (bist_a_flat),
        .arr_valid_out (phy_valid_out),
        .arr_c_flat    (phy_c_flat),
        .busy          (bist_busy),
        .done          (bist_done),
        .pass          (bist_pass),
        .fail_col      (bist_fail_col)
    );

    // -------------------------------------------------------------------------
    // 阵列输入复用: BIST 期间阵列由 BIST 控制器独占
    // -------------------------------------------------------------------------
    logic bist_active;
    assign bist_active = bist_busy;

    assign phy_load_w   = bist_active ? bist_load_w : load_w;
    assign phy_valid_in = bist_active ? bist_valid  : valid_in;
    assign phy_a_flat   = bist_active ? bist_a_flat : a_flat;

    // -------------------------------------------------------------------------
    // 权重重映射: 逻辑 N x N -> 物理 N x (N+1)
    //   健康态 (无故障): 物理第 N 列复用为 ABFT 校验列, 装载行均值权重;
    //   故障态:         按热备用逻辑切回为替代列。
    // -------------------------------------------------------------------------
    logic [N*NPC*16-1:0] w_phys;

    // 行均值校验权重 (N 为 2 的幂 => /N 即算术右移 $clog2(N) 位)
    always_comb begin
        for (int r = 0; r < N; r++) begin
            logic signed [16+$clog2(N)-1:0] row_sum;
            row_sum = '0;
            for (int c = 0; c < N; c++)
                row_sum = row_sum + $signed(w_flat[16*(r*N+c) +: 16]);
            chk_w[r] = row_sum >>> $clog2(N);
        end
    end

    always_comb begin
        for (int p = 0; p < NPC; p++) begin
            for (int r = 0; r < N; r++) begin
                if (fault_col_r == NO_FAULT[4:0] && p == N) begin
                    w_phys[16*(r*NPC+p) +: 16] = chk_w[r];   // 备用列复用为校验列
                end else begin
                    int l;
                    if (p < fault_col_r)       l = p;       // 故障列左侧: 直通
                    else if (p == fault_col_r) l = p;       // 故障列本身: 死列, 值任意
                    else                       l = p - 1;   // 故障列右侧: 左移一列
                    if (l > N-1) l = N-1;                   // 无故障时备用列复制末列
                    w_phys[16*(r*NPC+p) +: 16] = w_flat[16*(r*N+l) +: 16];
                end
            end
        end
    end

    assign phy_w_flat = bist_active ? bist_w_flat : w_phys;

    // -------------------------------------------------------------------------
    // 输出重映射: 物理 N+1 列 -> 逻辑 N 列 (结果先落 raw_* 供 ABFT + 零掩蔽接力)
    // -------------------------------------------------------------------------
    always_comb begin
        for (int c = 0; c < N; c++) begin
            int pc;
            pc = (c < fault_col_r) ? c : c + 1;
            raw_valid_out[c]       = phy_valid_out[pc];
            raw_c_flat[16*c +: 16] = phy_c_flat[16*pc +: 16];
        end
    end

    // -------------------------------------------------------------------------
    // Phase 1 内循环串联: ABFT 检测 (消耗物理第 N 列作校验列) + 零掩蔽
    // -------------------------------------------------------------------------
    // ABFT 使能: 无故障 (校验列可用) 且非 BIST (阵列由任务独占)
    assign abft_enable = (fault_col_r == NO_FAULT[4:0]) && !busy;

    abft_checksum #(.N(N)) u_abft (
        .clk        (clk),
        .rst_n      (rst_n),
        .enable     (abft_enable),
        .valid      (raw_valid_out),
        .c_flat     (raw_c_flat),
        .chk_valid  (phy_valid_out[N]),
        .c_chk      (phy_c_flat[16*N +: 16]),
        .error_pulse(abft_err),
        .err_abs    (abft_abs)
    );

    generate
        for (genvar c = 0; c < N; c++) begin : g_mask
            zero_mask #(.DLY(N - c + 1)) u_zm (
                .clk       (clk),
                .rst_n     (rst_n),
                .in_valid  (raw_valid_out[c]),
                .in_data   (raw_c_flat[16*c +: 16]),
                .mask      (abft_err),
                .out_valid (valid_out[c]),
                .out_data  (c_flat[16*c +: 16])
            );
        end
    endgenerate

    // -------------------------------------------------------------------------
    // 自愈主流程: BIST1 -> (发现故障则重映射) -> BIST2 复验
    // -------------------------------------------------------------------------
    assign busy      = (hstate != H_IDLE);
    assign fault_col = fault_col_r;

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            hstate         <= H_IDLE;
            fault_col_r    <= NO_FAULT[4:0];
            fault_detected <= 1'b0;
            healed         <= 1'b0;
            unhealable     <= 1'b0;
            bist_start     <= 1'b0;
        end else begin
            bist_start <= 1'b0;

            case (hstate)
                H_IDLE: begin
                    if (heal_start) begin
                        bist_start <= 1'b1;
                        hstate     <= H_BIST1;
                    end
                end

                H_BIST1: begin
                    if (bist_done) begin
                        if (bist_pass) begin
                            hstate <= H_IDLE;             // 全列健康
                        end else if (fault_col_r != NO_FAULT[4:0]) begin
                            unhealable <= 1'b1;           // 备用列已耗尽
                            hstate     <= H_IDLE;
                        end else begin
                            fault_col_r    <= bist_fail_col;
                            fault_detected <= 1'b1;
                            bist_start     <= 1'b1;       // 复验 (跳过已标记坏列)
                            hstate         <= H_BIST2;
                        end
                    end
                end

                H_BIST2: begin
                    if (bist_done) begin
                        if (bist_pass) healed <= 1'b1;    // 修复确认
                        else           unhealable <= 1'b1;
                        hstate <= H_IDLE;
                    end
                end

                default: hstate <= H_IDLE;
            endcase
        end
    end

endmodule

``n
---


