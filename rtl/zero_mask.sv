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