`timescale 1ns/1ps

// =============================================================================
// tb_inj_trace.sv — 【新增文件，不改动任何现有 RTL/TB】Icarus 侧同场景对拍 trace TB
//
// 目的: 定位 Icarus 与 Verilator 在同一注入场景下的行为差异。
//   与 tb_transient_train_n4.sv 使用**完全相同**的注入惯用法:
//       @(negedge clk) 处 force  ->  @(posedge clk) 处 release   （单次单拍注入）
//   逐拍 dump 注入点 PE、其下游 PE、列出口与 abft_err / arb_trigger_heal,
//   供与 Verilator HIL 侧 (hil.exe) 的同口径 trace 做逐拍 diff。
//
// 采样约定 (两端严格一致, 与仿真器调度无关):
//   ne_*  = 第 t 拍 negedge 采样, 发生在【驱动/注入之前】 → 与训练 TB 的 obs 分支同相
//   out_* = 第 t 拍"周期末"状态, 在【第 t+1 拍 negedge、注入之前】读回
//           (寄存器在 posedge t 落定后一直保持到下个 posedge, 故 out_x(t) ≡ ne_x(t+1))
//           这样完全绕开 Icarus #0/NBA 区序与 Verilator eval 区序的口径差异。
//
// 用法:
//   vvp tb_pair.vvp +INJ_CYC=12 +INJ_ROW=2 +INJ_COL=0 +INJ_MODEL=1 +INJ_VAL=0 +NCYC=120
// =============================================================================

module tb_inj_trace;

    localparam int  N  = 4;
    localparam int  M  = 8;
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

    // ---- 与训练 TB 完全同源的 mission 数据 ----
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

    // ---- 注入值必须是模块级静态变量 (Icarus: 过程块中的自动变量不能被 force 引用) ----
    logic [39:0] inj_val40;
    logic [15:0] inj_val16;

    // ---------------- 注入 / 释放（静态任务，与训练 TB 同一惯用法） ----------------
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

    // ---------------- 纯读（非 force 语义） ----------------
    function automatic logic [39:0] rd_psum(input int row, input int col);
        case (row*5+col)
            0: rd_psum = dut.u_phy.g_row[0].g_pe[0].u_pe.psum_out;
            1: rd_psum = dut.u_phy.g_row[0].g_pe[1].u_pe.psum_out;
            2: rd_psum = dut.u_phy.g_row[0].g_pe[2].u_pe.psum_out;
            3: rd_psum = dut.u_phy.g_row[0].g_pe[3].u_pe.psum_out;
            4: rd_psum = dut.u_phy.g_row[0].g_pe[4].u_pe.psum_out;
            5: rd_psum = dut.u_phy.g_row[1].g_pe[0].u_pe.psum_out;
            6: rd_psum = dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out;
            7: rd_psum = dut.u_phy.g_row[1].g_pe[2].u_pe.psum_out;
            8: rd_psum = dut.u_phy.g_row[1].g_pe[3].u_pe.psum_out;
            9: rd_psum = dut.u_phy.g_row[1].g_pe[4].u_pe.psum_out;
            10: rd_psum = dut.u_phy.g_row[2].g_pe[0].u_pe.psum_out;
            11: rd_psum = dut.u_phy.g_row[2].g_pe[1].u_pe.psum_out;
            12: rd_psum = dut.u_phy.g_row[2].g_pe[2].u_pe.psum_out;
            13: rd_psum = dut.u_phy.g_row[2].g_pe[3].u_pe.psum_out;
            14: rd_psum = dut.u_phy.g_row[2].g_pe[4].u_pe.psum_out;
            15: rd_psum = dut.u_phy.g_row[3].g_pe[0].u_pe.psum_out;
            16: rd_psum = dut.u_phy.g_row[3].g_pe[1].u_pe.psum_out;
            17: rd_psum = dut.u_phy.g_row[3].g_pe[2].u_pe.psum_out;
            18: rd_psum = dut.u_phy.g_row[3].g_pe[3].u_pe.psum_out;
            19: rd_psum = dut.u_phy.g_row[3].g_pe[4].u_pe.psum_out;
            default: rd_psum = 40'h0;
        endcase
    endfunction

    function automatic logic [15:0] rd_wreg(input int row, input int col);
        case (row*5+col)
            0: rd_wreg = dut.u_phy.g_row[0].g_pe[0].u_pe.w_reg;
            1: rd_wreg = dut.u_phy.g_row[0].g_pe[1].u_pe.w_reg;
            2: rd_wreg = dut.u_phy.g_row[0].g_pe[2].u_pe.w_reg;
            3: rd_wreg = dut.u_phy.g_row[0].g_pe[3].u_pe.w_reg;
            4: rd_wreg = dut.u_phy.g_row[0].g_pe[4].u_pe.w_reg;
            5: rd_wreg = dut.u_phy.g_row[1].g_pe[0].u_pe.w_reg;
            6: rd_wreg = dut.u_phy.g_row[1].g_pe[1].u_pe.w_reg;
            7: rd_wreg = dut.u_phy.g_row[1].g_pe[2].u_pe.w_reg;
            8: rd_wreg = dut.u_phy.g_row[1].g_pe[3].u_pe.w_reg;
            9: rd_wreg = dut.u_phy.g_row[1].g_pe[4].u_pe.w_reg;
            10: rd_wreg = dut.u_phy.g_row[2].g_pe[0].u_pe.w_reg;
            11: rd_wreg = dut.u_phy.g_row[2].g_pe[1].u_pe.w_reg;
            12: rd_wreg = dut.u_phy.g_row[2].g_pe[2].u_pe.w_reg;
            13: rd_wreg = dut.u_phy.g_row[2].g_pe[3].u_pe.w_reg;
            14: rd_wreg = dut.u_phy.g_row[2].g_pe[4].u_pe.w_reg;
            15: rd_wreg = dut.u_phy.g_row[3].g_pe[0].u_pe.w_reg;
            16: rd_wreg = dut.u_phy.g_row[3].g_pe[1].u_pe.w_reg;
            17: rd_wreg = dut.u_phy.g_row[3].g_pe[2].u_pe.w_reg;
            18: rd_wreg = dut.u_phy.g_row[3].g_pe[3].u_pe.w_reg;
            19: rd_wreg = dut.u_phy.g_row[3].g_pe[4].u_pe.w_reg;
            default: rd_wreg = 16'h0;
        endcase
    endfunction

    task apply_reset();
        @(negedge clk);
        rst_n = 0; valid_in = 0; load_w = 0; heal_start = 0;
        a_flat = '0; w_flat = '0;
        repeat(2) @(negedge clk);
        rst_n = 1;
        @(negedge clk);
    endtask

    // ---------------- 主流程 ----------------
    int   INJ_CYC, INJ_ROW, INJ_COL, INJ_MODEL, NCYC;
    logic [39:0] INJ_VAL;
    integer csv;

    // 采样缓存: ne = 本拍 negedge 前置值; out = 本拍周期末值(= 下一拍 negedge 前置值)
    logic [39:0] ne_psum, ne_wreg, ne_dn;
    logic [39:0] out_psum, out_wreg, out_dn;
    logic        ne_err, ne_trig, ne_busy;
    logic        out_err, out_trig, out_busy;
    logic [15:0] out_cout;
    logic [3:0]  out_vout;
    logic [31:0] ne_tcnt, out_tcnt;

    task sample_ne();
        ne_err  = dut.abft_err;
        ne_trig = dut.arb_trigger_heal;
        ne_tcnt = transient_cnt;
        ne_busy = busy;
        ne_psum = rd_psum(INJ_ROW, INJ_COL);
        ne_wreg = {24'h0, rd_wreg(INJ_ROW, INJ_COL)};
        ne_dn   = (INJ_ROW < N-1) ? rd_psum(INJ_ROW+1, INJ_COL)
                                  : {24'h0, c_flat[16*INJ_COL +: 16]};
    endtask

    task sample_out();
        out_err  = dut.abft_err;
        out_trig = dut.arb_trigger_heal;
        out_tcnt = transient_cnt;
        out_busy = busy;
        out_psum = rd_psum(INJ_ROW, INJ_COL);
        out_wreg = {24'h0, rd_wreg(INJ_ROW, INJ_COL)};
        out_dn   = (INJ_ROW < N-1) ? rd_psum(INJ_ROW+1, INJ_COL)
                                   : {24'h0, c_flat[16*INJ_COL +: 16]};
        out_cout = c_flat[16*INJ_COL +: 16];
        out_vout = valid_out;
    endtask

    initial begin
        INJ_CYC = 12; INJ_ROW = 2; INJ_COL = 0; INJ_MODEL = 1; NCYC = 120; INJ_VAL = 40'h0;
        void'($value$plusargs("INJ_CYC=%d",   INJ_CYC));
        void'($value$plusargs("INJ_ROW=%d",   INJ_ROW));
        void'($value$plusargs("INJ_COL=%d",   INJ_COL));
        void'($value$plusargs("INJ_MODEL=%d", INJ_MODEL));
        void'($value$plusargs("INJ_VAL=%h",   INJ_VAL));
        void'($value$plusargs("NCYC=%d",      NCYC));

        rst_n = 1; valid_in = 0; load_w = 0; heal_start = 0;
        a_flat = '0; w_flat = '0;

        csv = $fopen("inj_trace_icarus.csv", "w");
        $fwrite(csv, "t,ne_err,out_err,ne_psum,out_psum,ne_wreg,out_wreg,ne_dn,out_dn,ne_tcnt,out_tcnt,ne_busy,out_busy,ne_trig,out_trig,out_cout,out_vout\n");

        apply_reset();
        for (int i = 0; i < N*N; i++) w_flat[16*i +: 16] = wcode[i];
        load_w = 1;
        @(negedge clk);
        load_w = 0; w_flat = '0;
        repeat(3) @(negedge clk);      // 与训练 TB 完全同相位的装载后静默

        @(negedge clk);                // 流式第 0 拍 negedge
        sample_ne();

        for (int t = 0; t < NCYC; t++) begin
            logic pending;

            // ---- 驱动 (与训练 TB 的 drv 分支同口径) ----
            if (busy === 1'b1) begin
                valid_in = 1'b0; a_flat = '0;
            end else begin
                a_flat = acode[t % M];
                valid_in = 1'b1;
            end

            // ---- 注入: negedge force / posedge release (训练 TB 的单拍惯用法) ----
            pending = 1'b0;
            if (t == INJ_CYC) begin
                inj_val40 = INJ_VAL; inj_val16 = INJ_VAL[15:0];
                inject_ev(INJ_MODEL, INJ_COL, INJ_ROW);
                pending = 1'b1;
            end

            @(posedge clk);
            if (pending) release_ev(INJ_MODEL, INJ_COL, INJ_ROW);

            @(negedge clk);            // 第 t+1 拍 negedge (注入之前) => 读回第 t 拍周期末值
            sample_out();

            $fwrite(csv, "%0d,%0d,%0d,%0h,%0h,%0h,%0h,%0h,%0h,%0d,%0d,%0d,%0d,%0d,%0d,%0h,%0d\n",
                    t, ne_err, out_err, ne_psum, out_psum, ne_wreg, out_wreg,
                    ne_dn, out_dn, ne_tcnt, out_tcnt, ne_busy, out_busy,
                    ne_trig, out_trig, out_cout, out_vout);

            // ne(t+1) ≡ out(t) (下一拍的注入尚未施加)
            ne_err = out_err; ne_trig = out_trig; ne_tcnt = out_tcnt; ne_busy = out_busy;
            ne_psum = out_psum; ne_wreg = out_wreg; ne_dn = out_dn;
        end

        $fclose(csv);
        $display("[inj_trace] INJ_CYC=%0d ROW=%0d COL=%0d MODEL=%0d VAL=%0h NCYC=%0d DONE",
                 INJ_CYC, INJ_ROW, INJ_COL, INJ_MODEL, INJ_VAL, NCYC);
        $finish;
    end

endmodule
