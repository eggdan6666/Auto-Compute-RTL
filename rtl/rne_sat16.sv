`timescale 1ns / 1ps

// =============================================================================
// rne_sat16.sv — 脉动阵列列出口: 40 位部分和 -> RNE 舍入 -> 16 位饱和
// 与 mac_unit Stage 2 的舍入/饱和逻辑逐位等价 (MAC_microarchitecture_spec.md
// 4.2/5 节)。in_valid=0 时 out_data 保持上一有效值。
// =============================================================================

module rne_sat16 (
    input  logic               clk,
    input  logic               rst_n,
    input  logic               in_valid,
    input  logic signed [39:0] in_psum,
    output logic               out_valid,
    output logic signed [15:0] out_data
);

    logic g_bit, st_bit, l_bit, inc;
    logic signed [40:0] q_floor, q_round;
    logic pos_ovf, neg_ovf;
    logic signed [15:0] out_next;

    always_comb begin
        g_bit = in_psum[14];
        st_bit = |in_psum[13:0];
        l_bit = in_psum[15];
        inc = g_bit & (st_bit | l_bit);

        q_floor = 41'(in_psum >>> 15);
        q_round = q_floor + 41'(inc);

        pos_ovf = !q_round[40] & (|q_round[39:15]);
        neg_ovf = q_round[40] & ~(&q_round[39:15]);

        if (pos_ovf)      out_next = 16'sh7FFF;
        else if (neg_ovf) out_next = 16'sh8000;
        else              out_next = q_round[15:0];
    end

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            out_valid <= 1'b0;
            out_data  <= 16'h0000;
        end else begin
            out_valid <= in_valid;
            if (in_valid) out_data <= out_next;
        end
    end

endmodule
