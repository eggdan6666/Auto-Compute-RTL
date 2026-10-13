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
