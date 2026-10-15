`timescale 1ns/1ps

// tb_transient_train_n4.sv — 瞬态脉冲串训练集 TB (N=4, 由 gen_transient_train_tb.py 生成)
// 在线路径: 持续流式任务 -> 注入脉冲事件 -> 观测 dut.abft_err / arbiter 行为 -> 特征 CSV

module tb_transient_train;

    localparam int  N  = 4;
    localparam int  M  = 8;
    localparam int  STREAM_CYCLES = 200;
    localparam real CLK_PERIOD = 10.0;

    logic              clk, rst_n, load_w;
    logic [N*N*16-1:0] w_flat;
    logic              valid_in;
    logic [N*16-1:0]   a_flat;
    logic [N-1:0]      valid_out;
    logic [N*16-1:0]   c_flat;
    logic              heal_start, busy, fault_detected, healed, unhealable;
    logic [4:0]        fault_col;
    logic [15:0]       detect_latency, repair_latency, total_mttr;
    logic [31:0]       transient_cnt;

    self_healing_array #(.N(N)) dut (
        .clk(clk), .rst_n(rst_n), .load_w(load_w), .w_flat(w_flat),
        .valid_in(valid_in), .a_flat(a_flat), .valid_out(valid_out), .c_flat(c_flat),
        .heal_start(heal_start), .busy(busy), .fault_detected(fault_detected),
        .healed(healed), .unhealable(unhealable), .fault_col(fault_col),
        .detect_latency(detect_latency), .repair_latency(repair_latency),
        .total_mttr(total_mttr), .transient_cnt(transient_cnt)
    );

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
                wcode[r*N + c] = p2a(r + c);
        for (int m = 0; m < M; m++) begin
            acode[m] = '0;
            for (int r = 0; r < N; r++)
                acode[m][16*r +: 16] = p2a(m + r + 1);
        end
    end

    logic [191:0] scen [0:4095];
    integer       csv, NS;

    logic [39:0] inj_val40;   // 模块级(静态)变量: 供 force 引用(自动变量不允许)
    logic [15:0] inj_val16;

    // ---------------- 注入 / 释放（静态任务） ----------------
    task inject_ev(input int model, input int col, input int row);
        if (model == 1) begin
            case (row*5+col)
                0: force dut.u_phy.g_row[0].g_pe[0].u_pe.psum_out = inj_val40;
                1: force dut.u_phy.g_row[0].g_pe[1].u_pe.psum_out = inj_val40;
                2: force dut.u_phy.g_row[0].g_pe[2].u_pe.psum_out = inj_val40;
                3: force dut.u_phy.g_row[0].g_pe[3].u_pe.psum_out = inj_val40;
                4: force dut.u_phy.g_row[0].g_pe[4].u_pe.psum_out = inj_val40;
                5: force dut.u_phy.g_row[1].g_pe[0].u_pe.psum_out = inj_val40;
                6: force dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out = inj_val40;
                7: force dut.u_phy.g_row[1].g_pe[2].u_pe.psum_out = inj_val40;
                8: force dut.u_phy.g_row[1].g_pe[3].u_pe.psum_out = inj_val40;
                9: force dut.u_phy.g_row[1].g_pe[4].u_pe.psum_out = inj_val40;
                10: force dut.u_phy.g_row[2].g_pe[0].u_pe.psum_out = inj_val40;
                11: force dut.u_phy.g_row[2].g_pe[1].u_pe.psum_out = inj_val40;
                12: force dut.u_phy.g_row[2].g_pe[2].u_pe.psum_out = inj_val40;
                13: force dut.u_phy.g_row[2].g_pe[3].u_pe.psum_out = inj_val40;
                14: force dut.u_phy.g_row[2].g_pe[4].u_pe.psum_out = inj_val40;
                15: force dut.u_phy.g_row[3].g_pe[0].u_pe.psum_out = inj_val40;
                16: force dut.u_phy.g_row[3].g_pe[1].u_pe.psum_out = inj_val40;
                17: force dut.u_phy.g_row[3].g_pe[2].u_pe.psum_out = inj_val40;
                18: force dut.u_phy.g_row[3].g_pe[3].u_pe.psum_out = inj_val40;
                19: force dut.u_phy.g_row[3].g_pe[4].u_pe.psum_out = inj_val40;
            endcase
        end else if (model == 2) begin
            case (row*5+col)
                0: force dut.u_phy.g_row[0].g_pe[0].u_pe.w_reg = inj_val16;
                1: force dut.u_phy.g_row[0].g_pe[1].u_pe.w_reg = inj_val16;
                2: force dut.u_phy.g_row[0].g_pe[2].u_pe.w_reg = inj_val16;
                3: force dut.u_phy.g_row[0].g_pe[3].u_pe.w_reg = inj_val16;
                4: force dut.u_phy.g_row[0].g_pe[4].u_pe.w_reg = inj_val16;
                5: force dut.u_phy.g_row[1].g_pe[0].u_pe.w_reg = inj_val16;
                6: force dut.u_phy.g_row[1].g_pe[1].u_pe.w_reg = inj_val16;
                7: force dut.u_phy.g_row[1].g_pe[2].u_pe.w_reg = inj_val16;
                8: force dut.u_phy.g_row[1].g_pe[3].u_pe.w_reg = inj_val16;
                9: force dut.u_phy.g_row[1].g_pe[4].u_pe.w_reg = inj_val16;
                10: force dut.u_phy.g_row[2].g_pe[0].u_pe.w_reg = inj_val16;
                11: force dut.u_phy.g_row[2].g_pe[1].u_pe.w_reg = inj_val16;
                12: force dut.u_phy.g_row[2].g_pe[2].u_pe.w_reg = inj_val16;
                13: force dut.u_phy.g_row[2].g_pe[3].u_pe.w_reg = inj_val16;
                14: force dut.u_phy.g_row[2].g_pe[4].u_pe.w_reg = inj_val16;
                15: force dut.u_phy.g_row[3].g_pe[0].u_pe.w_reg = inj_val16;
                16: force dut.u_phy.g_row[3].g_pe[1].u_pe.w_reg = inj_val16;
                17: force dut.u_phy.g_row[3].g_pe[2].u_pe.w_reg = inj_val16;
                18: force dut.u_phy.g_row[3].g_pe[3].u_pe.w_reg = inj_val16;
                19: force dut.u_phy.g_row[3].g_pe[4].u_pe.w_reg = inj_val16;
            endcase
        end
    endtask

    task release_ev(input int model, input int col, input int row);
        if (model == 1) begin
            case (row*5+col)
            0: release dut.u_phy.g_row[0].g_pe[0].u_pe.psum_out;
            1: release dut.u_phy.g_row[0].g_pe[1].u_pe.psum_out;
            2: release dut.u_phy.g_row[0].g_pe[2].u_pe.psum_out;
            3: release dut.u_phy.g_row[0].g_pe[3].u_pe.psum_out;
            4: release dut.u_phy.g_row[0].g_pe[4].u_pe.psum_out;
            5: release dut.u_phy.g_row[1].g_pe[0].u_pe.psum_out;
            6: release dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out;
            7: release dut.u_phy.g_row[1].g_pe[2].u_pe.psum_out;
            8: release dut.u_phy.g_row[1].g_pe[3].u_pe.psum_out;
            9: release dut.u_phy.g_row[1].g_pe[4].u_pe.psum_out;
            10: release dut.u_phy.g_row[2].g_pe[0].u_pe.psum_out;
            11: release dut.u_phy.g_row[2].g_pe[1].u_pe.psum_out;
            12: release dut.u_phy.g_row[2].g_pe[2].u_pe.psum_out;
            13: release dut.u_phy.g_row[2].g_pe[3].u_pe.psum_out;
            14: release dut.u_phy.g_row[2].g_pe[4].u_pe.psum_out;
            15: release dut.u_phy.g_row[3].g_pe[0].u_pe.psum_out;
            16: release dut.u_phy.g_row[3].g_pe[1].u_pe.psum_out;
            17: release dut.u_phy.g_row[3].g_pe[2].u_pe.psum_out;
            18: release dut.u_phy.g_row[3].g_pe[3].u_pe.psum_out;
            19: release dut.u_phy.g_row[3].g_pe[4].u_pe.psum_out;
            endcase
        end else if (model == 2) begin
            case (row*5+col)
            0: release dut.u_phy.g_row[0].g_pe[0].u_pe.w_reg;
            1: release dut.u_phy.g_row[0].g_pe[1].u_pe.w_reg;
            2: release dut.u_phy.g_row[0].g_pe[2].u_pe.w_reg;
            3: release dut.u_phy.g_row[0].g_pe[3].u_pe.w_reg;
            4: release dut.u_phy.g_row[0].g_pe[4].u_pe.w_reg;
            5: release dut.u_phy.g_row[1].g_pe[0].u_pe.w_reg;
            6: release dut.u_phy.g_row[1].g_pe[1].u_pe.w_reg;
            7: release dut.u_phy.g_row[1].g_pe[2].u_pe.w_reg;
            8: release dut.u_phy.g_row[1].g_pe[3].u_pe.w_reg;
            9: release dut.u_phy.g_row[1].g_pe[4].u_pe.w_reg;
            10: release dut.u_phy.g_row[2].g_pe[0].u_pe.w_reg;
            11: release dut.u_phy.g_row[2].g_pe[1].u_pe.w_reg;
            12: release dut.u_phy.g_row[2].g_pe[2].u_pe.w_reg;
            13: release dut.u_phy.g_row[2].g_pe[3].u_pe.w_reg;
            14: release dut.u_phy.g_row[2].g_pe[4].u_pe.w_reg;
            15: release dut.u_phy.g_row[3].g_pe[0].u_pe.w_reg;
            16: release dut.u_phy.g_row[3].g_pe[1].u_pe.w_reg;
            17: release dut.u_phy.g_row[3].g_pe[2].u_pe.w_reg;
            18: release dut.u_phy.g_row[3].g_pe[3].u_pe.w_reg;
            19: release dut.u_phy.g_row[3].g_pe[4].u_pe.w_reg;
            endcase
        end
    endtask

    task release_all();
        release dut.u_phy.g_row[0].g_pe[0].u_pe.psum_out;
        release dut.u_phy.g_row[0].g_pe[1].u_pe.psum_out;
        release dut.u_phy.g_row[0].g_pe[2].u_pe.psum_out;
        release dut.u_phy.g_row[0].g_pe[3].u_pe.psum_out;
        release dut.u_phy.g_row[0].g_pe[4].u_pe.psum_out;
        release dut.u_phy.g_row[1].g_pe[0].u_pe.psum_out;
        release dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out;
        release dut.u_phy.g_row[1].g_pe[2].u_pe.psum_out;
        release dut.u_phy.g_row[1].g_pe[3].u_pe.psum_out;
        release dut.u_phy.g_row[1].g_pe[4].u_pe.psum_out;
        release dut.u_phy.g_row[2].g_pe[0].u_pe.psum_out;
        release dut.u_phy.g_row[2].g_pe[1].u_pe.psum_out;
        release dut.u_phy.g_row[2].g_pe[2].u_pe.psum_out;
        release dut.u_phy.g_row[2].g_pe[3].u_pe.psum_out;
        release dut.u_phy.g_row[2].g_pe[4].u_pe.psum_out;
        release dut.u_phy.g_row[3].g_pe[0].u_pe.psum_out;
        release dut.u_phy.g_row[3].g_pe[1].u_pe.psum_out;
        release dut.u_phy.g_row[3].g_pe[2].u_pe.psum_out;
        release dut.u_phy.g_row[3].g_pe[3].u_pe.psum_out;
        release dut.u_phy.g_row[3].g_pe[4].u_pe.psum_out;
        release dut.u_phy.g_row[0].g_pe[0].u_pe.w_reg;
        release dut.u_phy.g_row[0].g_pe[1].u_pe.w_reg;
        release dut.u_phy.g_row[0].g_pe[2].u_pe.w_reg;
        release dut.u_phy.g_row[0].g_pe[3].u_pe.w_reg;
        release dut.u_phy.g_row[0].g_pe[4].u_pe.w_reg;
        release dut.u_phy.g_row[1].g_pe[0].u_pe.w_reg;
        release dut.u_phy.g_row[1].g_pe[1].u_pe.w_reg;
        release dut.u_phy.g_row[1].g_pe[2].u_pe.w_reg;
        release dut.u_phy.g_row[1].g_pe[3].u_pe.w_reg;
        release dut.u_phy.g_row[1].g_pe[4].u_pe.w_reg;
        release dut.u_phy.g_row[2].g_pe[0].u_pe.w_reg;
        release dut.u_phy.g_row[2].g_pe[1].u_pe.w_reg;
        release dut.u_phy.g_row[2].g_pe[2].u_pe.w_reg;
        release dut.u_phy.g_row[2].g_pe[3].u_pe.w_reg;
        release dut.u_phy.g_row[2].g_pe[4].u_pe.w_reg;
        release dut.u_phy.g_row[3].g_pe[0].u_pe.w_reg;
        release dut.u_phy.g_row[3].g_pe[1].u_pe.w_reg;
        release dut.u_phy.g_row[3].g_pe[2].u_pe.w_reg;
        release dut.u_phy.g_row[3].g_pe[3].u_pe.w_reg;
        release dut.u_phy.g_row[3].g_pe[4].u_pe.w_reg;
    endtask

    task apply_reset();
        @(negedge clk);
        rst_n = 0; valid_in = 0; load_w = 0; heal_start = 0;
        a_flat = '0; w_flat = '0;
        repeat(2) @(negedge clk);
        rst_n = 1;
        @(negedge clk);
    endtask

    // ---------------- 逐场景 ----------------
    int ev_m [0:3], ev_c [0:3], ev_r [0:3], ev_s [0:3], ev_d [0:3];
    logic [15:0] ev_v [0:3];

    initial begin
        int n_ev, label, npu, maxw, ntrig, busy_ev, first_cyc, last_cyc;
        int gap_le8, gap_m, gap_h, gap_x, prev_cyc, tc_before;
        logic e;
        logic [31:0] win;
        int wcnt, newc;
        string fname;

        if (!$value$plusargs("NS=%d", NS)) NS = 1;
        $sformat(fname, "transient_train_results_n4.csv");
        $readmemh("transient_train_scen4.hex", scen);

        csv = $fopen(fname, "w");
        $fwrite(csv, "id,label,n_ev,n_pulse,max_win32,n_trig,tcnt_delta,busy_ever,first_cyc,last_cyc,gap_le8,gap_9_32,gap_33_40,gap_gt40\n");

        rst_n = 1; valid_in = 0; load_w = 0; heal_start = 0;
        a_flat = '0; w_flat = '0;

        for (int id = 0; id < NS; id++) begin
            n_ev  = scen[id][191:184];
            label = scen[id][183:176];
            for (int k = 0; k < 4; k++) begin
                logic [39:0] w;
                w = scen[id][(175 - 40*k) -: 40];
                ev_m[k] = w[39:38]; ev_c[k] = w[37:35]; ev_r[k] = w[34:32];
                ev_s[k] = w[31:24]; ev_d[k] = w[23:16]; ev_v[k] = w[15:0];
            end

            apply_reset();
            for (int i = 0; i < N*N; i++) w_flat[16*i +: 16] = wcode[i];
            load_w = 1;
            @(negedge clk);
            load_w = 0; w_flat = '0;
            repeat(3) @(negedge clk);

            tc_before = transient_cnt;

            fork
                begin : drv
                    logic bp;
                    bp = 1'b0;
                    for (int t = 0; t < STREAM_CYCLES; t++) begin
                        @(negedge clk);
                        if (busy === 1'b1) begin
                            valid_in = 1'b0; a_flat = '0; bp = 1'b1;      // 自愈期间停灌
                        end else begin
                            if (bp) begin     // 自愈刚结束: 按设计契约重载任务权重(BIST 覆盖了驻留权重)
                                for (int i = 0; i < N*N; i++) w_flat[16*i +: 16] = wcode[i];
                                load_w = 1'b1;
                                @(negedge clk);
                                load_w = 1'b0; w_flat = '0; bp = 1'b0;
                            end
                            a_flat = acode[t % M];
                            valid_in = 1'b1;
                        end
                    end
                    @(negedge clk);
                    valid_in = 1'b0; a_flat = '0;
                end
                begin : obs
                    npu=0; maxw=0; ntrig=0; busy_ev=0; first_cyc=-1; last_cyc=-1;
                    gap_le8=0; gap_m=0; gap_h=0; gap_x=0; prev_cyc=-1;
                    win='0; wcnt=0;
                    for (int t = 0; t < STREAM_CYCLES; t++) begin
                        @(negedge clk);
                        e = dut.abft_err;
                        if (dut.arb_trigger_heal === 1'b1) ntrig++;
                        if (busy === 1'b1) busy_ev = 1;
                        if (e) begin
                            if (first_cyc < 0) first_cyc = t;
                            if (prev_cyc >= 0) begin
                                int g; g = t - prev_cyc;
                                if      (g <= 8)  gap_le8++;
                                else if (g <= 32) gap_m++;
                                else if (g <= 40) gap_h++;
                                else              gap_x++;
                            end
                            prev_cyc = t; last_cyc = t; npu++;
                        end
                        newc = wcnt + (e ? 1 : 0) - win[31];
                        win  = {win[30:0], e};
                        wcnt = newc;
                        if (newc > maxw) maxw = newc;
                    end
                end
                begin : inj0
                    if (n_ev > 0) begin
                        inj_val40 = {24'h0, ev_v[0]}; inj_val16 = ev_v[0];
                        repeat(ev_s[0]) @(negedge clk);
                        inject_ev(ev_m[0], ev_c[0], ev_r[0]);
                        if (ev_d[0] == 1) begin @(posedge clk); release_ev(ev_m[0], ev_c[0], ev_r[0]); end
                    end
                end
                begin : inj1
                    if (n_ev > 1) begin
                        inj_val40 = {24'h0, ev_v[1]}; inj_val16 = ev_v[1];
                        repeat(ev_s[1]) @(negedge clk);
                        inject_ev(ev_m[1], ev_c[1], ev_r[1]);
                        if (ev_d[1] == 1) begin @(posedge clk); release_ev(ev_m[1], ev_c[1], ev_r[1]); end
                    end
                end
                begin : inj2
                    if (n_ev > 2) begin
                        inj_val40 = {24'h0, ev_v[2]}; inj_val16 = ev_v[2];
                        repeat(ev_s[2]) @(negedge clk);
                        inject_ev(ev_m[2], ev_c[2], ev_r[2]);
                        if (ev_d[2] == 1) begin @(posedge clk); release_ev(ev_m[2], ev_c[2], ev_r[2]); end
                    end
                end
                begin : inj3
                    if (n_ev > 3) begin
                        inj_val40 = {24'h0, ev_v[3]}; inj_val16 = ev_v[3];
                        repeat(ev_s[3]) @(negedge clk);
                        inject_ev(ev_m[3], ev_c[3], ev_r[3]);
                        if (ev_d[3] == 1) begin @(posedge clk); release_ev(ev_m[3], ev_c[3], ev_r[3]); end
                    end
                end
            join

            release_all();

            $fwrite(csv, "%0d,%0d,%0d,%0d,%0d,%0d,%0d,%0d,%0d,%0d,%0d,%0d,%0d,%0d\n",
                    id, label, n_ev, npu, maxw, ntrig, (transient_cnt - tc_before),
                    busy_ev, first_cyc, last_cyc, gap_le8, gap_m, gap_h, gap_x);
            if (id % 50 == 0) $display("[ttrain N=%0d] %0d/%0d done", N, id, NS);
        end

        $fclose(csv);
        $display("[ttrain N=%0d] ALL %0d SCENARIOS DONE", N, NS);
        $finish;
    end

endmodule
