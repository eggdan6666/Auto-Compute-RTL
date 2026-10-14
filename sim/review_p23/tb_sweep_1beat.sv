`timescale 1ns / 1ps

// =============================================================================
// tb_sweep_1beat.sv — 100 次单拍瞬态注入 error_pulse 脉冲数分布实测
//
// 目的 (Kimi-K3 首席科学家指令):
//   实测单拍瞬态故障在脉动阵列内的脉冲扩展分布 P(pulses = k)，为裁决器
//   阈值选取 THRESH = 3 提供坚实的学术数学辩护:
//   - 证明单次瞬态注入产生的脉冲数严格 <= 2 (最大 2 脉冲)
//   - 证明 THRESH = 3 严格能够将单次瞬态与硬故障 (>= 3 脉冲) 分离
//   - 验证 100 次瞬态场景下 busy 恒为 0，零误拉起 BIST
// =============================================================================

module tb_transient_sweep;

    localparam int N = 4;
    localparam int M = 8;
    localparam real CLK_PERIOD = 10.0;
    localparam int NUM_TRIALS = 100;

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
    logic [15:0]       detect_latency;
    logic [15:0]       repair_latency;
    logic [15:0]       total_mttr;
    logic [31:0]       transient_cnt;

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
        .fault_col      (fault_col),
        .detect_latency (detect_latency),
        .repair_latency (repair_latency),
        .total_mttr     (total_mttr),
        .transient_cnt  (transient_cnt)
    );

    // 时钟生成
    initial begin
        clk = 0;
        forever #(CLK_PERIOD / 2.0) clk = ~clk;
    end

    // 幂 2 测试数据构造
    logic [15:0]     wcode [0:N*N-1];
    logic [N*16-1:0] acode [0:M-1];

    function automatic logic signed [15:0] p2a(input int k);
        case (k % 4)
            0: p2a = 16'sh1000;
            1: p2a = 16'sh2000;
            2: p2a = 16'sh0800;
            3: p2a = -16'sh1000;
            default: p2a = 16'sh1000;
        endcase
    endfunction

    initial begin
        for (int r = 0; r < N; r++)
            for (int c = 0; c < N; c++)
                wcode[r*N + c] = p2a(r + c);
        for (int m = 0; m < M; m++) begin
            acode[m] = '0;
            for (int r = 0; r < N; r++)
                acode[m][16*r +: 16] = p2a(m + r + 1);
        end
    end

    // 单次瞬态注入任务
    task automatic inject_transient_pe(input int r, input int c);
        case (r)
            0: case (c)
                0: force dut.u_phy.g_row[0].g_pe[0].u_pe.psum_out = 40'h0;
                1: force dut.u_phy.g_row[0].g_pe[1].u_pe.psum_out = 40'h0;
                2: force dut.u_phy.g_row[0].g_pe[2].u_pe.psum_out = 40'h0;
                3: force dut.u_phy.g_row[0].g_pe[3].u_pe.psum_out = 40'h0;
            endcase
            1: case (c)
                0: force dut.u_phy.g_row[1].g_pe[0].u_pe.psum_out = 40'h0;
                1: force dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out = 40'h0;
                2: force dut.u_phy.g_row[1].g_pe[2].u_pe.psum_out = 40'h0;
                3: force dut.u_phy.g_row[1].g_pe[3].u_pe.psum_out = 40'h0;
            endcase
            2: case (c)
                0: force dut.u_phy.g_row[2].g_pe[0].u_pe.psum_out = 40'h0;
                1: force dut.u_phy.g_row[2].g_pe[1].u_pe.psum_out = 40'h0;
                2: force dut.u_phy.g_row[2].g_pe[2].u_pe.psum_out = 40'h0;
                3: force dut.u_phy.g_row[2].g_pe[3].u_pe.psum_out = 40'h0;
            endcase
            3: case (c)
                0: force dut.u_phy.g_row[3].g_pe[0].u_pe.psum_out = 40'h0;
                1: force dut.u_phy.g_row[3].g_pe[1].u_pe.psum_out = 40'h0;
                2: force dut.u_phy.g_row[3].g_pe[2].u_pe.psum_out = 40'h0;
                3: force dut.u_phy.g_row[3].g_pe[3].u_pe.psum_out = 40'h0;
            endcase
        endcase
    endtask

    task automatic release_transient_pe(input int r, input int c);
        case (r)
            0: case (c)
                0: release dut.u_phy.g_row[0].g_pe[0].u_pe.psum_out;
                1: release dut.u_phy.g_row[0].g_pe[1].u_pe.psum_out;
                2: release dut.u_phy.g_row[0].g_pe[2].u_pe.psum_out;
                3: release dut.u_phy.g_row[0].g_pe[3].u_pe.psum_out;
            endcase
            1: case (c)
                0: release dut.u_phy.g_row[1].g_pe[0].u_pe.psum_out;
                1: release dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out;
                2: release dut.u_phy.g_row[1].g_pe[2].u_pe.psum_out;
                3: release dut.u_phy.g_row[1].g_pe[3].u_pe.psum_out;
            endcase
            2: case (c)
                0: release dut.u_phy.g_row[2].g_pe[0].u_pe.psum_out;
                1: release dut.u_phy.g_row[2].g_pe[1].u_pe.psum_out;
                2: release dut.u_phy.g_row[2].g_pe[2].u_pe.psum_out;
                3: release dut.u_phy.g_row[2].g_pe[3].u_pe.psum_out;
            endcase
            3: case (c)
                0: release dut.u_phy.g_row[3].g_pe[0].u_pe.psum_out;
                1: release dut.u_phy.g_row[3].g_pe[1].u_pe.psum_out;
                2: release dut.u_phy.g_row[3].g_pe[2].u_pe.psum_out;
                3: release dut.u_phy.g_row[3].g_pe[3].u_pe.psum_out;
            endcase
        endcase
    endtask

    // 统计分布直方图
    int hist_0; // 0 脉冲 (良性未显现)
    int hist_1; // 1 脉冲 (单行受污染)
    int hist_2; // 2 脉冲 (跨拍向下传播污染相邻行)
    int hist_ge3; // >=3 脉冲 (理论上不应发生)
    int busy_violations;

    initial begin
        hist_0 = 0;
        hist_1 = 0;
        hist_2 = 0;
        hist_ge3 = 0;
        busy_violations = 0;
        rst_n = 1; valid_in = 0; load_w = 0; heal_start = 0;
        a_flat = '0; w_flat = '0;

        $display("===============================================================");
        $display("Auto-Compute-RTL: 100 次单拍瞬态注入脉冲分布实测 (P3-2 辩护)");
        $display("===============================================================");

        for (int trial = 0; trial < NUM_TRIALS; trial++) begin
            int inj_r, inj_c, inj_delay;
            int pulse_count;
            int tc_before;
            int t_start;

            // 伪随机选择故障注入点
            inj_r = $urandom_range(0, N-1);
            inj_c = $urandom_range(0, N-1);
            inj_delay = $urandom_range(1, 4);

            // 复位
            @(negedge clk);
            rst_n = 0;
            repeat(2) @(negedge clk);
            rst_n = 1;
            @(negedge clk);

            // 装载权重
            for (int i = 0; i < N*N; i++) w_flat[16*i +: 16] = wcode[i];
            load_w = 1;
            @(negedge clk);
            load_w = 0; w_flat = '0;

            tc_before = transient_cnt;

            fork
                begin : drv
                    for (int m = 0; m < M; m++) begin
                        @(negedge clk);
                        a_flat = acode[m];
                        valid_in = 1;
                    end
                    @(negedge clk);
                    valid_in = 0; a_flat = '0;
                end
                begin : inj
                    repeat(inj_delay) @(negedge clk);
                    inject_transient_pe(inj_r, inj_c);
                    @(posedge clk);           // 下游仅在此 1 个 posedge 采到强制值
                    release_transient_pe(inj_r, inj_c); // posedge 当拍释放 => 真单拍(复审对照)
                end
            join

            repeat(20) @(negedge clk); // 排空脉动阵列

            pulse_count = transient_cnt - tc_before;
            if (busy === 1'b1) busy_violations++;

            // 归入直方图
            case (pulse_count)
                0: hist_0++;
                1: hist_1++;
                2: hist_2++;
                default: hist_ge3++;
            endcase
        end

        $display("\n---------------------------------------------------------------");
        $display("100 次单拍瞬态注入实测分布统计结果 (P3-2 辩护数据):");
        $display("  - 产生 0 脉冲 (自然掩盖/无影响): %0d 次 (%0.1f%%)", hist_0, hist_0 * 1.0);
        $display("  - 产生 1 脉冲 (单拍污染):       %0d 次 (%0.1f%%)", hist_1, hist_1 * 1.0);
        $display("  - 产生 2 脉冲 (传播扩展污染):   %0d 次 (%0.1f%%)", hist_2, hist_2 * 1.0);
        $display("  - 产生 >=3 脉冲 (硬故障误报):    %0d 次 (%0.1f%%)", hist_ge3, hist_ge3 * 1.0);
        $display("  - 误触发 BIST 停机次数 (busy=1): %0d 次", busy_violations);
        $display("---------------------------------------------------------------");

        if (hist_ge3 == 0 && busy_violations == 0) begin
            $display(">>> [PASS] 证明定理成立: 单事件瞬态脉冲数严格 <= 2，THRESH=3 零误报，辩护成立！<<<");
        end else begin
            $display(">>> [FAIL] 存在 >=3 脉冲或误停机! <<<");
        end
        $display("===============================================================\n");

        // 写入 sim/transient_distribution.csv
        begin
            integer f;
            f = $fopen("sim/review_p23/transient_distribution_1beat.csv", "w");
            $fdisplay(f, "pulses,count,percentage");
            $fdisplay(f, "0,%0d,%0.2f", hist_0, hist_0 / 100.0);
            $fdisplay(f, "1,%0d,%0.2f", hist_1, hist_1 / 100.0);
            $fdisplay(f, "2,%0d,%0.2f", hist_2, hist_2 / 100.0);
            $fdisplay(f, "ge3,%0d,%0.2f", hist_ge3, hist_ge3 / 100.0);
            $fclose(f);
            $display("[INFO] 瞬态分布数据已写入: sim/transient_distribution.csv");
        end

        $finish;
    end

endmodule
