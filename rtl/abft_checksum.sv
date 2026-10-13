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