`timescale 1ns/1ps

// tb_fault_campaign_n4.sv — 批量故障注入回归 TB (N=4, 由 gen_campaign_tb.py 生成)
// 每场景: 复位 -> 注入(可选) -> 自愈前任务(基线) -> BIST+定位+重映射+复验 -> 自愈后任务 -> CSV

module tb_fault_campaign;

    localparam N = 4;
    localparam M = 8;
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

    integer csv;
    integer NS;

    self_healing_array #(.N(N)) dut (
        .clk(clk), .rst_n(rst_n), .load_w(load_w), .w_flat(w_flat),
        .valid_in(valid_in), .a_flat(a_flat), .valid_out(valid_out), .c_flat(c_flat),
        .heal_start(heal_start), .busy(busy), .fault_detected(fault_detected),
        .healed(healed), .unhealable(unhealable), .fault_col(fault_col)
    );

    initial begin
        clk = 0;
        forever #(CLK_PERIOD / 2.0) clk = ~clk;
    end

    logic [47:0]       flist  [0:4095];
    logic [15:0]       mw_mem [0:N*N-1];
    logic [2*N*16-1:0] mv_mem [0:M-1];

    task automatic apply_reset();
        @(negedge clk);
        rst_n = 0; valid_in = 0; load_w = 0; heal_start = 0;
        a_flat = '0; w_flat = '0;
        repeat(2) @(negedge clk);
        rst_n = 1;
        @(negedge clk);
    endtask

    task automatic mission_run(output int err);
        int cnt [N];
        int timeout;
        logic [15:0] exp_c;
        logic [15:0] act_c;
        err = 0;
        @(negedge clk);
        for (int i = 0; i < N*N; i++) w_flat[16*i +: 16] = mw_mem[i];
        load_w = 1;
        @(negedge clk);
        load_w = 0; w_flat = '0;
        fork
            begin : drv
                for (int m = 0; m < M; m++) begin
                    @(negedge clk);
                    for (int r = 0; r < N; r++)
                        a_flat[16*r +: 16] = mv_mem[m][16*(N+r) +: 16];
                    valid_in = 1;
                end
                @(negedge clk);
                valid_in = 0; a_flat = '0;
            end
            begin : chk
                for (int c = 0; c < N; c++) cnt[c] = 0;
                timeout = 0;
                while ((cnt[0] < M || cnt[1] < M || cnt[2] < M || cnt[3] < M) && timeout < 600) begin
                    @(negedge clk);
                    timeout++;
                    for (int c = 0; c < N; c++) begin
                        if (valid_out[c] === 1'b1) begin
                            exp_c = mv_mem[cnt[c]][16*c +: 16];
                            act_c = c_flat[16*c +: 16];
                            if (act_c !== exp_c) err++;
                            cnt[c]++;
                        end
                    end
                end
                if (timeout >= 600) err = err + 999;
            end
        join
        repeat(2) @(negedge clk);
    endtask

    task automatic run_heal(output int det, output int loc_ok, output int hld, output int unheal, input int inj_col);
        int t;
        @(negedge clk);
        heal_start = 1;
        @(negedge clk);
        heal_start = 0;
        t = 0;
        while (busy !== 1'b1 && t < 20)   begin @(negedge clk); t++; end
        while (busy === 1'b1 && t < 4000) begin @(negedge clk); t++; end
        @(negedge clk);
        det    = (fault_detected === 1'b1);
        loc_ok = det && (fault_col === inj_col[4:0]);
        hld    = (healed === 1'b1);
        unheal = (unhealable === 1'b1);
    endtask

    task inject(input int model, input int col, input int row, input logic [39:0] val);
        if (model == 1) begin
            case (row*5+col)
            0: force dut.u_phy.g_row[0].g_pe[0].u_pe.psum_out = val;
            1: force dut.u_phy.g_row[0].g_pe[1].u_pe.psum_out = val;
            2: force dut.u_phy.g_row[0].g_pe[2].u_pe.psum_out = val;
            3: force dut.u_phy.g_row[0].g_pe[3].u_pe.psum_out = val;
            4: force dut.u_phy.g_row[0].g_pe[4].u_pe.psum_out = val;
            5: force dut.u_phy.g_row[1].g_pe[0].u_pe.psum_out = val;
            6: force dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out = val;
            7: force dut.u_phy.g_row[1].g_pe[2].u_pe.psum_out = val;
            8: force dut.u_phy.g_row[1].g_pe[3].u_pe.psum_out = val;
            9: force dut.u_phy.g_row[1].g_pe[4].u_pe.psum_out = val;
            10: force dut.u_phy.g_row[2].g_pe[0].u_pe.psum_out = val;
            11: force dut.u_phy.g_row[2].g_pe[1].u_pe.psum_out = val;
            12: force dut.u_phy.g_row[2].g_pe[2].u_pe.psum_out = val;
            13: force dut.u_phy.g_row[2].g_pe[3].u_pe.psum_out = val;
            14: force dut.u_phy.g_row[2].g_pe[4].u_pe.psum_out = val;
            15: force dut.u_phy.g_row[3].g_pe[0].u_pe.psum_out = val;
            16: force dut.u_phy.g_row[3].g_pe[1].u_pe.psum_out = val;
            17: force dut.u_phy.g_row[3].g_pe[2].u_pe.psum_out = val;
            18: force dut.u_phy.g_row[3].g_pe[3].u_pe.psum_out = val;
            19: force dut.u_phy.g_row[3].g_pe[4].u_pe.psum_out = val;
            endcase
        end else if (model == 2) begin
            case (row*5+col)
            0: force dut.u_phy.g_row[0].g_pe[0].u_pe.w_reg = val[15:0];
            1: force dut.u_phy.g_row[0].g_pe[1].u_pe.w_reg = val[15:0];
            2: force dut.u_phy.g_row[0].g_pe[2].u_pe.w_reg = val[15:0];
            3: force dut.u_phy.g_row[0].g_pe[3].u_pe.w_reg = val[15:0];
            4: force dut.u_phy.g_row[0].g_pe[4].u_pe.w_reg = val[15:0];
            5: force dut.u_phy.g_row[1].g_pe[0].u_pe.w_reg = val[15:0];
            6: force dut.u_phy.g_row[1].g_pe[1].u_pe.w_reg = val[15:0];
            7: force dut.u_phy.g_row[1].g_pe[2].u_pe.w_reg = val[15:0];
            8: force dut.u_phy.g_row[1].g_pe[3].u_pe.w_reg = val[15:0];
            9: force dut.u_phy.g_row[1].g_pe[4].u_pe.w_reg = val[15:0];
            10: force dut.u_phy.g_row[2].g_pe[0].u_pe.w_reg = val[15:0];
            11: force dut.u_phy.g_row[2].g_pe[1].u_pe.w_reg = val[15:0];
            12: force dut.u_phy.g_row[2].g_pe[2].u_pe.w_reg = val[15:0];
            13: force dut.u_phy.g_row[2].g_pe[3].u_pe.w_reg = val[15:0];
            14: force dut.u_phy.g_row[2].g_pe[4].u_pe.w_reg = val[15:0];
            15: force dut.u_phy.g_row[3].g_pe[0].u_pe.w_reg = val[15:0];
            16: force dut.u_phy.g_row[3].g_pe[1].u_pe.w_reg = val[15:0];
            17: force dut.u_phy.g_row[3].g_pe[2].u_pe.w_reg = val[15:0];
            18: force dut.u_phy.g_row[3].g_pe[3].u_pe.w_reg = val[15:0];
            19: force dut.u_phy.g_row[3].g_pe[4].u_pe.w_reg = val[15:0];
            endcase
        end
    endtask

    task release_inj(input int model, input int col, input int row);
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

    initial begin
        int pre_err, post_err, det, loc_ok, hld, unheal;
        int model, col, row;
        logic [39:0] val;
        string fname;

        if (!$value$plusargs("NS=%d", NS)) NS = 1100;
        $sformat(fname, "campaign_results%0d.csv", N);

        $readmemh("fault_list4.hex", flist);
        $readmemh("mission4_weights.hex", mw_mem);
        $readmemh("mission4_vectors.hex", mv_mem);

        csv = $fopen(fname, "w");
        $fwrite(csv, "id,model,col,row,val,pre_err,detected,loc_ok,healed,post_err\n");

        rst_n = 1; valid_in = 0; load_w = 0; heal_start = 0;
        a_flat = '0; w_flat = '0;

        for (int id = 0; id < NS; id++) begin
            model = flist[id][47:46];
            col   = flist[id][45:43];
            row   = flist[id][42:40];
            val   = flist[id][39:0];

            apply_reset();
            if (model != 0) inject(model, col, row, val);

            mission_run(pre_err);
            run_heal(det, loc_ok, hld, unheal, col);
            mission_run(post_err);

            if (model != 0) release_inj(model, col, row);

            $fwrite(csv, "%0d,%0d,%0d,%0d,%h,%0d,%0d,%0d,%0d,%0d\n",
                    id, model, col, row, val, pre_err, det, loc_ok, hld, post_err);
            if (id % 100 == 0) $display("[campaign N=%0d] %0d/%0d done", N, id, NS);
        end

        $fclose(csv);
        $display("[campaign N=%0d] ALL %0d SCENARIOS DONE", N, NS);
        $finish;
    end

endmodule
