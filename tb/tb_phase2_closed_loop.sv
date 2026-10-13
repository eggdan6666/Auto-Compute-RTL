`timescale 1ns/1ps

// =============================================================================
// tb_phase2_closed_loop.sv — Phase 2 外循环闭环 + MTTR 量化验证
//
// 场景 A (瞬态软错误): 注入 1 拍错误, 验证 zero_mask 掩蔽、transient_cnt 递增、
//                     busy 恒 0 (SVA 保证不停机, 不误触发 BIST)。
// 场景 B (永久硬故障): 注入持续卡死故障, 验证 fault_arbiter 自主拉起 BIST、
//                     热备用列替换, 打印并验证 detect_latency / repair_latency /
//                     total_mttr; 自愈后复测任务输出与黄金一致。
// 结果自动追加至 sim/experiment_ledger.csv。
//
// 任务数据采用自造小幅 Q1.15 幂2 幅值 (2^11..2^13), 保证全流程不饱和,
// 使 ABFT 线性等价关系在每一拍都成立、任意卡死故障均被可靠检出。
// =============================================================================

module tb_phase2_closed_loop;

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
    // Phase 2 测量端口
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

    // ------------------------------------------------------------------ 数据
    logic signed [15:0] W    [0:N-1][0:N-1];
    logic signed [15:0] A    [0:M-1][0:N-1];
    logic signed [15:0] EXP  [0:M-1][0:N-1];
    logic [15:0]        wcode [0:N*N-1];
    logic [N*16-1:0]    acode [0:M-1];
    logic [N*16-1:0]    ecode [0:M-1];

    function automatic logic signed [15:0] p2a(input int k);
        case (k % 4)
            0: p2a = 16'sh1000;   // +2^12
            1: p2a = 16'sh2000;   // +2^13
            2: p2a = 16'sh0800;   // +2^11
            3: p2a = -16'sh1000;  // -2^12
            default: p2a = 16'sh1000;
        endcase
    endfunction

    integer      mi, ri, ci;
    longint signed acc_l;

    initial begin
        for (ri = 0; ri < N; ri++)
            for (ci = 0; ci < N; ci++)
                W[ri][ci] = p2a(ri + ci);
        for (mi = 0; mi < M; mi++)
            for (ri = 0; ri < N; ri++)
                A[mi][ri] = p2a(mi + ri + 1);

        // 黄金输出: acc = Σ_r A*W 为 Q2.30 整数; 幂2 幅值 => 低 15 位为 0, >>15 精确
        for (mi = 0; mi < M; mi++)
            for (ci = 0; ci < N; ci++) begin
                acc_l = 0;
                for (ri = 0; ri < N; ri++)
                    acc_l = acc_l + longint'( $signed(A[mi][ri]) ) * longint'( $signed(W[ri][ci]) );
                acc_l = acc_l >>> 15;
                if (acc_l > 32767)       acc_l = 32767;
                else if (acc_l < -32768) acc_l = -32768;
                EXP[mi][ci] = acc_l[15:0];
            end

        for (ri = 0; ri < N; ri++)
            for (ci = 0; ci < N; ci++)
                wcode[ri*N + ci] = W[ri][ci];
        for (mi = 0; mi < M; mi++) begin
            acode[mi] = '0;
            ecode[mi] = '0;
            for (ri = 0; ri < N; ri++) acode[mi][16*ri +: 16] = A[mi][ri];
            for (ci = 0; ci < N; ci++) ecode[mi][16*ci +: 16] = EXP[mi][ci];
        end

        $display("Phase2 任务数据 W[0][:]=%h %h %h %h | EXP[0][:]=%h %h %h %h",
                 W[0][0], W[0][1], W[0][2], W[0][3],
                 EXP[0][0], EXP[0][1], EXP[0][2], EXP[0][3]);
    end

    initial begin
        clk = 0;
        forever #(CLK_PERIOD / 2.0) clk = ~clk;
    end

    // ------------------------------------------------------------ 复位 / 装载
    task automatic apply_reset();
        @(negedge clk);
        rst_n = 0; load_w = 0; valid_in = 0; heal_start = 0;
        a_flat = '0; w_flat = '0;
        repeat(2) @(negedge clk);
        rst_n = 1;
        @(negedge clk);
    endtask

    task automatic load_weights();
        @(negedge clk);
        for (int i = 0; i < N*N; i++) w_flat[16*i +: 16] = wcode[i];
        load_w = 1;
        @(negedge clk);
        load_w = 0; w_flat = '0;
    endtask

    // -------------------------------------------- 掩蔽事件计数 (error_pulse 时整拍置零)
    integer mask_total, mask_ok;
    always @(posedge clk) begin
        if (dut.u_abft.error_pulse) begin
            mask_total = mask_total + 1;
            if ((&dut.valid_out) === 1'b1 && dut.c_flat === '0)
                mask_ok = mask_ok + 1;
        end
    end

    // -------------------------------------------- SVA: 瞬态场景期间 busy 恒 0
    logic scen_a_active;
    logic busy_violation;

    // 工具无关的等价监控 (iverilog 可直接编译运行)
    always @(posedge clk) begin
        if (scen_a_active && rst_n && busy === 1'b1)
            busy_violation <= 1'b1;
    end

    // 并发 SVA 断言 (供 Verilator 动态断言仿真; iverilog 无并发断言故跳过)
`ifndef __ICARUS__
    property p_no_halt;
        @(posedge clk) disable iff (!rst_n)
        scen_a_active |-> (busy === 1'b0);
    endproperty
    assert property (p_no_halt)
        else $error("[SVA-A] 瞬态场景误触发 BIST (busy 拉高) t=%0t", $time);
`endif

    // ------------------------------------------------------------ 结果统计
    integer tests, passes, fails;
    task automatic check(input string name, input logic cond);
        tests = tests + 1;
        if (cond) begin
            passes = passes + 1;
            $display("[PASS] %s", name);
        end else begin
            fails = fails + 1;
            $error("[FAIL] %s", name);
        end
    endtask

    // ------------------------------------------------------------ CSV 追加
    task automatic csv_append(input string scen, input int inj_col, input int inj_row,
                              input logic [31:0] tc, input logic [15:0] dl,
                              input logic [15:0] rl, input logic [15:0] tm,
                              input logic hld, input logic [4:0] fc, input logic ok);
        integer fchk, fh, fd;
        fchk = $fopen("sim/experiment_ledger.csv", "r");
        if (fchk == 0) begin
            fh = $fopen("sim/experiment_ledger.csv", "w");
            $fdisplay(fh, "scenario,inj_phys_col,inj_phys_row,transient_cnt,detect_latency,repair_latency,total_mttr,healed,fault_col,pass");
            $fclose(fh);
        end else begin
            $fclose(fchk);
        end
        fd = $fopen("sim/experiment_ledger.csv", "a");
        $fdisplay(fd, "%s,%0d,%0d,%0d,%0d,%0d,%0d,%0d,%0d,%0d",
                  scen, inj_col, inj_row, tc, dl, rl, tm, hld, fc, ok);
        $fclose(fd);
    endtask

    // ------------------------------------------------------------ 场景 A
    task automatic scenario_A();
        integer tc_before, delta;
        $display("\n=== 场景 A: 瞬态软错误 (1 拍注入) ===");
        apply_reset();
        mask_total = 0; mask_ok = 0;
        scen_a_active = 1;
        load_weights();
        tc_before = transient_cnt;

        fork
            begin : drv_a
                for (int m = 0; m < M; m++) begin
                    @(negedge clk);
                    a_flat = acode[m];
                    valid_in = 1;
                end
                @(negedge clk);
                valid_in = 0; a_flat = '0;
            end
            begin : inj_a
                repeat(3) @(negedge clk);
                force dut.u_phy.g_row[0].g_pe[0].u_pe.psum_out = 40'h0;
                repeat(1) @(negedge clk);
                release dut.u_phy.g_row[0].g_pe[0].u_pe.psum_out;
            end
        join

        repeat(20) @(negedge clk);   // 排空
        scen_a_active = 0;

        delta = transient_cnt - tc_before;
        check("A.1 busy 恒 0 (不误停机)", busy_violation === 1'b0);
        check("A.2 transient_cnt 递增", delta >= 1);
        check("A.3 零掩蔽生效 (err 拍 c_flat 全零)", mask_total >= 1 && mask_ok == mask_total);
        check("A.4 未产生 fault_detected", fault_detected === 1'b0);
        check("A.5 未拉高 healed", healed === 1'b0);
        $display("  [INFO] transient_cnt 增量 = %0d (掩蔽事件 %0d/%0d 正确)", delta, mask_ok, mask_total);
        csv_append("A_transient", 0, 0, delta, 0, 0, 0, 1'b0, 5'd5,
                   (delta >= 1) && (busy === 1'b0));
    endtask

    // ------------------------------------------------------------ 场景 B
    task automatic scenario_B();
        int t;
        int errs_post;
        $display("\n=== 场景 B: 永久硬故障 (自主闭环 + 热替换) ===");
        apply_reset();
        // 持续卡死故障 @ 物理列 1, 行 1 的 PE 部分和 (stuck-at-0)
        force dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out = 40'h0;
        load_weights();

        fork
            begin : drv_b
                for (int m = 0; m < M; m++) begin
                    @(negedge clk);
                    a_flat = acode[m];
                    valid_in = 1;
                end
                @(negedge clk);
                valid_in = 0; a_flat = '0;
            end
            begin : wait_heal
                t = 0;
                while (!healed && t < 4000) begin @(negedge clk); t = t + 1; end
            end
        join
        repeat(2) @(negedge clk);

        check("B.1 自主拉起 BIST 完成 (healed=1)", healed === 1'b1);
        check("B.2 定位故障物理列 col=1", fault_col === 5'd1);
        check("B.3 未耗尽备用列", unhealable === 1'b0);
        check("B.4 detect_latency > 0", detect_latency > 0);
        check("B.5 repair_latency > 0", repair_latency > 0);
        check("B.6 total_mttr == detect + repair", total_mttr === (detect_latency + repair_latency));

        $display("  [MTTR] detect_latency = %0d cycles", detect_latency);
        $display("  [MTTR] repair_latency = %0d cycles", repair_latency);
        $display("  [MTTR] total_mttr     = %0d cycles", total_mttr);

        // 自愈后复测: 故障列被旁路, 任务输出必须与黄金一致
        load_weights();
        errs_post = 0;
        fork
            begin : drv_b2
                for (int m = 0; m < M; m++) begin
                    @(negedge clk);
                    a_flat = acode[m];
                    valid_in = 1;
                end
                @(negedge clk);
                valid_in = 0; a_flat = '0;
            end
            begin : chk_b2
                int cnt[N];
                int timeout;
                for (int c = 0; c < N; c++) cnt[c] = 0;
                timeout = 0;
                while ((cnt[0] < M || cnt[1] < M || cnt[2] < M || cnt[3] < M) && timeout < 400) begin
                    @(negedge clk);
                    timeout = timeout + 1;
                    for (int c = 0; c < N; c++) begin
                        if (valid_out[c] === 1'b1) begin
                            if (c_flat[16*c +: 16] !== ecode[cnt[c]][16*c +: 16]) begin
                                errs_post = errs_post + 1;
                                $display("  [MISMATCH] m=%0d c=%0d Got=%h Exp=%h valid_out=%b", cnt[c], c, c_flat[16*c +: 16], ecode[cnt[c]][16*c +: 16], valid_out);
                            end
                            cnt[c] = cnt[c] + 1;
                        end
                    end
                end
                if (timeout >= 400) begin
                    errs_post = errs_post + 999;
                    $display("  [TIMEOUT] cnt=%0d,%0d,%0d,%0d", cnt[0], cnt[1], cnt[2], cnt[3]);
                end
            end
        join
        check("B.7 自愈后任务输出与黄金一致", errs_post === 0);

        release dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out;
        csv_append("B_hardfault", 1, 1, transient_cnt, detect_latency,
                   repair_latency, total_mttr, healed, fault_col,
                   (healed === 1'b1) && (fault_col === 5'd1) &&
                   (total_mttr === (detect_latency + repair_latency)) && (errs_post === 0));
    endtask

    initial begin
        tests = 0; passes = 0; fails = 0;
        mask_total = 0; mask_ok = 0;
        scen_a_active = 0;
        busy_violation = 0;
        rst_n = 1; valid_in = 0; load_w = 0; heal_start = 0;
        a_flat = '0; w_flat = '0;

        $display("===============================================================");
        $display("Phase 2 外循环闭环 + MTTR 量化验证 (N=%0d)", N);
        $display("===============================================================");

        scenario_A();
        scenario_B();

        $display("\n===============================================================");
        $display("Phase 2 汇总: Total=%0d Pass=%0d Fail=%0d", tests, passes, fails);
        $display("===============================================================");
        if (fails == 0) $display(">>> PHASE 2 CLOSED-LOOP TEST PASSED <<<\n");
        else            $display(">>> PHASE 2 TEST FAILED <<<\n");
        $finish;
    end

endmodule