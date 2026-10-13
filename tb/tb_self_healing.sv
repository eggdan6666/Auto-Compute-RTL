`timescale 1ns/1ps

// =============================================================================
// tb_self_healing.sv — 自愈脉动阵列闭环验证
//
// 验证序列:
//   1. 复位 -> 启动 BIST -> 期望全列健康 (fault_detected=0)
//   2. 任务模式: 装载逻辑权重, 流入 M 行激活, 比对黄金输出
//   3. 故障注入: force 物理列 2 某 PE 的 psum_out 为固定垃圾值
//   4. 再次 BIST -> 期望定位 fault_col=2, 重映射, 复验通过 (healed=1)
//   5. 任务模式复测 -> 逻辑结果必须与无故障时完全一致
//
// 激励/黄金: golden_bist.py (mission_weights.hex / mission_vectors.hex)
// BIST ROM/签名: bist_weights.hex / bist_rows.hex / bist_golden.hex
// =============================================================================

module tb_self_healing;

    localparam N  = 4;
    localparam M  = 8;
    localparam CLK_PERIOD = 10;

    logic              clk;
    logic              rst_n;
    logic              load_w;
    logic [N*N*16-1:0] w_flat;
    logic              valid_in;
    logic [N*16-1:0]   a_flat;
    logic [N-1:0]      valid_out;
    logic [N*16-1:0]   c_flat;
    logic              heal_start;
    logic              busy;
    logic              fault_detected;
    logic              healed;
    logic              unhealable;
    logic [4:0]        fault_col;

    int total_tests  = 0;
    int tests_passed = 0;
    int tests_failed = 0;

    self_healing_array #(.N(N)) dut (
        .clk            (clk),
        .rst_n          (rst_n),
        .load_w         (load_w),
        .w_flat         (w_flat),
        .valid_in       (valid_in),
        .a_flat         (a_flat),
        .valid_out      (valid_out),
        .c_flat         (c_flat),
        .heal_start     (heal_start),
        .busy           (busy),
        .fault_detected (fault_detected),
        .healed         (healed),
        .unhealable     (unhealable),
        .fault_col      (fault_col)
    );

    initial begin
        clk = 0;
        forever #(CLK_PERIOD / 2.0) clk = ~clk;
    end

    logic [15:0]       mw_mem  [0:N*N-1];
    logic [2*N*16-1:0] mv_mem  [0:M-1];

    task automatic apply_reset();
        @(negedge clk);
        rst_n = 0;
        valid_in = 0;
        load_w = 0;
        heal_start = 0;
        a_flat = '0;
        w_flat = '0;
        repeat(2) @(negedge clk);
        rst_n = 1;
        @(negedge clk);
    endtask

    task automatic check_flag(input string name, input logic cond);
        total_tests++;
        if (cond) begin
            $display("[PASS] %s", name);
            tests_passed++;
        end else begin
            $error("[FAIL] %s", name);
            tests_failed++;
        end
    endtask

    // 启动一次自愈流程并等待完成
    task automatic run_heal();
        @(negedge clk);
        heal_start = 1;
        @(negedge clk);
        heal_start = 0;
        begin : wait_done
            int t;
            t = 0;
            while (busy !== 1'b1 && t < 10) begin @(negedge clk); t++; end
            while (busy === 1'b1 && t < 500) begin @(negedge clk); t++; end
            if (t >= 500) begin
                total_tests++;
                tests_failed++;
                $error("[FAIL] heal timeout");
            end
        end
        @(negedge clk);
    endtask

    // 任务模式: 装载逻辑权重 + 流入 M 行 + 按逻辑列比对黄金
    task automatic mission_test(input string name);
        int cnt [N];
        int timeout;
        logic [15:0] exp_c;
        logic [15:0] act_c;

        $display("--- Mission: %s ---", name);

        @(negedge clk);
        for (int i = 0; i < N*N; i++) w_flat[16*i +: 16] = mw_mem[i];
        load_w = 1;
        @(negedge clk);
        load_w = 0;
        w_flat = '0;

        fork
            begin : drv
                for (int m = 0; m < M; m++) begin
                    @(negedge clk);
                    for (int r = 0; r < N; r++)
                        a_flat[16*r +: 16] = mv_mem[m][16*(N+r) +: 16];
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
                            exp_c = mv_mem[cnt[c]][16*c +: 16];
                            act_c = c_flat[16*c +: 16];
                            total_tests++;
                            if (act_c === exp_c) begin
                                tests_passed++;
                                $display("[PASS] %s col%0d row%0d | Out=%h", name, c, cnt[c], act_c);
                            end else begin
                                tests_failed++;
                                $error("[FAIL] %s col%0d row%0d | Expected=%h Got=%h",
                                       name, c, cnt[c], exp_c, act_c);
                            end
                            cnt[c]++;
                        end
                    end
                end
                if (timeout >= 200) begin
                    total_tests++;
                    tests_failed++;
                    $error("[FAIL] %s timeout cnt=[%0d %0d %0d %0d]", name, cnt[0], cnt[1], cnt[2], cnt[3]);
                end
            end
        join
        repeat(3) @(negedge clk);
    endtask

    initial begin
        rst_n = 1;
        valid_in = 0;
        load_w = 0;
        heal_start = 0;
        a_flat = '0;
        w_flat = '0;

        $display("\n========================================");
        $display("Self-Healing Systolic Array Testbench (N=%0d)", N);
        $display("========================================\n");

        $readmemh("mission_weights.hex", mw_mem);
        $readmemh("mission_vectors.hex", mv_mem);

        apply_reset();

        // 1. 上电自检: 期望无故障
        $display("--- Phase A: power-on BIST (expect healthy) ---");
        run_heal();
        check_flag("A.1 no fault detected",  fault_detected === 1'b0);
        check_flag("A.2 not healed",         healed === 1'b0);
        check_flag("A.3 not unhealable",     unhealable === 1'b0);

        // 2. 任务模式 (健康)
        mission_test("healthy");

        // 3. 故障注入: 物理列 2, 行 1 的 PE 部分和卡死
        $display("--- Phase B: inject stuck fault @ physical col 2 ---");
        force dut.u_phy.g_row[1].g_pe[2].u_pe.psum_out = 40'h12_3456_789A;
        repeat(2) @(negedge clk);

        // 4. 自检 -> 定位 -> 重映射 -> 复验
        run_heal();
        check_flag("B.1 fault detected",     fault_detected === 1'b1);
        check_flag("B.2 fault_col == 2",     fault_col === 5'd2);
        check_flag("B.3 healed",             healed === 1'b1);
        check_flag("B.4 not unhealable",     unhealable === 1'b0);

        // 5. 任务模式复测 (故障已被旁路, 逻辑结果必须不变)
        mission_test("healed");

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
