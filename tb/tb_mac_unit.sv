`timescale 1ns/1ps

module tb_mac_unit;

    // Parameters
    localparam CLK_PERIOD = 10;

    // Signals
    logic clk;
    logic rst_n;
    logic valid_in;
    logic signed [15:0] A;
    logic signed [15:0] B;
    logic signed [39:0] Acc_in;

    logic signed [15:0] Out;
    logic Valid_out;

    // Statistics
    int total_tests  = 0;
    int tests_passed = 0;
    int tests_failed = 0;

    // DUT Instantiation
    mac_unit uut (
        .clk(clk),
        .rst_n(rst_n),
        .valid_in(valid_in),
        .A(A),
        .B(B),
        .Acc_in(Acc_in),
        .Out(Out),
        .Valid_out(Valid_out)
    );

    // Clock Generation
    initial begin
        clk = 0;
        forever #(CLK_PERIOD / 2.0) clk = ~clk;
    end

    // Reference Model (Spec 4.1/4.2/5 完整实现: 41 位扩展加法 + 40 位钳位 + RNE + 饱和)
    function automatic logic signed [15:0] ref_model(
        input logic signed [15:0] a,
        input logic signed [15:0] b,
        input logic signed [39:0] acc
    );
        logic signed [31:0] prod;
        logic signed [40:0] sum41;
        logic signed [39:0] sum;
        logic G, L;
        logic St;
        logic inc;
        logic signed [40:0] shifted_sum;

        prod = a * b;

        // Spec 4.1: 41 位扩展加法，防止 40 位环绕
        sum41 = 41'(acc) + 41'(prod);

        // Spec 4.1: 40 位有符号范围钳位
        if (sum41[40:39] == 2'b01) begin
            sum = 40'sh7F_FFFF_FFFF;
        end else if (sum41[40:39] == 2'b10) begin
            sum = 40'sh80_0000_0000;
        end else begin
            sum = sum41[39:0];
        end

        // RNE Logic Extraction
        // Shift amount is 15. Discarded bits are [14:0].
        // Bit 14 is the Guard bit.
        // Bits 13:0 are the Sticky bits.
        // Bit 15 is the LSB of the remaining value.
        G = sum[14];
        St = (sum[13:0] != 0);
        L = sum[15];

        // Round to nearest even
        inc = G & (St | L);

        // Arithmetic shift right and apply rounding
        shifted_sum = 41'(sum >>> 15) + inc;
        
        // Saturation to 16 bits
        if (shifted_sum > 41'sh000007FFF) begin
            return 16'h7FFF;
        end else if (shifted_sum < -41'sh000008000) begin
            return 16'h8000;
        end else begin
            return 16'(shifted_sum);
        end
    endfunction

    // Checker Function
    function automatic void check_result(input string name, input logic signed [15:0] exp, input logic act_valid, input logic exp_valid);
        total_tests++;
        if (Out === exp && Valid_out === exp_valid) begin
            $display("[PASS] %s | Out=%h Valid=%b", name, Out, Valid_out);
            tests_passed++;
        end else begin
            $error("[FAIL] %s | Expected: Out=%h Valid=%b | Got: Out=%h Valid=%b", name, exp, exp_valid, Out, Valid_out);
            tests_failed++;
        end
    endfunction

    // Reset Task
    task automatic apply_reset();
        @(negedge clk);
        rst_n = 0;
        valid_in = 0;
        A = 0;
        B = 0;
        Acc_in = 0;
        repeat(2) @(negedge clk);
        rst_n = 1;
        @(negedge clk);
    endtask

    // Single Test Task
    task automatic run_single_test(
        input string name,
        input logic signed [15:0] a_in,
        input logic signed [15:0] b_in,
        input logic signed [39:0] acc
    );
        logic signed [15:0] exp;
        exp = ref_model(a_in, b_in, acc);
        
        @(negedge clk);
        A = a_in;
        B = b_in;
        Acc_in = acc;
        valid_in = 1;
        
        @(negedge clk);
        valid_in = 0;
        A = 'x; B = 'x; Acc_in = 'x; // To ensure pipeline isolation
        
        @(negedge clk);
        // At this point, 2 cycles have passed since valid_in=1 was sampled
        check_result(name, exp, Valid_out, 1'b1);
    endtask

    // Pipeline Back-to-Back Test Task
    task automatic test_pipeline();
        logic signed [15:0] a_arr[4]   = '{16'h4000, 16'hC000, 16'h2000, 16'hE000};
        logic signed [15:0] b_arr[4]   = '{16'h4000, 16'h4000, 16'h2000, 16'h2000};
        logic signed [39:0] acc_arr[4] = '{40'h0, 40'h0, 40'h0, 40'h0};
        logic signed [15:0] exp_arr[4];
        
        foreach(a_arr[i]) exp_arr[i] = ref_model(a_arr[i], b_arr[i], acc_arr[i]);
        
        fork
            begin : drv
                for(int i=0; i<4; i++) begin
                    @(negedge clk);
                    A = a_arr[i];
                    B = b_arr[i];
                    Acc_in = acc_arr[i];
                    valid_in = 1;
                end
                @(negedge clk);
                valid_in = 0;
                A = 'x; B = 'x; Acc_in = 'x;
            end
            begin : chk
                repeat(2) @(negedge clk);
                for(int i=0; i<4; i++) begin
                    @(negedge clk);
                    check_result($sformatf("9. Pipeline %0d", i), exp_arr[i], Valid_out, 1'b1);
                end
            end
        join
    endtask

    // Valid Gating Test Task
    task automatic test_valid_gating();
        logic signed [15:0] exp_prev;
        exp_prev = ref_model(16'h4000, 16'h4000, 40'h0);
        
        run_single_test("10. Valid Gating - Setup", 16'h4000, 16'h4000, 40'h0);
        
        @(negedge clk);
        A = 16'h7FFF; B = 16'h7FFF; Acc_in = 40'sh3FFFFFFFFF;
        valid_in = 0; 
        
        @(negedge clk);
        @(negedge clk);
        
        total_tests++;
        if (Out === exp_prev && Valid_out === 1'b0) begin
            $display("[PASS] 10. Valid Gating | Out held=%h", Out);
            tests_passed++;
        end else begin
            $error("[FAIL] 10. Valid Gating | Expected Out=%h Valid=0 | Got Out=%h Valid=%b", exp_prev, Out, Valid_out);
            tests_failed++;
        end
    endtask

    // Golden Oracle Vector Test (test_vectors.hex 由 golden_model.py 独立生成)
    task automatic test_golden_vectors();
        logic [87:0] vec_mem [0:14]; // {A[16], B[16], Acc_in[40], Exp[16]}
        logic signed [15:0] exp;

        $readmemh("test_vectors.hex", vec_mem);

        for (int n = 0; n < 15; n++) begin
            exp = vec_mem[n][15:0];

            @(negedge clk);
            A        = vec_mem[n][87:72];
            B        = vec_mem[n][71:56];
            Acc_in   = vec_mem[n][55:16];
            valid_in = 1;

            @(negedge clk);
            valid_in = 0;
            A = 'x; B = 'x; Acc_in = 'x;

            @(negedge clk);
            check_result($sformatf("20.%0d Golden Vector", n+1), exp, Valid_out, 1'b1);
        end
    endtask

    // Main Test Sequence
    initial begin
        // Initialize
        clk = 0;
        rst_n = 1;
        valid_in = 0;
        A = 0;
        B = 0;
        Acc_in = 0;

        $display("\n========================================");
        $display("Starting MAC Unit Testbench...");
        $display("========================================\n");
        
        apply_reset();
        
        // 1. Reset Test
        total_tests++;
        if (Out === 16'h0 && Valid_out === 1'b0) begin
            $display("[PASS] 1. Reset Test");
            tests_passed++;
        end else begin
            $error("[FAIL] 1. Reset Test | Out=%h Valid=%b", Out, Valid_out);
            tests_failed++;
        end

        // Basic operations
        run_single_test("2. Basic Multiply (0.5 * 0.5)", 16'h4000, 16'h4000, 40'h0);
        run_single_test("3. Negative Multiply (-0.5 * 0.5)", 16'hC000, 16'h4000, 40'h0);
        run_single_test("4. Accumulate Test (0.25 + 0.5)", 16'h4000, 16'h4000, 40'sh00_2000_0000);

        apply_reset();

        // Saturation & Limits
        run_single_test("5. Positive Saturation", 16'h7FFF, 16'h7FFF, 40'sh3F_FFFF_FFFF);
        run_single_test("6. Negative Saturation", 16'h8000, 16'h7FFF, 40'sh80_0000_0000);
        run_single_test("7. Special Case A=B=16'h8000", 16'h8000, 16'h8000, 40'h0);
        run_single_test("8. Zero Inputs", 16'h0, 16'h0, 40'h0);

        apply_reset();

        // Pipeline and Valid tracking
        test_pipeline();       // 9. Pipeline Back-to-Back
        test_valid_gating();   // 10. Valid Gating

        apply_reset();

        // Round-to-Nearest-Even (RNE) Cases
        run_single_test("11.1 RNE: G=1, St=0, L=0 (Round even down)", 16'h0, 16'h0, 40'h00_0001_4000);
        run_single_test("11.2 RNE: G=1, St=0, L=1 (Round even up)",   16'h0, 16'h0, 40'h00_0001_C000);
        run_single_test("11.3 RNE: G=1, St=1, L=0 (Round up)",        16'h0, 16'h0, 40'h00_0001_4001);

        apply_reset();

        // Independent Golden Oracle Vectors (generated by golden_model.py)
        test_golden_vectors();   // 20.x Golden Vectors x15

        // Summary
        $display("\n========================================");
        $display("Test Summary:");
        $display("Total Tests : %0d", total_tests);
        $display("Passed      : %0d", tests_passed);
        $display("Failed      : %0d", tests_failed);
        $display("========================================");
        
        if (tests_failed == 0) $display(">>> ALL TESTS PASSED <<<\n");
        else $display(">>> SOME TESTS FAILED <<<\n");
        
        $finish;
    end

endmodule
