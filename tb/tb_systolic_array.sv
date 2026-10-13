`timescale 1ns/1ps

// =============================================================================
// tb_systolic_array.sv — N×N 权重固定脉动阵列 Testbench
// 激励与黄金预期全部由 golden_array.py 独立生成 ($readmemh):
//   array_weights.hex : S*N*N 行 16 bit
//   array_vectors.hex : S*M 行 2*N*16 bit {a[N-1..0], c_exp[N-1..0]}
// =============================================================================

module tb_systolic_array;

    localparam N = 4;   // 与 golden_array.py 的 N 一致
    localparam S = 6;   // 场景数
    localparam M = 8;   // 每场景激活行数
    localparam CLK_PERIOD = 10;

    logic                 clk;
    logic                 rst_n;
    logic                 load_w;
    logic [N*N*16-1:0]    w_flat;
    logic                 valid_in;
    logic [N*16-1:0]      a_flat;
    logic [N-1:0]         valid_out;
    logic [N*16-1:0]      c_flat;

    int total_tests  = 0;
    int tests_passed = 0;
    int tests_failed = 0;

    systolic_array #(.NR(N), .NC(N)) dut (
        .clk       (clk),
        .rst_n     (rst_n),
        .load_w    (load_w),
        .w_flat    (w_flat),
        .valid_in  (valid_in),
        .a_flat    (a_flat),
        .valid_out (valid_out),
        .c_flat    (c_flat)
    );

    initial begin
        clk = 0;
        forever #(CLK_PERIOD / 2.0) clk = ~clk;
    end

    logic [15:0]        w_mem   [0:S*N*N-1];
    logic [2*N*16-1:0]  vec_mem [0:S*M-1];

    task automatic apply_reset();
        @(negedge clk);
        rst_n = 0;
        valid_in = 0;
        load_w = 0;
        a_flat = '0;
        w_flat = '0;
        repeat(2) @(negedge clk);
        rst_n = 1;
        @(negedge clk);
    endtask

    // 单个场景: 加载权重 -> 流入 M 行激活 -> 按列独立收集并比对
    task automatic run_scenario(input int s, input string name);
        int base_w;
        int base_v;
        int cnt [N];
        int timeout;
        logic [15:0] exp_c;
        logic [15:0] act_c;

        base_w = s * N * N;
        base_v = s * M;

        $display("--- Scenario %0d: %s ---", s, name);

        // 1. 并行加载权重
        @(negedge clk);
        for (int i = 0; i < N*N; i++) w_flat[16*i +: 16] = w_mem[base_w + i];
        load_w = 1;
        @(negedge clk);
        load_w = 0;
        w_flat = '0;

        // 2. 流入激活 + 按列收集输出
        fork
            begin : drv
                for (int m = 0; m < M; m++) begin
                    @(negedge clk);
                    for (int r = 0; r < N; r++)
                        a_flat[16*r +: 16] = vec_mem[base_v + m][16*(N+r) +: 16];
                    valid_in = 1;
                end
                @(negedge clk);
                valid_in = 0;
                a_flat = '0;
            end
            begin : chk
                for (int c = 0; c < N; c++) cnt[c] = 0;
                timeout = 0;
                while ((cnt[0] < M || cnt[1] < M || cnt[2] < M || cnt[3] < M) && timeout < 200) begin
                    @(negedge clk);
                    timeout++;
                    for (int c = 0; c < N; c++) begin
                        if (valid_out[c] === 1'b1) begin
                            exp_c = vec_mem[base_v + cnt[c]][16*c +: 16];
                            act_c = c_flat[16*c +: 16];
                            total_tests++;
                            if (act_c === exp_c) begin
                                tests_passed++;
                                $display("[PASS] S%0d col%0d row%0d | Out=%h", s, c, cnt[c], act_c);
                            end else begin
                                tests_failed++;
                                $error("[FAIL] S%0d col%0d row%0d | Expected=%h Got=%h",
                                       s, c, cnt[c], exp_c, act_c);
                            end
                            cnt[c]++;
                        end
                    end
                end
                if (timeout >= 200) begin
                    $error("[FAIL] S%0d timeout: cnt=[%0d %0d %0d %0d]", s, cnt[0], cnt[1], cnt[2], cnt[3]);
                    tests_failed++;
                end
            end
        join

        // 3. 场景间排空
        repeat(3) @(negedge clk);
    endtask

    initial begin
        rst_n = 1;
        valid_in = 0;
        load_w = 0;
        a_flat = '0;
        w_flat = '0;

        $display("\n========================================");
        $display("Starting Systolic Array Testbench (N=%0d)", N);
        $display("========================================\n");

        $readmemh("array_weights.hex", w_mem);
        $readmemh("array_vectors.hex", vec_mem);

        apply_reset();

        // 复位测试: 输出有效位必须全 0
        total_tests++;
        if (valid_out === '0) begin
            $display("[PASS] Reset Test");
            tests_passed++;
        end else begin
            $error("[FAIL] Reset Test | valid_out=%b", valid_out);
            tests_failed++;
        end

        run_scenario(0, "near-identity");
        run_scenario(1, "all-max-pos saturate+");
        run_scenario(2, "(-1)x(-1) x4 guard-bits");
        run_scenario(3, "all-max-neg saturate-");
        run_scenario(4, "RNE midpoint odd/even");
        run_scenario(5, "random full-range");

        $display("\n========================================");
        $display("Test Summary:");
        $display("Total Checks: %0d", total_tests);
        $display("Passed      : %0d", tests_passed);
        $display("Failed      : %0d", tests_failed);
        $display("========================================");

        if (tests_failed == 0) $display(">>> ALL TESTS PASSED <<<\n");
        else $display(">>> SOME TESTS FAILED <<<\n");

        $finish;
    end

endmodule
