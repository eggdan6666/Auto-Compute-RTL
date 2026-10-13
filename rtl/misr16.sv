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
