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
