`timescale 1ns/1ps
// =============================================================================
// tb_mttr_poison2.sv — 回归 TB (v2): MTTR 测量毒化 + BIST 污染脆弱性
//
// 整合自 WorkBuddy 复审证据 (sim/review_p23/tb_mttr_poison2.sv), 作为 F1/F2
// 修复的回归门禁: 修复后三项毒化均须"未复现", 即本 TB 三场景全部 PASS。
//
// SCEN=1: detect 毒化 — 早期单个瞬态(不触发)使 first_err_seen 常置,
//         后续真实硬故障的 detect_latency 虚高。修复: 40 拍静默超时清哨。
// SCEN=2: repair 毒化 — 早期瞬态簇触发 BIST 且 BIST1 无故障通过(干净),
//         repair_running 卡死。修复: H_IDLE 且 !healed 时强制清哨。
// SCEN=3: BIST 污染脆弱性 (持续灌流自愈) — 任务流在自愈期间仍在飞,
//         排空数据污染 MISR。修复: !busy 入口门控 + S_DRAIN 排空等待。
// =============================================================================
module tb_mttr_poison2;
    localparam int N = 4;
    localparam int M = 8;
    localparam real CLK_PERIOD = 10.0;
    localparam int MAXB = 420;

    logic              clk, rst_n, load_w, valid_in, heal_start;
    logic [N*N*16-1:0] w_flat;
    logic [N*16-1:0]   a_flat;
    logic [N-1:0]      valid_out;
    logic [N*16-1:0]   c_flat;
    logic              busy, fault_detected, healed, unhealable;
    logic [4:0]        fault_col;
    logic [15:0]       detect_latency, repair_latency, total_mttr;
    logic [31:0]       transient_cnt;

    self_healing_array #(.N(N)) dut (
        .clk(clk), .rst_n(rst_n), .load_w(load_w), .w_flat(w_flat),
        .valid_in(valid_in), .a_flat(a_flat), .valid_out(valid_out), .c_flat(c_flat),
        .heal_start(heal_start), .busy(busy), .fault_detected(fault_detected),
        .healed(healed), .unhealable(unhealable), .fault_col(fault_col),
        .detect_latency(detect_latency), .repair_latency(repair_latency),
        .total_mttr(total_mttr), .transient_cnt(transient_cnt));

    initial begin clk = 0; forever #(CLK_PERIOD/2.0) clk = ~clk; end

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
                wcode[r*N+c] = p2a(r+c);
        for (int m = 0; m < M; m++) begin
            acode[m] = '0;
            for (int r = 0; r < N; r++)
                acode[m][16*r +: 16] = p2a(m+r+1);
        end
    end

    // ---------------------------------------------------------------- 调度器
    int beat;
    int sched [0:MAXB-1];
    int scen;

    always @(posedge clk) beat <= (beat < MAXB) ? beat + 1 : beat;

    task automatic set_feed(input int lo, input int hi);
        for (int b = lo; b < hi && b < MAXB; b++) sched[b] = 1;
    endtask

    task automatic wait_beat(input int b);
        while (beat < b) @(negedge clk);
    endtask

    // 驱动器: 按调度表灌流
    initial begin
        valid_in = 0; a_flat = '0;
        forever begin
            @(negedge clk);
            if (beat < MAXB && sched[beat] === 1) begin
                a_flat  = acode[beat % M];
                valid_in = 1'b1;
            end else begin
                valid_in = 1'b0;
            end
        end
    end

    // ---------------------------------------------------------------- 注入
    task automatic inject_1beat(input int r, input int c);
        case (r*4+c)
            0:  force dut.u_phy.g_row[0].g_pe[0].u_pe.psum_out = 40'h0;
            1:  force dut.u_phy.g_row[0].g_pe[1].u_pe.psum_out = 40'h0;
            2:  force dut.u_phy.g_row[0].g_pe[2].u_pe.psum_out = 40'h0;
            3:  force dut.u_phy.g_row[0].g_pe[3].u_pe.psum_out = 40'h0;
            4:  force dut.u_phy.g_row[1].g_pe[0].u_pe.psum_out = 40'h0;
            5:  force dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out = 40'h0;
            6:  force dut.u_phy.g_row[1].g_pe[2].u_pe.psum_out = 40'h0;
            7:  force dut.u_phy.g_row[1].g_pe[3].u_pe.psum_out = 40'h0;
            8:  force dut.u_phy.g_row[2].g_pe[0].u_pe.psum_out = 40'h0;
            9:  force dut.u_phy.g_row[2].g_pe[1].u_pe.psum_out = 40'h0;
            10: force dut.u_phy.g_row[2].g_pe[2].u_pe.psum_out = 40'h0;
            11: force dut.u_phy.g_row[2].g_pe[3].u_pe.psum_out = 40'h0;
            12: force dut.u_phy.g_row[3].g_pe[0].u_pe.psum_out = 40'h0;
            13: force dut.u_phy.g_row[3].g_pe[1].u_pe.psum_out = 40'h0;
            14: force dut.u_phy.g_row[3].g_pe[2].u_pe.psum_out = 40'h0;
            15: force dut.u_phy.g_row[3].g_pe[3].u_pe.psum_out = 40'h0;
        endcase
        @(posedge clk);
        case (r*4+c)
            0:  release dut.u_phy.g_row[0].g_pe[0].u_pe.psum_out;
            1:  release dut.u_phy.g_row[0].g_pe[1].u_pe.psum_out;
            2:  release dut.u_phy.g_row[0].g_pe[2].u_pe.psum_out;
            3:  release dut.u_phy.g_row[0].g_pe[3].u_pe.psum_out;
            4:  release dut.u_phy.g_row[1].g_pe[0].u_pe.psum_out;
            5:  release dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out;
            6:  release dut.u_phy.g_row[1].g_pe[2].u_pe.psum_out;
            7:  release dut.u_phy.g_row[1].g_pe[3].u_pe.psum_out;
            8:  release dut.u_phy.g_row[2].g_pe[0].u_pe.psum_out;
            9:  release dut.u_phy.g_row[2].g_pe[1].u_pe.psum_out;
            10: release dut.u_phy.g_row[2].g_pe[2].u_pe.psum_out;
            11: release dut.u_phy.g_row[2].g_pe[3].u_pe.psum_out;
            12: release dut.u_phy.g_row[3].g_pe[0].u_pe.psum_out;
            13: release dut.u_phy.g_row[3].g_pe[1].u_pe.psum_out;
            14: release dut.u_phy.g_row[3].g_pe[2].u_pe.psum_out;
            15: release dut.u_phy.g_row[3].g_pe[3].u_pe.psum_out;
        endcase
    endtask

    task automatic wait_heal_done(input int maxb);
        int t;
        t = 0;
        while (!healed && !unhealable && t < maxb) begin @(negedge clk); t = t + 1; end
        repeat(3) @(negedge clk);
    endtask

    task automatic show(input string tag);
        $display("[%0s] beat=%0d busy=%b healed=%b unheal=%b fault_col=%0d detect=%0d repair=%0d total=%0d tc=%0d | int: fes=%b dcnt=%0d prun=%b pcnt=%0d",
                 tag, beat, busy, healed, unhealable, fault_col,
                 detect_latency, repair_latency, total_mttr, transient_cnt,
                 dut.first_err_seen, dut.detect_cnt, dut.repair_running, dut.repair_cnt);
    endtask

    // ---------------------------------------------------------------- 主流程
    logic scen_pass;

    initial begin
        beat = 0; scen = 1; scen_pass = 1'b0;
        for (int b = 0; b < MAXB; b++) sched[b] = 0;
        if (!$value$plusargs("SCEN=%d", scen)) scen = 1;
        rst_n = 1; load_w = 0; heal_start = 0;
        w_flat = '0;

        @(negedge clk); rst_n = 0;
        repeat(2) @(negedge clk); rst_n = 1;
        @(negedge clk);
        for (int i = 0; i < N*N; i++) w_flat[16*i +: 16] = wcode[i];
        load_w = 1;
        @(negedge clk);
        load_w = 0; w_flat = '0;

        if (scen == 1) begin
            // ---------- SCEN 1: detect 毒化 ----------
            set_feed(10, 18);                 // 突发1: beat 10..17
            wait_beat(14);                    // 稳态中注入单瞬态
            inject_1beat(0, 0);
            repeat(16) @(negedge clk);        // 等脉冲计数
            show("S1-after-transient");
            wait_beat(200);                   // 长时间静默 (无灌流)
            show("S1-silent-200");
            set_feed(200, 208);               // 突发2 + 永久故障
            force dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out = 40'h0;
            wait_heal_done(200);
            show("S1-after-heal");
            if (detect_latency > 16'd10) begin
                scen_pass = 1'b0;
                $display("[S1] *** [BUG 证实] detect_latency=%0d (真值=2, 虚高 %0d 拍) ***",
                         detect_latency, detect_latency - 16'd2);
            end else begin
                scen_pass = 1'b1;
                $display("[S1] detect_latency=%0d (F1 修复生效, 未复现毒化)", detect_latency);
            end
            release dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out;
        end else if (scen == 2) begin
            // ---------- SCEN 2: repair 毒化 ----------
            set_feed(10, 18);                 // 突发1
            wait_beat(12); inject_1beat(0, 0);   // 瞬态簇 (同点错拍)
            wait_beat(14); inject_1beat(0, 0);
            wait_beat(16); inject_1beat(0, 0);
            repeat(30) @(negedge clk);
            show("S2-after-cluster");
            wait_beat(60);
            show("S2-after-bist1-pass");
            wait_beat(200);
            set_feed(200, 208);               // 突发2 + 永久故障
            force dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out = 40'h0;
            wait_heal_done(300);
            show("S2-after-heal");
            if (repair_latency > 16'd80) begin
                scen_pass = 1'b0;
                $display("[S2] *** [BUG 证实] repair_latency=%0d (真值~59, 虚高 %0d 拍) ***",
                         repair_latency, repair_latency - 16'd59);
            end else begin
                scen_pass = 1'b1;
                $display("[S2] repair_latency=%0d (F1 修复生效, 未复现毒化)", repair_latency);
            end
            release dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out;
        end else begin
            // ---------- SCEN 3: BIST 污染脆弱性 (持续灌流自愈) ----------
            set_feed(10, 40);                 // 持续灌流跨越整个自愈期
            wait_beat(12); inject_1beat(0, 0);   // 瞬态簇 (同点错拍)
            wait_beat(14); inject_1beat(0, 0);
            wait_beat(16); inject_1beat(0, 0);
            repeat(80) @(negedge clk);
            show("S3-after-cluster-heal");
            if (healed === 1'b1 && fault_col !== 5'd5 && unhealable === 1'b0) begin
                scen_pass = 1'b0;
                $display("[S3] *** [BUG 证实] 无真实故障却 healed=1, fault_col=%0d (健康列被永久旁路) ***", fault_col);
            end else if (unhealable === 1'b1) begin
                scen_pass = 1'b0;
                $display("[S3] *** [BUG 证实] 无真实故障却 unhealable=1 (BIST 被排空数据污染) ***");
            end else begin
                scen_pass = 1'b1;
                $display("[S3] F2 修复生效, 未复现污染 (healed=%b fc=%0d unheal=%b)", healed, fault_col, unhealable);
            end
        end

        repeat(20) @(negedge clk);
        if (scen_pass) $display(">>> SCEN=%0d PASS <<<", scen);
        else           $display(">>> SCEN=%0d FAIL <<<", scen);
        $finish;
    end
endmodule