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
