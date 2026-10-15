#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""gen_mf_campaign_tb.py — 多故障(multi-fault)注入 TB 生成器（P1 分支 A）

**只产新文件，不触碰任何现有 TB / RTL / 数据**：
  out : tb/tb_fault_campaign_mf_n{N}.sv      （新文件）
  读   : fault_list_mf{N}.hex / mission{N}_weights.hex / mission{N}_vectors.hex
  写   : campaign_results{N}_mf.csv          （与 E1 基线 campaign_results{N}.csv 分开）

为什么单独一支：现有 tb_fault_campaign_n{N}.sv 是 **E1 单故障**批处理流
（heal_start 触发 + BIST 检出）。本支把故障模型扩到 **0/1/2 个卡死故障**，
用于压"二次故障 / 备用列"这类 E1 单故障空间压不到的角落。

fault_list_mf{N}.hex 每行 128bit（32 hex）:
  [127:120] nf      故障数 0..2
  [119:72]  fault1  复用 E1 单故障布局: model[47:46] col[45:43] row[42:40] val[39:0]
  [71:24]   fault2  同上
  [23:16]   cat     场景类别标签（调用方自定义，便于结果分组）
  [15:0]    rsvd

每场景流程与 E1 一致：复位 -> 注入 -> 自愈前任务(基线) -> heal(BIST+定位+重映射+复验)
                    -> 自愈后任务 -> CSV

用法:  python sim/gen_mf_campaign_tb.py 4
"""
import io
import sys

N = int(sys.argv[1]) if len(sys.argv) > 1 else 4
N1 = N + 1
NL = chr(92) + "n"   # SV 字符串里的 \n 转义

force_psum = "\n".join(
    f"                {r*N1+c}: force dut.u_phy.g_row[{r}].g_pe[{c}].u_pe.psum_out = val;"
    for r in range(N) for c in range(N1))
release_psum = "\n".join(
    f"                {r*N1+c}: release dut.u_phy.g_row[{r}].g_pe[{c}].u_pe.psum_out;"
    for r in range(N) for c in range(N1))
force_w = "\n".join(
    f"                {r*N1+c}: force dut.u_phy.g_row[{r}].g_pe[{c}].u_pe.w_reg = val[15:0];"
    for r in range(N) for c in range(N1))
release_w = "\n".join(
    f"                {r*N1+c}: release dut.u_phy.g_row[{r}].g_pe[{c}].u_pe.w_reg;"
    for r in range(N) for c in range(N1))
allcnt = " || ".join(f"cnt[{c}] < M" for c in range(N))

tb = f'''`timescale 1ns/1ps

// tb_fault_campaign_mf_n{N}.sv — 多故障注入回归 TB (N={N}, 由 gen_mf_campaign_tb.py 生成)
// 每场景: 复位 -> 注入 0/1/2 个卡死故障 -> 自愈前任务(基线)
//         -> BIST+定位+重映射+复验 -> 自愈后任务 -> CSV(campaign_results{N}_mf.csv)
// 说明: 走与 E1 相同的 heal_start 批处理流(BIST 检出)，不涉及在线 ABFT/仲裁路径。

module tb_fault_campaign_mf;

    localparam N = {N};
    localparam M = 8;
    localparam CLK_PERIOD = 10;

    logic              clk, rst_n, load_w;
    logic [N*N*16-1:0] w_flat;
    logic              valid_in;
    logic [N*16-1:0]   a_flat;
    logic [N-1:0]      valid_out;
    logic [N*16-1:0]   c_flat;
    logic              heal_start, busy, fault_detected, healed, unhealable;
    logic [4:0]        fault_col;

    integer csv, NS;

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

    logic [127:0]      flist  [0:4095];
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
                while (({allcnt}) && timeout < 600) begin
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

    task automatic run_heal(output int det, output int fcol, output int hld, output int unheal);
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
        fcol   = fault_col;
        hld    = (healed === 1'b1);
        unheal = (unhealable === 1'b1);
    endtask

    task inject_fault(input int model, input int col, input int row, input logic [39:0] val);
        if (model == 1) begin
            case (row*{N1}+col)
{force_psum}
            endcase
        end else if (model == 2) begin
            case (row*{N1}+col)
{force_w}
            endcase
        end
    endtask

    task release_fault(input int model, input int col, input int row);
        if (model == 1) begin
            case (row*{N1}+col)
{release_psum}
            endcase
        end else if (model == 2) begin
            case (row*{N1}+col)
{release_w}
            endcase
        end
    endtask

    initial begin
        int pre_err, post_err, det, hcol, hld, unheal;
        int nf, cat, f1m, f1c, f1r, f2m, f2c, f2r;
        logic [39:0] f1v, f2v;
        logic [47:0] w1, w2;
        string fname;

        if (!$value$plusargs("NS=%d", NS)) NS = 1100;
        $sformat(fname, "campaign_results%0d_mf.csv", N);

        $readmemh("fault_list_mf{N}.hex", flist);
        $readmemh("mission{N}_weights.hex", mw_mem);
        $readmemh("mission{N}_vectors.hex", mv_mem);

        csv = $fopen(fname, "w");
        $fwrite(csv, "id,nf,cat,f1m,f1c,f1r,f1v,f2m,f2c,f2r,f2v,pre_err,detected,fault_col,healed,unhealable,post_err{NL}");

        rst_n = 1; valid_in = 0; load_w = 0; heal_start = 0;
        a_flat = '0; w_flat = '0;

        for (int id = 0; id < NS; id++) begin
            nf  = flist[id][127:120];
            w1  = flist[id][119:72];
            w2  = flist[id][71:24];
            cat = flist[id][23:16];
            f1m = w1[47:46]; f1c = w1[45:43]; f1r = w1[42:40]; f1v = w1[39:0];
            f2m = w2[47:46]; f2c = w2[45:43]; f2r = w2[42:40]; f2v = w2[39:0];

            apply_reset();
            if (nf >= 1 && f1m != 0) inject_fault(f1m, f1c, f1r, f1v);
            if (nf >= 2 && f2m != 0) inject_fault(f2m, f2c, f2r, f2v);

            mission_run(pre_err);
            run_heal(det, hcol, hld, unheal);
            mission_run(post_err);

            if (nf >= 1 && f1m != 0) release_fault(f1m, f1c, f1r);
            if (nf >= 2 && f2m != 0) release_fault(f2m, f2c, f2r);

            $fwrite(csv, "%0d,%0d,%0d,%0d,%0d,%0d,%h,%0d,%0d,%0d,%h,%0d,%0d,%0d,%0d,%0d,%0d{NL}",
                    id, nf, cat,
                    f1m, f1c, f1r, f1v, f2m, f2c, f2r, f2v,
                    pre_err, det, hcol, hld, unheal, post_err);
            if (id % 100 == 0) $display("[mf-campaign N=%0d] %0d/%0d done", N, id, NS);
        end

        $fclose(csv);
        $display("[mf-campaign N=%0d] ALL %0d SCENARIOS DONE", N, NS);
        $finish;
    end

endmodule
'''

out = f"tb/tb_fault_campaign_mf_n{N}.sv"
io.open(out, "w", encoding="utf-8", newline="").write(tb)
print(f"{out} generated ({len(tb)} bytes, N={N}, {N}x{N1} PE force table x2 fault slots)")
