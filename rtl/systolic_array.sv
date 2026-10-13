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
