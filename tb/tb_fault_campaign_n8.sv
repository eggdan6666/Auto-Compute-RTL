`timescale 1ns/1ps

// tb_fault_campaign_n8.sv — 批量故障注入回归 TB (N=8, 由 gen_campaign_tb.py 生成)
// 每场景: 复位 -> 注入(可选) -> 自愈前任务(基线) -> BIST+定位+重映射+复验 -> 自愈后任务 -> CSV

module tb_fault_campaign;

    localparam N = 8;
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
                while ((cnt[0] < M || cnt[1] < M || cnt[2] < M || cnt[3] < M || cnt[4] < M || cnt[5] < M || cnt[6] < M || cnt[7] < M) && timeout < 600) begin
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
            case (row*9+col)
            0: force dut.u_phy.g_row[0].g_pe[0].u_pe.psum_out = val;
            1: force dut.u_phy.g_row[0].g_pe[1].u_pe.psum_out = val;
            2: force dut.u_phy.g_row[0].g_pe[2].u_pe.psum_out = val;
            3: force dut.u_phy.g_row[0].g_pe[3].u_pe.psum_out = val;
            4: force dut.u_phy.g_row[0].g_pe[4].u_pe.psum_out = val;
            5: force dut.u_phy.g_row[0].g_pe[5].u_pe.psum_out = val;
            6: force dut.u_phy.g_row[0].g_pe[6].u_pe.psum_out = val;
            7: force dut.u_phy.g_row[0].g_pe[7].u_pe.psum_out = val;
            8: force dut.u_phy.g_row[0].g_pe[8].u_pe.psum_out = val;
            9: force dut.u_phy.g_row[1].g_pe[0].u_pe.psum_out = val;
            10: force dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out = val;
            11: force dut.u_phy.g_row[1].g_pe[2].u_pe.psum_out = val;
            12: force dut.u_phy.g_row[1].g_pe[3].u_pe.psum_out = val;
            13: force dut.u_phy.g_row[1].g_pe[4].u_pe.psum_out = val;
            14: force dut.u_phy.g_row[1].g_pe[5].u_pe.psum_out = val;
            15: force dut.u_phy.g_row[1].g_pe[6].u_pe.psum_out = val;
            16: force dut.u_phy.g_row[1].g_pe[7].u_pe.psum_out = val;
            17: force dut.u_phy.g_row[1].g_pe[8].u_pe.psum_out = val;
            18: force dut.u_phy.g_row[2].g_pe[0].u_pe.psum_out = val;
            19: force dut.u_phy.g_row[2].g_pe[1].u_pe.psum_out = val;
            20: force dut.u_phy.g_row[2].g_pe[2].u_pe.psum_out = val;
            21: force dut.u_phy.g_row[2].g_pe[3].u_pe.psum_out = val;
            22: force dut.u_phy.g_row[2].g_pe[4].u_pe.psum_out = val;
            23: force dut.u_phy.g_row[2].g_pe[5].u_pe.psum_out = val;
            24: force dut.u_phy.g_row[2].g_pe[6].u_pe.psum_out = val;
            25: force dut.u_phy.g_row[2].g_pe[7].u_pe.psum_out = val;
            26: force dut.u_phy.g_row[2].g_pe[8].u_pe.psum_out = val;
            27: force dut.u_phy.g_row[3].g_pe[0].u_pe.psum_out = val;
            28: force dut.u_phy.g_row[3].g_pe[1].u_pe.psum_out = val;
            29: force dut.u_phy.g_row[3].g_pe[2].u_pe.psum_out = val;
            30: force dut.u_phy.g_row[3].g_pe[3].u_pe.psum_out = val;
            31: force dut.u_phy.g_row[3].g_pe[4].u_pe.psum_out = val;
            32: force dut.u_phy.g_row[3].g_pe[5].u_pe.psum_out = val;
            33: force dut.u_phy.g_row[3].g_pe[6].u_pe.psum_out = val;
            34: force dut.u_phy.g_row[3].g_pe[7].u_pe.psum_out = val;
            35: force dut.u_phy.g_row[3].g_pe[8].u_pe.psum_out = val;
            36: force dut.u_phy.g_row[4].g_pe[0].u_pe.psum_out = val;
            37: force dut.u_phy.g_row[4].g_pe[1].u_pe.psum_out = val;
            38: force dut.u_phy.g_row[4].g_pe[2].u_pe.psum_out = val;
            39: force dut.u_phy.g_row[4].g_pe[3].u_pe.psum_out = val;
            40: force dut.u_phy.g_row[4].g_pe[4].u_pe.psum_out = val;
            41: force dut.u_phy.g_row[4].g_pe[5].u_pe.psum_out = val;
            42: force dut.u_phy.g_row[4].g_pe[6].u_pe.psum_out = val;
            43: force dut.u_phy.g_row[4].g_pe[7].u_pe.psum_out = val;
            44: force dut.u_phy.g_row[4].g_pe[8].u_pe.psum_out = val;
            45: force dut.u_phy.g_row[5].g_pe[0].u_pe.psum_out = val;
            46: force dut.u_phy.g_row[5].g_pe[1].u_pe.psum_out = val;
            47: force dut.u_phy.g_row[5].g_pe[2].u_pe.psum_out = val;
            48: force dut.u_phy.g_row[5].g_pe[3].u_pe.psum_out = val;
            49: force dut.u_phy.g_row[5].g_pe[4].u_pe.psum_out = val;
            50: force dut.u_phy.g_row[5].g_pe[5].u_pe.psum_out = val;
            51: force dut.u_phy.g_row[5].g_pe[6].u_pe.psum_out = val;
            52: force dut.u_phy.g_row[5].g_pe[7].u_pe.psum_out = val;
            53: force dut.u_phy.g_row[5].g_pe[8].u_pe.psum_out = val;
            54: force dut.u_phy.g_row[6].g_pe[0].u_pe.psum_out = val;
            55: force dut.u_phy.g_row[6].g_pe[1].u_pe.psum_out = val;
            56: force dut.u_phy.g_row[6].g_pe[2].u_pe.psum_out = val;
            57: force dut.u_phy.g_row[6].g_pe[3].u_pe.psum_out = val;
            58: force dut.u_phy.g_row[6].g_pe[4].u_pe.psum_out = val;
            59: force dut.u_phy.g_row[6].g_pe[5].u_pe.psum_out = val;
            60: force dut.u_phy.g_row[6].g_pe[6].u_pe.psum_out = val;
            61: force dut.u_phy.g_row[6].g_pe[7].u_pe.psum_out = val;
            62: force dut.u_phy.g_row[6].g_pe[8].u_pe.psum_out = val;
            63: force dut.u_phy.g_row[7].g_pe[0].u_pe.psum_out = val;
            64: force dut.u_phy.g_row[7].g_pe[1].u_pe.psum_out = val;
            65: force dut.u_phy.g_row[7].g_pe[2].u_pe.psum_out = val;
            66: force dut.u_phy.g_row[7].g_pe[3].u_pe.psum_out = val;
            67: force dut.u_phy.g_row[7].g_pe[4].u_pe.psum_out = val;
            68: force dut.u_phy.g_row[7].g_pe[5].u_pe.psum_out = val;
            69: force dut.u_phy.g_row[7].g_pe[6].u_pe.psum_out = val;
            70: force dut.u_phy.g_row[7].g_pe[7].u_pe.psum_out = val;
            71: force dut.u_phy.g_row[7].g_pe[8].u_pe.psum_out = val;
            endcase
        end else if (model == 2) begin
            case (row*9+col)
            0: force dut.u_phy.g_row[0].g_pe[0].u_pe.w_reg = val[15:0];
            1: force dut.u_phy.g_row[0].g_pe[1].u_pe.w_reg = val[15:0];
            2: force dut.u_phy.g_row[0].g_pe[2].u_pe.w_reg = val[15:0];
            3: force dut.u_phy.g_row[0].g_pe[3].u_pe.w_reg = val[15:0];
            4: force dut.u_phy.g_row[0].g_pe[4].u_pe.w_reg = val[15:0];
            5: force dut.u_phy.g_row[0].g_pe[5].u_pe.w_reg = val[15:0];
            6: force dut.u_phy.g_row[0].g_pe[6].u_pe.w_reg = val[15:0];
            7: force dut.u_phy.g_row[0].g_pe[7].u_pe.w_reg = val[15:0];
            8: force dut.u_phy.g_row[0].g_pe[8].u_pe.w_reg = val[15:0];
            9: force dut.u_phy.g_row[1].g_pe[0].u_pe.w_reg = val[15:0];
            10: force dut.u_phy.g_row[1].g_pe[1].u_pe.w_reg = val[15:0];
            11: force dut.u_phy.g_row[1].g_pe[2].u_pe.w_reg = val[15:0];
            12: force dut.u_phy.g_row[1].g_pe[3].u_pe.w_reg = val[15:0];
            13: force dut.u_phy.g_row[1].g_pe[4].u_pe.w_reg = val[15:0];
            14: force dut.u_phy.g_row[1].g_pe[5].u_pe.w_reg = val[15:0];
            15: force dut.u_phy.g_row[1].g_pe[6].u_pe.w_reg = val[15:0];
            16: force dut.u_phy.g_row[1].g_pe[7].u_pe.w_reg = val[15:0];
            17: force dut.u_phy.g_row[1].g_pe[8].u_pe.w_reg = val[15:0];
            18: force dut.u_phy.g_row[2].g_pe[0].u_pe.w_reg = val[15:0];
            19: force dut.u_phy.g_row[2].g_pe[1].u_pe.w_reg = val[15:0];
            20: force dut.u_phy.g_row[2].g_pe[2].u_pe.w_reg = val[15:0];
            21: force dut.u_phy.g_row[2].g_pe[3].u_pe.w_reg = val[15:0];
            22: force dut.u_phy.g_row[2].g_pe[4].u_pe.w_reg = val[15:0];
            23: force dut.u_phy.g_row[2].g_pe[5].u_pe.w_reg = val[15:0];
            24: force dut.u_phy.g_row[2].g_pe[6].u_pe.w_reg = val[15:0];
            25: force dut.u_phy.g_row[2].g_pe[7].u_pe.w_reg = val[15:0];
            26: force dut.u_phy.g_row[2].g_pe[8].u_pe.w_reg = val[15:0];
            27: force dut.u_phy.g_row[3].g_pe[0].u_pe.w_reg = val[15:0];
            28: force dut.u_phy.g_row[3].g_pe[1].u_pe.w_reg = val[15:0];
            29: force dut.u_phy.g_row[3].g_pe[2].u_pe.w_reg = val[15:0];
            30: force dut.u_phy.g_row[3].g_pe[3].u_pe.w_reg = val[15:0];
            31: force dut.u_phy.g_row[3].g_pe[4].u_pe.w_reg = val[15:0];
            32: force dut.u_phy.g_row[3].g_pe[5].u_pe.w_reg = val[15:0];
            33: force dut.u_phy.g_row[3].g_pe[6].u_pe.w_reg = val[15:0];
            34: force dut.u_phy.g_row[3].g_pe[7].u_pe.w_reg = val[15:0];
            35: force dut.u_phy.g_row[3].g_pe[8].u_pe.w_reg = val[15:0];
            36: force dut.u_phy.g_row[4].g_pe[0].u_pe.w_reg = val[15:0];
            37: force dut.u_phy.g_row[4].g_pe[1].u_pe.w_reg = val[15:0];
            38: force dut.u_phy.g_row[4].g_pe[2].u_pe.w_reg = val[15:0];
            39: force dut.u_phy.g_row[4].g_pe[3].u_pe.w_reg = val[15:0];
            40: force dut.u_phy.g_row[4].g_pe[4].u_pe.w_reg = val[15:0];
            41: force dut.u_phy.g_row[4].g_pe[5].u_pe.w_reg = val[15:0];
            42: force dut.u_phy.g_row[4].g_pe[6].u_pe.w_reg = val[15:0];
            43: force dut.u_phy.g_row[4].g_pe[7].u_pe.w_reg = val[15:0];
            44: force dut.u_phy.g_row[4].g_pe[8].u_pe.w_reg = val[15:0];
            45: force dut.u_phy.g_row[5].g_pe[0].u_pe.w_reg = val[15:0];
            46: force dut.u_phy.g_row[5].g_pe[1].u_pe.w_reg = val[15:0];
            47: force dut.u_phy.g_row[5].g_pe[2].u_pe.w_reg = val[15:0];
            48: force dut.u_phy.g_row[5].g_pe[3].u_pe.w_reg = val[15:0];
            49: force dut.u_phy.g_row[5].g_pe[4].u_pe.w_reg = val[15:0];
            50: force dut.u_phy.g_row[5].g_pe[5].u_pe.w_reg = val[15:0];
            51: force dut.u_phy.g_row[5].g_pe[6].u_pe.w_reg = val[15:0];
            52: force dut.u_phy.g_row[5].g_pe[7].u_pe.w_reg = val[15:0];
            53: force dut.u_phy.g_row[5].g_pe[8].u_pe.w_reg = val[15:0];
            54: force dut.u_phy.g_row[6].g_pe[0].u_pe.w_reg = val[15:0];
            55: force dut.u_phy.g_row[6].g_pe[1].u_pe.w_reg = val[15:0];
            56: force dut.u_phy.g_row[6].g_pe[2].u_pe.w_reg = val[15:0];
            57: force dut.u_phy.g_row[6].g_pe[3].u_pe.w_reg = val[15:0];
            58: force dut.u_phy.g_row[6].g_pe[4].u_pe.w_reg = val[15:0];
            59: force dut.u_phy.g_row[6].g_pe[5].u_pe.w_reg = val[15:0];
            60: force dut.u_phy.g_row[6].g_pe[6].u_pe.w_reg = val[15:0];
            61: force dut.u_phy.g_row[6].g_pe[7].u_pe.w_reg = val[15:0];
            62: force dut.u_phy.g_row[6].g_pe[8].u_pe.w_reg = val[15:0];
            63: force dut.u_phy.g_row[7].g_pe[0].u_pe.w_reg = val[15:0];
            64: force dut.u_phy.g_row[7].g_pe[1].u_pe.w_reg = val[15:0];
            65: force dut.u_phy.g_row[7].g_pe[2].u_pe.w_reg = val[15:0];
            66: force dut.u_phy.g_row[7].g_pe[3].u_pe.w_reg = val[15:0];
            67: force dut.u_phy.g_row[7].g_pe[4].u_pe.w_reg = val[15:0];
            68: force dut.u_phy.g_row[7].g_pe[5].u_pe.w_reg = val[15:0];
            69: force dut.u_phy.g_row[7].g_pe[6].u_pe.w_reg = val[15:0];
            70: force dut.u_phy.g_row[7].g_pe[7].u_pe.w_reg = val[15:0];
            71: force dut.u_phy.g_row[7].g_pe[8].u_pe.w_reg = val[15:0];
            endcase
        end
    endtask

    task release_inj(input int model, input int col, input int row);
        if (model == 1) begin
            case (row*9+col)
            0: release dut.u_phy.g_row[0].g_pe[0].u_pe.psum_out;
            1: release dut.u_phy.g_row[0].g_pe[1].u_pe.psum_out;
            2: release dut.u_phy.g_row[0].g_pe[2].u_pe.psum_out;
            3: release dut.u_phy.g_row[0].g_pe[3].u_pe.psum_out;
            4: release dut.u_phy.g_row[0].g_pe[4].u_pe.psum_out;
            5: release dut.u_phy.g_row[0].g_pe[5].u_pe.psum_out;
            6: release dut.u_phy.g_row[0].g_pe[6].u_pe.psum_out;
            7: release dut.u_phy.g_row[0].g_pe[7].u_pe.psum_out;
            8: release dut.u_phy.g_row[0].g_pe[8].u_pe.psum_out;
            9: release dut.u_phy.g_row[1].g_pe[0].u_pe.psum_out;
            10: release dut.u_phy.g_row[1].g_pe[1].u_pe.psum_out;
            11: release dut.u_phy.g_row[1].g_pe[2].u_pe.psum_out;
            12: release dut.u_phy.g_row[1].g_pe[3].u_pe.psum_out;
            13: release dut.u_phy.g_row[1].g_pe[4].u_pe.psum_out;
            14: release dut.u_phy.g_row[1].g_pe[5].u_pe.psum_out;
            15: release dut.u_phy.g_row[1].g_pe[6].u_pe.psum_out;
            16: release dut.u_phy.g_row[1].g_pe[7].u_pe.psum_out;
            17: release dut.u_phy.g_row[1].g_pe[8].u_pe.psum_out;
            18: release dut.u_phy.g_row[2].g_pe[0].u_pe.psum_out;
            19: release dut.u_phy.g_row[2].g_pe[1].u_pe.psum_out;
            20: release dut.u_phy.g_row[2].g_pe[2].u_pe.psum_out;
            21: release dut.u_phy.g_row[2].g_pe[3].u_pe.psum_out;
            22: release dut.u_phy.g_row[2].g_pe[4].u_pe.psum_out;
            23: release dut.u_phy.g_row[2].g_pe[5].u_pe.psum_out;
            24: release dut.u_phy.g_row[2].g_pe[6].u_pe.psum_out;
            25: release dut.u_phy.g_row[2].g_pe[7].u_pe.psum_out;
            26: release dut.u_phy.g_row[2].g_pe[8].u_pe.psum_out;
            27: release dut.u_phy.g_row[3].g_pe[0].u_pe.psum_out;
            28: release dut.u_phy.g_row[3].g_pe[1].u_pe.psum_out;
            29: release dut.u_phy.g_row[3].g_pe[2].u_pe.psum_out;
            30: release dut.u_phy.g_row[3].g_pe[3].u_pe.psum_out;
            31: release dut.u_phy.g_row[3].g_pe[4].u_pe.psum_out;
            32: release dut.u_phy.g_row[3].g_pe[5].u_pe.psum_out;
            33: release dut.u_phy.g_row[3].g_pe[6].u_pe.psum_out;
            34: release dut.u_phy.g_row[3].g_pe[7].u_pe.psum_out;
            35: release dut.u_phy.g_row[3].g_pe[8].u_pe.psum_out;
            36: release dut.u_phy.g_row[4].g_pe[0].u_pe.psum_out;
            37: release dut.u_phy.g_row[4].g_pe[1].u_pe.psum_out;
            38: release dut.u_phy.g_row[4].g_pe[2].u_pe.psum_out;
            39: release dut.u_phy.g_row[4].g_pe[3].u_pe.psum_out;
            40: release dut.u_phy.g_row[4].g_pe[4].u_pe.psum_out;
            41: release dut.u_phy.g_row[4].g_pe[5].u_pe.psum_out;
            42: release dut.u_phy.g_row[4].g_pe[6].u_pe.psum_out;
            43: release dut.u_phy.g_row[4].g_pe[7].u_pe.psum_out;
            44: release dut.u_phy.g_row[4].g_pe[8].u_pe.psum_out;
            45: release dut.u_phy.g_row[5].g_pe[0].u_pe.psum_out;
            46: release dut.u_phy.g_row[5].g_pe[1].u_pe.psum_out;
            47: release dut.u_phy.g_row[5].g_pe[2].u_pe.psum_out;
            48: release dut.u_phy.g_row[5].g_pe[3].u_pe.psum_out;
            49: release dut.u_phy.g_row[5].g_pe[4].u_pe.psum_out;
            50: release dut.u_phy.g_row[5].g_pe[5].u_pe.psum_out;
            51: release dut.u_phy.g_row[5].g_pe[6].u_pe.psum_out;
            52: release dut.u_phy.g_row[5].g_pe[7].u_pe.psum_out;
            53: release dut.u_phy.g_row[5].g_pe[8].u_pe.psum_out;
            54: release dut.u_phy.g_row[6].g_pe[0].u_pe.psum_out;
            55: release dut.u_phy.g_row[6].g_pe[1].u_pe.psum_out;
            56: release dut.u_phy.g_row[6].g_pe[2].u_pe.psum_out;
            57: release dut.u_phy.g_row[6].g_pe[3].u_pe.psum_out;
            58: release dut.u_phy.g_row[6].g_pe[4].u_pe.psum_out;
            59: release dut.u_phy.g_row[6].g_pe[5].u_pe.psum_out;
            60: release dut.u_phy.g_row[6].g_pe[6].u_pe.psum_out;
            61: release dut.u_phy.g_row[6].g_pe[7].u_pe.psum_out;
            62: release dut.u_phy.g_row[6].g_pe[8].u_pe.psum_out;
            63: release dut.u_phy.g_row[7].g_pe[0].u_pe.psum_out;
            64: release dut.u_phy.g_row[7].g_pe[1].u_pe.psum_out;
            65: release dut.u_phy.g_row[7].g_pe[2].u_pe.psum_out;
            66: release dut.u_phy.g_row[7].g_pe[3].u_pe.psum_out;
            67: release dut.u_phy.g_row[7].g_pe[4].u_pe.psum_out;
            68: release dut.u_phy.g_row[7].g_pe[5].u_pe.psum_out;
            69: release dut.u_phy.g_row[7].g_pe[6].u_pe.psum_out;
            70: release dut.u_phy.g_row[7].g_pe[7].u_pe.psum_out;
            71: release dut.u_phy.g_row[7].g_pe[8].u_pe.psum_out;
            endcase
        end else if (model == 2) begin
            case (row*9+col)
            0: release dut.u_phy.g_row[0].g_pe[0].u_pe.w_reg;
            1: release dut.u_phy.g_row[0].g_pe[1].u_pe.w_reg;
            2: release dut.u_phy.g_row[0].g_pe[2].u_pe.w_reg;
            3: release dut.u_phy.g_row[0].g_pe[3].u_pe.w_reg;
            4: release dut.u_phy.g_row[0].g_pe[4].u_pe.w_reg;
            5: release dut.u_phy.g_row[0].g_pe[5].u_pe.w_reg;
            6: release dut.u_phy.g_row[0].g_pe[6].u_pe.w_reg;
            7: release dut.u_phy.g_row[0].g_pe[7].u_pe.w_reg;
            8: release dut.u_phy.g_row[0].g_pe[8].u_pe.w_reg;
            9: release dut.u_phy.g_row[1].g_pe[0].u_pe.w_reg;
            10: release dut.u_phy.g_row[1].g_pe[1].u_pe.w_reg;
            11: release dut.u_phy.g_row[1].g_pe[2].u_pe.w_reg;
            12: release dut.u_phy.g_row[1].g_pe[3].u_pe.w_reg;
            13: release dut.u_phy.g_row[1].g_pe[4].u_pe.w_reg;
            14: release dut.u_phy.g_row[1].g_pe[5].u_pe.w_reg;
            15: release dut.u_phy.g_row[1].g_pe[6].u_pe.w_reg;
            16: release dut.u_phy.g_row[1].g_pe[7].u_pe.w_reg;
            17: release dut.u_phy.g_row[1].g_pe[8].u_pe.w_reg;
            18: release dut.u_phy.g_row[2].g_pe[0].u_pe.w_reg;
            19: release dut.u_phy.g_row[2].g_pe[1].u_pe.w_reg;
            20: release dut.u_phy.g_row[2].g_pe[2].u_pe.w_reg;
            21: release dut.u_phy.g_row[2].g_pe[3].u_pe.w_reg;
            22: release dut.u_phy.g_row[2].g_pe[4].u_pe.w_reg;
            23: release dut.u_phy.g_row[2].g_pe[5].u_pe.w_reg;
            24: release dut.u_phy.g_row[2].g_pe[6].u_pe.w_reg;
            25: release dut.u_phy.g_row[2].g_pe[7].u_pe.w_reg;
            26: release dut.u_phy.g_row[2].g_pe[8].u_pe.w_reg;
            27: release dut.u_phy.g_row[3].g_pe[0].u_pe.w_reg;
            28: release dut.u_phy.g_row[3].g_pe[1].u_pe.w_reg;
            29: release dut.u_phy.g_row[3].g_pe[2].u_pe.w_reg;
            30: release dut.u_phy.g_row[3].g_pe[3].u_pe.w_reg;
            31: release dut.u_phy.g_row[3].g_pe[4].u_pe.w_reg;
            32: release dut.u_phy.g_row[3].g_pe[5].u_pe.w_reg;
            33: release dut.u_phy.g_row[3].g_pe[6].u_pe.w_reg;
            34: release dut.u_phy.g_row[3].g_pe[7].u_pe.w_reg;
            35: release dut.u_phy.g_row[3].g_pe[8].u_pe.w_reg;
            36: release dut.u_phy.g_row[4].g_pe[0].u_pe.w_reg;
            37: release dut.u_phy.g_row[4].g_pe[1].u_pe.w_reg;
            38: release dut.u_phy.g_row[4].g_pe[2].u_pe.w_reg;
            39: release dut.u_phy.g_row[4].g_pe[3].u_pe.w_reg;
            40: release dut.u_phy.g_row[4].g_pe[4].u_pe.w_reg;
            41: release dut.u_phy.g_row[4].g_pe[5].u_pe.w_reg;
            42: release dut.u_phy.g_row[4].g_pe[6].u_pe.w_reg;
            43: release dut.u_phy.g_row[4].g_pe[7].u_pe.w_reg;
            44: release dut.u_phy.g_row[4].g_pe[8].u_pe.w_reg;
            45: release dut.u_phy.g_row[5].g_pe[0].u_pe.w_reg;
            46: release dut.u_phy.g_row[5].g_pe[1].u_pe.w_reg;
            47: release dut.u_phy.g_row[5].g_pe[2].u_pe.w_reg;
            48: release dut.u_phy.g_row[5].g_pe[3].u_pe.w_reg;
            49: release dut.u_phy.g_row[5].g_pe[4].u_pe.w_reg;
            50: release dut.u_phy.g_row[5].g_pe[5].u_pe.w_reg;
            51: release dut.u_phy.g_row[5].g_pe[6].u_pe.w_reg;
            52: release dut.u_phy.g_row[5].g_pe[7].u_pe.w_reg;
            53: release dut.u_phy.g_row[5].g_pe[8].u_pe.w_reg;
            54: release dut.u_phy.g_row[6].g_pe[0].u_pe.w_reg;
            55: release dut.u_phy.g_row[6].g_pe[1].u_pe.w_reg;
            56: release dut.u_phy.g_row[6].g_pe[2].u_pe.w_reg;
            57: release dut.u_phy.g_row[6].g_pe[3].u_pe.w_reg;
            58: release dut.u_phy.g_row[6].g_pe[4].u_pe.w_reg;
            59: release dut.u_phy.g_row[6].g_pe[5].u_pe.w_reg;
            60: release dut.u_phy.g_row[6].g_pe[6].u_pe.w_reg;
            61: release dut.u_phy.g_row[6].g_pe[7].u_pe.w_reg;
            62: release dut.u_phy.g_row[6].g_pe[8].u_pe.w_reg;
            63: release dut.u_phy.g_row[7].g_pe[0].u_pe.w_reg;
            64: release dut.u_phy.g_row[7].g_pe[1].u_pe.w_reg;
            65: release dut.u_phy.g_row[7].g_pe[2].u_pe.w_reg;
            66: release dut.u_phy.g_row[7].g_pe[3].u_pe.w_reg;
            67: release dut.u_phy.g_row[7].g_pe[4].u_pe.w_reg;
            68: release dut.u_phy.g_row[7].g_pe[5].u_pe.w_reg;
            69: release dut.u_phy.g_row[7].g_pe[6].u_pe.w_reg;
            70: release dut.u_phy.g_row[7].g_pe[7].u_pe.w_reg;
            71: release dut.u_phy.g_row[7].g_pe[8].u_pe.w_reg;
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

        $readmemh("fault_list8.hex", flist);
        $readmemh("mission8_weights.hex", mw_mem);
        $readmemh("mission8_vectors.hex", mv_mem);

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
