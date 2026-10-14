`timescale 1ns/1ps
// 复审对照实验: RTL rne_sat16 逐拍灌入 rne_in.hex, 输出写 rne_out_rtl.hex
module tb_rne_check;
    localparam string IN_FILE  = "sim/review_p23/rne_in.hex";
    localparam string OUT_FILE = "sim/review_p23/rne_out_rtl.hex";

    logic clk, rst_n;
    logic in_valid;
    logic signed [39:0] psum_mem [0:4095];
    logic signed [39:0] in_psum;
    logic out_valid;
    logic signed [15:0] out_data;

    rne_sat16 dut (
        .clk(clk), .rst_n(rst_n),
        .in_valid(in_valid), .in_psum(in_psum),
        .out_valid(out_valid), .out_data(out_data));

    initial begin clk = 0; forever #5 clk = ~clk; end

    integer n, i, fout, wptr;
    logic signed [15:0] cap [0:4095];

    // 并发捕获: 仅 out_valid 拍录入
    initial begin
        wptr = 0;
        forever begin
            @(posedge clk);
            if (rst_n && out_valid && wptr < 4096) begin
                cap[wptr] = out_data;
                wptr = wptr + 1;
            end
        end
    end

    initial begin
        $readmemh(IN_FILE, psum_mem);
        n = 0;
        for (i = 0; i < 4096; i = i + 1)
            if (psum_mem[i] !== 40'bx) n = n + 1;
        $display("[rne-check] loaded %0d vectors", n);

        rst_n = 0; in_valid = 0; in_psum = '0;
        @(negedge clk); @(negedge clk); rst_n = 1;

        for (i = 0; i < n; i = i + 1) begin
            @(negedge clk);
            in_psum  = psum_mem[i];
            in_valid = 1'b1;
        end
        @(negedge clk);
        in_valid = 1'b0;
        repeat (4) @(posedge clk);   // 等最后 1 拍输出

        fout = $fopen(OUT_FILE, "w");
        for (i = 0; i < wptr; i = i + 1)
            $fdisplay(fout, "%04X", cap[i] & 16'hFFFF);
        $fclose(fout);
        $display("[rne-check] captured %0d outputs (expect %0d) -> %s", wptr, n, OUT_FILE);
        if (wptr != n) $display("[rne-check][FAIL] capture count mismatch");
        $finish;
    end
endmodule
