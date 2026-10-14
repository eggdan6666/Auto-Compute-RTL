`timescale 1ns/1ps
// =============================================================================
// tb_round2_gates.sv — Round 2 终审门控 (G3 MTTR 解毒 / G4 BIST 灌流韧性)
//
// 四个场景顺序执行 (每场景前完整复位), 各打印规范化判定行:
//   [GATE] G3a: PASS|FAIL: detail   — detect 毒化 (修复后 detect_latency 须 <=10)
//   [GATE] G3b: PASS|FAIL: detail   — repair 毒化 (修复后 repair_latency 须 <=80)
//   [GATE] G4a: PASS|FAIL: detail   — 瞬态簇 + 持续灌流 (不得伪 unhealable/伪重映射)
//   [GATE] G4b: PASS|FAIL: detail   — 永久故障 + 持续灌流 (须正确自愈 fault_col=1)
//
// 修复前基线 (Round 1 实测): G3a FAIL (detect=190), G3b FAIL (repair=248),
//                           G4a FAIL (unhealable=1), G4b FAIL (unhealable=1, fc=0)
// =============================================================================
module tb_round2_gates;
    localparam int N = 4;
    localparam int M = 8;
    localparam real CLK_PERIOD = 10.0;
    localparam int MAXB = 460;

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

    always @(posedge clk) beat <= (beat < MAXB) ? beat + 1 : beat;

    task automatic set_feed(input int lo, input int hi);
        for (int b = lo; b < hi && b < MAXB; b++) sched[b] = 1;
    endtask

    task automatic wait_beat(input int b);
        while (beat < b) @(negedge clk);
    endtask

    task automatic wait_busy_low(input int max_beats);
        int t;
        t = 0;
        while (busy === 1'b1 && t < max_beats) begin @(negedge clk); t = t + 1; end
        repeat(3) @(negedge clk);
    endtask

    task automatic wait_heal_or_fail(input int max_beats);
        int t;
        t = 0;
        while (healed === 1'b0 && unhealable === 1'b0 && t < max_beats) begin
            @(negedge clk); t = t + 1;
        end
        repeat(3) @(negedge clk);
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

    // ---------------------------------------------------------------- 注入
    task automatic inject_1beat(input int r, input int c);
        case (r*4+c)
            0:  force dut.u_phy.g_row[0].g_pe[0].u_pe.psum_out = 40'h0;
            5:  force dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out = 40'h0;
            10: force dut.u_phy.g_row[2].g_pe[2].u_pe.psum_out = 40'h0;
            default: $fatal(1, "gate: 未支持的注入点");
        endcase
        @(posedge clk);
        case (r*4+c)
            0:  release dut.u_phy.g_row[0].g_pe[0].u_pe.psum_out;
            5:  release dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out;
            10: release dut.u_phy.g_row[2].g_pe[2].u_pe.psum_out;
        endcase
    endtask

    task automatic do_reset();
        for (int b = 0; b < MAXB; b++) sched[b] = 0;
        beat = 0;                          // TB 内部拍计数器随场景归零 (调度表按绝对拍编制)
        @(negedge clk); rst_n = 0; load_w = 0; heal_start = 0;
        repeat(2) @(negedge clk); rst_n = 1;
        @(negedge clk);
        for (int i = 0; i < N*N; i++) w_flat[16*i +: 16] = wcode[i];
        load_w = 1;
        @(negedge clk);
        load_w = 0; w_flat = '0;
    endtask

    // ---------------------------------------------------------------- 场景
    task automatic scen_detect_poison();   // G3a
        set_feed(10, 18);
        wait_beat(14);
        inject_1beat(0, 0);                // 单个真单拍瞬态 (1 脉冲, 不触发)
        repeat(16) @(negedge clk);
        wait_beat(200);                    // 长时间静默 (detect_cnt 修复前会虚涨)
        set_feed(200, 208);
        force dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out = 40'h0;   // 永久硬故障
        wait_heal_or_fail(250);
        release dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out;
        if (healed === 1'b1 && fault_col === 5'd1 && detect_latency <= 16'd10)
            $display("[GATE] G3a: PASS: detect_latency=%0d (<=10), healed=1, fault_col=1",
                     detect_latency);
        else
            $display("[GATE] G3a: FAIL: detect_latency=%0d (须<=10), healed=%b, fault_col=%0d, unheal=%b",
                     detect_latency, healed, fault_col, unhealable);
    endtask

    task automatic scen_repair_poison();   // G3b
        set_feed(10, 18);
        wait_beat(12); inject_1beat(0, 0); // 瞬态簇: 3 脉冲 -> 触发 BIST
        wait_beat(14); inject_1beat(1, 1);
        wait_beat(16); inject_1beat(2, 2);
        wait_busy_low(200);                // 等 BIST1 结束 (无故障应通过)
        wait_beat(200);
        set_feed(200, 208);
        force dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out = 40'h0;   // 永久硬故障
        wait_heal_or_fail(250);
        release dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out;
        if (healed === 1'b1 && fault_col === 5'd1 &&
            repair_latency <= 16'd80 && detect_latency <= 16'd10)
            $display("[GATE] G3b: PASS: repair_latency=%0d (<=80), detect=%0d, healed=1, fault_col=1",
                     repair_latency, detect_latency);
        else
            $display("[GATE] G3b: FAIL: repair_latency=%0d (须<=80), detect=%0d, healed=%b, fault_col=%0d, unheal=%b",
                     repair_latency, detect_latency, healed, fault_col, unhealable);
    endtask

    task automatic scen_cluster_under_feed();  // G4a
        set_feed(10, 200);                 // 持续灌流跨越整个自愈期
        wait_beat(12); inject_1beat(0, 0); // 瞬态簇 (无任何真实故障)
        wait_beat(14); inject_1beat(1, 1);
        wait_beat(16); inject_1beat(2, 2);
        wait_busy_low(250);                // 等自愈流程结束
        wait_beat(280);
        if (unhealable === 1'b0 && busy === 1'b0 &&
            !(healed === 1'b1 && fault_col !== 5'd5))
            $display("[GATE] G4a: PASS: 无伪 unhealable/伪重映射 (healed=%b, fault_col=%0d, unheal=%b)",
                     healed, fault_col, unhealable);
        else
            $display("[GATE] G4a: FAIL: healed=%b, fault_col=%0d, unhealable=%b, busy=%b (持续灌流下BIST被污染!)",
                     healed, fault_col, unhealable, busy);
    endtask

    task automatic scen_permfault_under_feed();  // G4b
        set_feed(10, 300);                 // 持续灌流
        wait_beat(100);
        force dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out = 40'h0;   // 永久硬故障 @ 列1
        wait_heal_or_fail(250);
        release dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out;
        if (healed === 1'b1 && fault_col === 5'd1 && unhealable === 1'b0)
            $display("[GATE] G4b: PASS: 持续灌流下正确自愈 (healed=1, fault_col=1, detect=%0d, repair=%0d)",
                     detect_latency, repair_latency);
        else
            $display("[GATE] G4b: FAIL: healed=%b, fault_col=%0d, unhealable=%b (持续灌流下自愈失败!)",
                     healed, fault_col, unhealable);
    endtask

    // ---------------------------------------------------------------- main
    initial begin
        beat = 0;
        for (int b = 0; b < MAXB; b++) sched[b] = 0;
        rst_n = 1; load_w = 0; heal_start = 0;
        w_flat = '0; a_flat = '0; valid_in = 0;

        do_reset();                scen_detect_poison();
        do_reset();                scen_repair_poison();
        do_reset();                scen_cluster_under_feed();
        do_reset();                scen_permfault_under_feed();

        $display("[GATE] tb_round2_gates: ALL SCENARIOS DONE");
        $finish;
    end
endmodule
