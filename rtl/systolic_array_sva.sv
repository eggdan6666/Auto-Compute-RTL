`timescale 1ns / 1ps

// =============================================================================
// systolic_array_sva.sv — 脉动阵列级形式化断言 (Phase 4 任务④)
//
// 列出口内部的保持/范围/复位由 rne_sat16_sva 逐实例覆盖; 本模块钉住
// *阵列级*波前时序契约 —— 此前只有黄金向量覆盖, 没有形式化契约:
//
//   W1: 列相位对齐 —— valid_out[c] 的每个上升沿恰好比 valid_out[c-1] 晚 1 拍
//       (斜切波前的固有性质; 斜切缓冲/互连出错会立即违反)
//   W2: 有效期间无 X 态 (扁平总线级)
//   W3: 复位期间 valid_out 全 0
// =============================================================================

module systolic_array_sva #(
    parameter int NR = 4,
    parameter int NC = 4
) (
    input logic              clk,
    input logic              rst_n,
    input logic              valid_in,
    input logic [NC-1:0]     valid_out,
    input logic [NC*16-1:0]  c_flat
);

    default clocking cb @(posedge clk); endclocking

    // W1: 列间相位差恰好 1 拍 (双向)
    generate
        for (genvar c = 1; c < NC; c++) begin : g_phase
            a_phase_fwd: assert property (
                disable iff (!rst_n)
                $rose(valid_out[c-1]) |-> ##1 $rose(valid_out[c]))
                else $error("[SVA-W1] 列 %0d 波前相位错误: 未跟随列 %0d (t=%0t)", c, c-1, $time);
            a_phase_back: assert property (
                disable iff (!rst_n || !$past(rst_n))
                $rose(valid_out[c]) |-> $past($rose(valid_out[c-1])))
                else $error("[SVA-W1b] 列 %0d 波前相位错误: 前列 %0d 无上升沿 (t=%0t)", c, c-1, $time);
        end
    endgenerate

    // W2: 任一列有效时, 对应输出切片无 X
    generate
        for (genvar c = 0; c < NC; c++) begin : g_no_x
            a_no_x: assert property (
                disable iff (!rst_n)
                valid_out[c] |-> !$isunknown(c_flat[16*c +: 16]))
                else $error("[SVA-W2] 列 %0d 输出含 X 态 (t=%0t)", c, $time);
        end
    endgenerate

    // W3: 复位期间 valid_out 全 0
    a_reset_valid: assert property (
        !rst_n |-> (valid_out === '0))
        else $error("[SVA-W3] 复位期间 valid_out 非零 (t=%0t)", $time);

    // 覆盖: 见证完整波前 (所有列同拍有效)
    c_full_wavefront: cover property (
        disable iff (!rst_n) &valid_out);

endmodule

bind systolic_array systolic_array_sva #(.NR(NR), .NC(NC)) u_systolic_array_sva (
    .clk       (clk),
    .rst_n     (rst_n),
    .valid_in  (valid_in),
    .valid_out (valid_out),
    .c_flat    (c_flat)
);
