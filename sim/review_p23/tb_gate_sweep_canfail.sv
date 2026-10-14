`timescale 1ns/1ps
// =============================================================================
// tb_gate_sweep_canfail.sv — Round 2 门控 G1c: 扫查测量链"可失败性"验证
//
// 目的 (测试必须能失败 — sim-verifier 铁律):
//   证明 tb_transient_sweep 的 pulse 测量链对"多拍注入"敏感。
//   若某日有人把注入机制改坏 (如注入完全无效), 原扫查会因 hist_2==0 而
//   "假通过"; 本门控以确定性的 2 拍注入要求测得 >=2 脉冲, 堵住该漏洞。
//
// 方法: 稳态任务流中, 对 3 个不同 PE 站点做确定性的 2 拍注入
//       (force@negedge, 覆盖 2 个 posedge 后 release), 每次注入后统计
//       transient_cnt 增量。任一站点的脉冲数 >=2 即证明测量链有效。
//
// 通过准则: [GATE] canfail_2beat: max_pulses>=2 -> PASS
// =============================================================================
module tb_gate_sweep_canfail;
    localparam int N = 4;
    localparam int M = 8;
    localparam real CLK_PERIOD = 10.0;
    localparam int MAXB = 300;

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

    int beat;
    int sched [0:MAXB-1];

    always @(posedge clk) beat <= (beat < MAXB) ? beat + 1 : beat;

    task automatic set_feed(input int lo, input int hi);
        for (int b = lo; b < hi && b < MAXB; b++) sched[b] = 1;
    endtask

    task automatic wait_beat(input int b);
        while (beat < b) @(negedge clk);
    endtask

    initial begin
        valid_in = 0; a_flat = '0;
        forever begin
            @(negedge clk);
            if (beat < MAXB && sched[beat] === 1) begin
                a_flat = acode[beat % M];
                valid_in = 1'b1;
            end else begin
                valid_in = 1'b0;
            end
        end
    end

    // 确定性 2 拍注入: force 覆盖 2 个 posedge (V1 模式, 复审已证实的双拍语义)
    task automatic inject_2beat(input int r, input int c);
        case (r*4+c)
            0:  force dut.u_phy.g_row[0].g_pe[0].u_pe.psum_out = 40'h0;
            5:  force dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out = 40'h0;
            10: force dut.u_phy.g_row[2].g_pe[2].u_pe.psum_out = 40'h0;
            default: $fatal(1, "canfail: 未支持的注入点");
        endcase
        repeat (2) @(negedge clk);   // 覆盖 2 个 posedge
        case (r*4+c)
            0:  release dut.u_phy.g_row[0].g_pe[0].u_pe.psum_out;
            5:  release dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out;
            10: release dut.u_phy.g_row[2].g_pe[2].u_pe.psum_out;
        endcase
    endtask

    int sites [0:2];
    int pulses [0:2];
    int tc_mark;
    int max_pulses;

    initial begin
        beat = 0;
        for (int b = 0; b < MAXB; b++) sched[b] = 0;
        sites[0] = 0; sites[1] = 5; sites[2] = 10;   // PE(0,0), PE(1,1), PE(2,2)
        rst_n = 1; load_w = 0; heal_start = 0;
        w_flat = '0;

        @(negedge clk); rst_n = 0;
        repeat(2) @(negedge clk); rst_n = 1;
        @(negedge clk);
        for (int i = 0; i < N*N; i++) w_flat[16*i +: 16] = wcode[i];
        load_w = 1;
        @(negedge clk);
        load_w = 0; w_flat = '0;

        // 三段稳态任务流, 每段一次 2 拍注入
        for (int t = 0; t < 3; t++) begin
            set_feed(20 + t*80, 28 + t*80);       // 8 拍稳态突发
            wait_beat(24 + t*80);                  // 稳态中注入
            tc_mark = transient_cnt;
            inject_2beat(sites[t]/4, sites[t]%4);
            repeat(24) @(negedge clk);             // 等脉冲计数稳定
            pulses[t] = transient_cnt - tc_mark;
            $display("[CANFAIL] trial %0d: 2-beat injection @ PE(%0d,%0d) -> pulses=%0d",
                     t, sites[t]/4, sites[t]%4, pulses[t]);
            if (busy === 1'b1)
                $display("[CANFAIL][WARN] trial %0d 触发 busy (2 拍 2 脉冲不应越阈 THRESH=3)", t);
        end

        max_pulses = 0;
        for (int t = 0; t < 3; t++) if (pulses[t] > max_pulses) max_pulses = pulses[t];

        if (max_pulses >= 2)
            $display("[GATE] canfail_2beat: max_pulses=%0d (>=2) -> PASS", max_pulses);
        else
            $display("[GATE] canfail_2beat: max_pulses=%0d (<2) -> FAIL (测量链对多拍注入不敏感!)", max_pulses);

        repeat(10) @(negedge clk);
        $finish;
    end
endmodule
