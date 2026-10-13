`timescale 1ns / 1ps

// =============================================================================
// axi_stream_array.sv — AXI4-Stream 算力核 IP 封装 (axi_stream_array_spec.md)
//
// 封装 self_healing_array: 64 位 s_axis 复用 {激活行, 权重字, 命令},
// 列错峰结果经 N 个重组 FIFO 对齐为整行 64 位 m_axis 输出拍。
// 信用节流 (in_flight) 保证内部无反压阵列不溢出; BIST 期间拒绝任务流量。
// =============================================================================

module axi_stream_array #(
    parameter int N  = 4,        // 逻辑列数 (= 行数)
    parameter int MB = 16,       // BIST 行数 (与 golden_bist.py 一致)
    parameter int FD = 32        // 每列重组 FIFO 深度
) (
    input  logic        clk,
    input  logic        rst_n,
    // 上行 (宿主 -> IP)
    input  logic [63:0] s_axis_tdata,
    input  logic [1:0]  s_axis_tuser,
    input  logic        s_axis_tvalid,
    output logic        s_axis_tready,
    input  logic        s_axis_tlast,
    // 下行 (IP -> 宿主)
    output logic [63:0] m_axis_tdata,
    output logic        m_axis_tvalid,
    input  logic        m_axis_tready,
    output logic        m_axis_tlast,
    // 自愈状态侧带 (生产环境映射 AXI4-Lite CSR)
    output logic        sh_busy,
    output logic        fault_detected,
    output logic        healed,
    output logic        unhealable,
    output logic [4:0]  fault_col
);

    localparam logic [1:0] TU_ACT = 2'b00;
    localparam logic [1:0] TU_W   = 2'b01;
    localparam logic [1:0] TU_CMD = 2'b10;
    localparam logic [7:0] CREDIT_MAX8 = 8'(FD - N - 2);
    localparam logic [7:0] W_LAST      = 8'(N * N - 1);

    // -------------------------------------------------------------------------
    // 内核 (自愈脉动阵列)
    // -------------------------------------------------------------------------
    logic              core_load_w;
    logic [N*N*16-1:0] core_w_flat;
    logic              core_valid_in;
    logic [N*16-1:0]   core_a_flat;
    logic              core_heal_start;
    logic [N-1:0]      core_valid_out;
    logic [N*16-1:0]   core_c_flat;

    self_healing_array #(.N(N), .MB(MB)) u_core (
        .clk            (clk),
        .rst_n          (rst_n),
        .load_w         (core_load_w),
        .w_flat         (core_w_flat),
        .valid_in       (core_valid_in),
        .a_flat         (core_a_flat),
        .valid_out      (core_valid_out),
        .c_flat         (core_c_flat),
        .heal_start     (core_heal_start),
        .busy           (sh_busy),
        .fault_detected (fault_detected),
        .healed         (healed),
        .unhealable     (unhealable),
        .fault_col      (fault_col)
    );

    // -------------------------------------------------------------------------
    // 上行解码与信用节流 (Spec 3.1/3.2)
    // -------------------------------------------------------------------------
    logic        beat;
    logic        is_act, is_w, is_cmd;
    logic [7:0]  in_flight;

    assign is_act = (s_axis_tuser == TU_ACT);
    assign is_w   = (s_axis_tuser == TU_W);
    assign is_cmd = (s_axis_tuser == TU_CMD);
    assign beat   = s_axis_tvalid && s_axis_tready;

    always_comb begin
        if (!rst_n) begin
            s_axis_tready = 1'b0;
        end else begin
            case (s_axis_tuser)
                TU_ACT:  s_axis_tready = !sh_busy && (in_flight < CREDIT_MAX8);
                TU_W:    s_axis_tready = !sh_busy && (in_flight == 8'd0);
                TU_CMD:  s_axis_tready = !sh_busy && (in_flight == 8'd0);
                default: s_axis_tready = 1'b0;
            endcase
        end
    end

    // 激活行直通内核 (同拍采样)
    assign core_valid_in  = beat && is_act;
    assign core_a_flat    = s_axis_tdata[N*16-1:0];
    // 命令拍: bit0 = heal_start 脉冲
    assign core_heal_start = beat && is_cmd && s_axis_tdata[0];

    // -------------------------------------------------------------------------
    // 权重移位装配: 收满 N*N 拍后自动装载 (Spec 2.1)
    // -------------------------------------------------------------------------
    logic [N*N*16-1:0] w_asm;
    logic [7:0]        w_cnt;

    assign core_w_flat = w_asm;

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            w_cnt        <= 8'd0;
            core_load_w  <= 1'b0;
        end else begin
            core_load_w <= 1'b0;
            if (beat && is_w) begin
                w_asm[16*w_cnt +: 16] <= s_axis_tdata[15:0];
                if (w_cnt == W_LAST) begin
                    w_cnt       <= 8'd0;
                    core_load_w <= 1'b1;
                end else begin
                    w_cnt <= w_cnt + 8'd1;
                end
            end
        end
    end

    // -------------------------------------------------------------------------
    // 矩阵行计数: 输入 tlast 锁存本矩阵行数
    // -------------------------------------------------------------------------
    logic [15:0] rows_acc;   // 当前矩阵已接受行数
    logic [15:0] m_rows;     // tlast 时锁存的总行数

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            rows_acc <= 16'd0;
            m_rows   <= 16'd0;
        end else if (beat && is_act) begin
            if (s_axis_tlast) begin
                rows_acc <= 16'd0;
                m_rows   <= rows_acc + 16'd1;
            end else begin
                rows_acc <= rows_acc + 16'd1;
            end
        end
    end

    // -------------------------------------------------------------------------
    // 信用计数: 已接受行 - 已出行
    // -------------------------------------------------------------------------
    logic out_beat;
    assign out_beat = m_axis_tvalid && m_axis_tready;

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) in_flight <= 8'd0;
        else case ({(beat && is_act), out_beat})
            2'b10:   in_flight <= in_flight + 8'd1;
            2'b01:   in_flight <= in_flight - 8'd1;
            default: in_flight <= in_flight;
        endcase
    end

    // -------------------------------------------------------------------------
    // 每列重组 FIFO (Spec 2.2): 列错峰结果对齐为整行
    // BIST 期间 (sh_busy) 内核输出被门控, 不进入 FIFO
    // -------------------------------------------------------------------------
    logic [15:0] fifo [N][FD];
    logic [5:0]  wp [N];
    logic [5:0]  rp [N];

    logic col_has_data [N];
    logic all_cols_ready;

    always_comb begin
        all_cols_ready = 1'b1;
        for (int c = 0; c < N; c++) begin
            col_has_data[c] = (wp[c] != rp[c]);
            if (!col_has_data[c]) all_cols_ready = 1'b0;
        end
    end

    genvar c;
    generate
        for (c = 0; c < N; c++) begin : g_fifo
            always_ff @(posedge clk or negedge rst_n) begin
                if (!rst_n) begin
                    wp[c] <= 6'd0;
                    rp[c] <= 6'd0;
                end else begin
                    if (core_valid_out[c] && !sh_busy) begin
                        fifo[c][wp[c][4:0]] <= core_c_flat[16*c +: 16];
                        wp[c] <= wp[c] + 6'd1;
                    end
                    if (out_beat) begin
                        rp[c] <= rp[c] + 6'd1;
                    end
                end
            end
            assign m_axis_tdata[16*c +: 16] = fifo[c][rp[c][4:0]];
        end
    endgenerate

    // -------------------------------------------------------------------------
    // 下行输出拍
    // -------------------------------------------------------------------------
    logic [15:0] out_cnt;

    assign m_axis_tvalid = all_cols_ready;
    assign m_axis_tlast  = (m_rows != 16'd0) && (out_cnt == m_rows - 16'd1);

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            out_cnt <= 16'd0;
        end else if (out_beat) begin
            if (m_axis_tlast) out_cnt <= 16'd0;
            else              out_cnt <= out_cnt + 16'd1;
        end
    end

endmodule
