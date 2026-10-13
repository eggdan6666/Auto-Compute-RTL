`timescale 1ns / 1ps

module mac_unit (
    input  logic        clk,
    input  logic        rst_n,
    input  logic        valid_in,
    input  logic signed [15:0] A,
    input  logic signed [15:0] B,
    input  logic signed [39:0] Acc_in,
    output logic signed [15:0] Out,
    output logic        Valid_out
);

    // =========================================================================
    // Stage 1 (PR1): Multiply + sign-extend + latch Acc_in
    // =========================================================================
    
    // Internal signals for Stage 1 combinational logic
    logic signed [31:0] p_s1;
    logic signed [39:0] p_ext_s1;
    
    // Pipeline registers for Stage 1
    logic signed [39:0] prod_ext_s1_reg;
    logic signed [39:0] acc_s1_reg;
    logic               valid_s1_reg;
    
    always_comb begin
        p_s1 = 32'(A) * 32'(B); // Explicit 32-bit signed context before multiply
        p_ext_s1 = {{8{p_s1[31]}}, p_s1[31:0]}; // Sign-extend to 40 bits
    end
    
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            prod_ext_s1_reg <= '0;
            acc_s1_reg      <= '0;
            valid_s1_reg    <= 1'b0;
        end else begin
            prod_ext_s1_reg <= p_ext_s1;
            acc_s1_reg      <= Acc_in;
            valid_s1_reg    <= valid_in;
        end
    end

    // =========================================================================
    // Stage 2 (PR2): Accumulate + RNE round + saturate
    // =========================================================================
    
    // Internal signals for Stage 2 combinational logic
    logic signed [40:0] s_41;
    logic signed [39:0] s_40;
    
    // Rounding signals
    logic g_bit;
    logic st_bit;
    logic l_bit;
    logic inc;
    logic signed [40:0] q_floor;
    logic signed [40:0] q_round;
    
    // Saturation signals
    logic pos_ovf;
    logic neg_ovf;
    logic signed [15:0] out_next;
    
    always_comb begin
        // 1. 41-bit signed addition: {acc_s1[39], acc_s1} + {prod_ext_s1[39], prod_ext_s1}
        s_41 = {acc_s1_reg[39], acc_s1_reg} + {prod_ext_s1_reg[39], prod_ext_s1_reg};
        
        // 2. 40-bit overflow clamp
        if (s_41[40:39] == 2'b01) begin
            s_40 = 40'sh7F_FFFF_FFFF;
        end else if (s_41[40:39] == 2'b10) begin
            s_40 = 40'sh80_0000_0000;
        end else begin
            s_40 = s_41[39:0];
        end
        
        // 3. Round-to-nearest-even (RNE) by right-shifting 15 bits
        g_bit = s_40[14];
        st_bit = |s_40[13:0];
        l_bit = s_40[15];
        inc = g_bit & (st_bit | l_bit);
        
        // Q_floor = sign_extend to 41 bits of S_40 >>> 15
        q_floor = 41'($signed(s_40) >>> 15);
        q_round = q_floor + 41'(inc);
        
        // 4. 16-bit saturation
        pos_ovf = !q_round[40] & (|q_round[39:15]);
        neg_ovf = q_round[40] & ~( &q_round[39:15] );
        
        if (pos_ovf) begin
            out_next = 16'sh7FFF;
        end else if (neg_ovf) begin
            out_next = 16'sh8000;
        end else begin
            out_next = q_round[15:0];
        end
    end
    
    // Output registers
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            Out <= 16'h0000;
            Valid_out <= 1'b0;
        end else begin
            Valid_out <= valid_s1_reg;
            // 5. & 6. Valid_out logic and holding previous result
            if (valid_s1_reg) begin
                Out <= out_next;
            end
        end
    end

endmodule
