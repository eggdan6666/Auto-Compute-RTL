`timescale 1ns / 1ps

// =============================================================================
// self_healing_array_sva.sv — Phase 1 内循环 ABFT + 零掩蔽 形式化断言
//
// 两层 bind:
//   (A) self_healing_array_sva  绑定 self_healing_array —— 零掩蔽生效拍时序
//       对齐: error_pulse 与零掩蔽输出严格同拍, 掩蔽拍全列有效且 c_flat 全零。
//   (B) zero_mask_sva           绑定 zero_mask —— 无反压死锁: 零掩蔽为纯前馈
//       移位, valid 逐拍 1:1 传递 (不丢失、不凭空产生、绝不卡死)。
//
// 覆用约束 (前置需求):
//   self_healing_array.sv 内需实例化 abft_checksum (实例名 u_abft) 与
//   zero_mask (生成块 g_mask), 并声明内部信号 abft_enable / abft_err /
//   abft_abs (见交付说明第 3 节集成段)。
// =============================================================================

module self_healing_array_sva #(
    parameter int N = 4
) (
    input logic               clk,
    input logic               rst_n,
    input logic               abft_enable,   // 内部: ABFT 使能 (健康态 & 非 BIST)
    input logic               abft_err,      // 内部: u_abft.error_pulse
    input logic signed [15:0] abft_abs,      // 内部: u_abft.err_abs
    input logic [N-1:0]       valid_out,
    input logic [N*16-1:0]    c_flat
);

    default clocking cb @(posedge clk); endclocking

    // Z0: error_pulse 仅在 ABFT 使能期出现 (故障重映射 / BIST 期不得误报)
    a_err_gated: assert property (
        disable iff (!rst_n)
        abft_err |-> abft_enable)
        else $error("[SVA-Z0] abft_err 在非使能期被拉起 (t=%0t)", $time);

    // Z1: 零掩蔽生效 —— error_pulse 拍, 全列输出被置零 (整拍优雅降级)
    a_mask_zero: assert property (
        disable iff (!rst_n)
        abft_err |-> (c_flat === '0))
        else $error("[SVA-Z1] abft_err 拍 c_flat 未全零, 零掩蔽未生效 (t=%0t)", $time);

    // Z2: 零掩蔽与 valid 同拍对齐 —— error_pulse 只能出现在全列有效拍
    a_mask_on_valid: assert property (
        disable iff (!rst_n)
        abft_err |-> &valid_out)
        else $error("[SVA-Z2] abft_err 未与 valid_out 对齐 (t=%0t)", $time);

    // N1: 无反压死锁 (数据完整性) —— valid_out 有效时 c_flat 无 X
    generate
        for (genvar c = 0; c < N; c++) begin : g_nox
            a_nox: assert property (
                disable iff (!rst_n)
                valid_out[c] |-> !$isunknown(c_flat[16*c +: 16]))
                else $error("[SVA-N1] 列 %0d 输出含 X (t=%0t)", c, $time);
        end
    endgenerate

    // N2: 复位期间输出静默
    a_reset: assert property (
        !rst_n |-> (valid_out === '0))
        else $error("[SVA-N2] 复位期间 valid_out 非零 (t=%0t)", $time);

    // 覆盖: 见证一次零掩蔽与一次非零检测真的发生 (否则 Z1/Z2 形同虚设)
    c_mask:    cover property (disable iff (!rst_n) abft_err);
    c_err_obs: cover property (disable iff (!rst_n) abft_abs > 16'sd0);

endmodule

// bind self_healing_array self_healing_array_sva #(.N(N)) u_sha_sva (
//     .clk         (clk),
//     .rst_n       (rst_n),
//     .abft_enable (abft_enable),
//     .abft_err    (abft_err),
//     .abft_abs    (abft_abs),
//     .valid_out   (valid_out),
//     .c_flat      (c_flat)
// );

// -----------------------------------------------------------------------------
// (B) zero_mask 单元级: valid 前馈 1:1 恒等 (无反压死锁) + 掩蔽生效
// -----------------------------------------------------------------------------
module zero_mask_sva #(
    parameter int DLY = 1
) (
    input logic               clk,
    input logic               rst_n,
    input logic               in_valid,
    input logic               mask,
    input logic               out_valid,
    input logic signed [15:0] out_data
);

    default clocking cb @(posedge clk); endclocking

    // ZM1: 无反压死锁 —— out_valid 恒等于 DLY 拍前的 in_valid
    // (固定 DLY 拍后必然到达, 前馈通路不存在可卡死的握手机制)
    a_delay_exact: assert property (
        (rst_n && $past(rst_n, DLY)) |-> (out_valid === $past(in_valid, DLY)))
        else $error("[SVA-ZM1] zero_mask valid 传递错位 (DLY=%0d, t=%0t)", DLY, $time);

    // ZM2: 掩蔽生效 —— mask 有效拍输出必为 0
    a_mask: assert property (
        disable iff (!rst_n)
        (mask && out_valid) |-> (out_data === 16'sd0))
        else $error("[SVA-ZM2] zero_mask 掩蔽未置零 (t=%0t)", $time);

    // ZM3: 复位静默
    a_reset: assert property (
        !rst_n |-> !out_valid)
        else $error("[SVA-ZM3] zero_mask 复位期 valid 非零 (t=%0t)", $time);

endmodule

bind zero_mask zero_mask_sva #(.DLY(DLY)) u_zero_mask_sva (
    .clk       (clk),
    .rst_n     (rst_n),
    .in_valid  (in_valid),
    .mask      (mask),
    .out_valid (out_valid),
    .out_data  (out_data)
);