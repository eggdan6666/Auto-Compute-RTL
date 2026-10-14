`timescale 1ns/1ps
// 变体测试: 不同 force/release 时序下, 下游实际采到几个脏拍
module tb_force_probe2;
    logic clk, rst_n;
    logic signed [39:0] psum_out;
    logic signed [39:0] psum_q;
    logic signed [39:0] s41;
    int variant;

    assign s41 = psum_q + 40'sd1;

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            psum_q   <= 40'sd1000;
            psum_out <= 40'sd1000;
        end else begin
            psum_q   <= s41;
            psum_out <= s41;
        end
    end

    // 下游消费者: 每个 posedge 采样 psum_out
    logic signed [39:0] sampled;
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) sampled <= 40'sd0;
        else        sampled <= psum_out;
    end

    initial begin clk = 0; forever #5 clk = ~clk; end

    int n;
    always @(posedge clk) begin
        n <= n + 1;
        $display("  t=%0t posedge#%0d: psum_out(pre)=%0d sampled_q=%0d", $time, n, psum_out, sampled);
    end

    initial begin
        n = 0; variant = 0;
        if (!$value$plusargs("V=%d", variant)) variant = 1;
        rst_n = 1;
        @(negedge clk); rst_n = 0;
        @(negedge clk); rst_n = 1;
        repeat (5) @(negedge clk);
        $display(">>> variant %0d: FORCE at negedge t=%0t", variant, $time);
        force psum_out = 40'sd0;
        if (variant == 1) begin
            // V1: release 在下一个 negedge (现行 sweep TB 模式)
            repeat (1) @(negedge clk);
            $display(">>> variant 1: RELEASE at negedge t=%0t", $time);
            release psum_out;
        end else if (variant == 2) begin
            // V2: release 在 posedge 当拍 (active region)
            @(posedge clk);
            $display(">>> variant 2: RELEASE at posedge t=%0t", $time);
            release psum_out;
        end else if (variant == 3) begin
            // V3: release 在 posedge 后 #1
            @(posedge clk);
            #1;
            $display(">>> variant 3: RELEASE at posedge+1 t=%0t", $time);
            release psum_out;
        end
        repeat (4) @(negedge clk);
        $display(">>> done (variant %0d)", variant);
        $finish;
    end
endmodule
