`timescale 1ns / 1ps
// =============================================================================
// tb_abft_zero_mask_align.sv — Peer Review 证据 TB
// (齐流晶审查 abft_checksum x zero_mask 时创建, 两模块此前无任何 TB)
//
// 方法: 绕过脉动阵列, 按 systolic_array_spec.md §4.2 的列出口拍律直接驱动
//       两模块输入, 精确复现阵列的列间相位差:
//         - 逻辑列 c 的第 r 行数据 C[r][c] 于拍 beat(r,c) = r+N+c+1 到达
//         - 校验列 (物理第 N 列) 的第 r 行均值于拍 K(r) = r+2N+1 到达
//           => 逻辑列 c 的波前比校验列早 (N-c) 拍 (spec §4.2 相位关系)
//       zero_mask 实例 c 按 zero_mask.sv 头注释取 DLY = (N-c+1)。
//
// 契约 (设计意图, 见 zero_mask.sv 头注释 / self_healing_array_sva.sv Z1/Z2):
//   C1 无故障且均值精确: error_pulse 恒 0 (零误报);
//   C2 每个输出拍: out_valid=1 且 out_data = 该行该列期望值 (不得被掩蔽);
//   C3 单列污染: error_pulse 必须恰好落在污染行的输出拍 out(FR) 上,
//      该拍整行掩蔽为 0, 且不得掩蔽任何其他拍 (相邻干净行)。
// =============================================================================
module tb_abft_zero_mask_align;

    localparam int N           = 4;
    localparam int M           = 6;      // 行数
    localparam int FR          = 2;      // 故障注入行
    localparam int FC          = 1;      // 故障注入列
    localparam int FAULT_DELTA = 500;    // 污染增量 (Q1.15 码, > ABFT_THRES)
    localparam int CLK_PERIOD  = 10;

    // ------------------------------------------------------------------ 信号
    logic               clk, rst_n, enable;
    logic [N-1:0]       valid;
    logic [N*16-1:0]    c_flat;
    logic               chk_valid;
    logic signed [15:0] c_chk;
    logic               error_pulse;
    logic signed [15:0] err_abs;

    abft_checksum #(.N(N)) u_abft (
        .clk(clk), .rst_n(rst_n), .enable(enable),
        .valid(valid), .c_flat(c_flat),
        .chk_valid(chk_valid), .c_chk(c_chk),
        .error_pulse(error_pulse), .err_abs(err_abs)
    );

    logic               zm_ov [N];
    logic signed [15:0] zm_od [N];
    genvar gc;
    generate
        for (gc = 0; gc < N; gc++) begin : g_zm
            zero_mask #(.DLY(N-gc+1)) u_zm (
                .clk(clk), .rst_n(rst_n),
                .in_valid(valid[gc]),
                .in_data(c_flat[16*gc +: 16]),
                .mask(error_pulse),
                .out_valid(zm_ov[gc]),
                .out_data(zm_od[gc])
            );
        end
    endgenerate

    initial begin
        clk = 0;
        forever #(CLK_PERIOD / 2.0) clk = ~clk;
    end

    // ------------------------------------------------------------------ 模型
    integer cv    [0:M-1][0:N-1];   // 驱动: 逻辑列输出码
    integer meanv [0:M-1];          // 驱动: 校验列输出码 (干净均值)
    integer pbase;                  // 当前相位起始拍
    integer fault_phase;            // 1 = 故障相位

    function automatic integer beat_of(input integer r, input integer c);
        beat_of = r + N + c + 1;    // spec §4.2: C[m][c] 列出口拍
    endfunction
    function automatic integer k_of(input integer r);
        k_of = r + 2*N + 1;         // 校验列 (物理第 N 列) 出口拍
    endfunction
    function automatic integer out_of(input integer r);
        out_of = r + 2*N + 1;       // zero_mask (DLY=N-c+1) 输出拍 (TB 拍号; 规格拍号 = +1 = m+2N+2)
    endfunction

    // ------------------------------------------------------------------ 统计
    integer n_pulse;
    integer pulse_at [0:63];
    integer n_false_pos;   // C1: 无故障期误报
    integer n_leak;        // C3: 污染行未被掩蔽 (泄漏)
    integer n_overmask;    // C3: 干净拍被误掩蔽
    integer n_baddata;     // C2: 输出拍数据不符
    integer n_badvalid;

    // ------------------------------------------------------------------ 驱动
    task automatic drive_cycle(input integer t);
        integer r, c;
        valid     = '0;
        chk_valid = 1'b0;
        for (r = 0; r < M; r++)
            for (c = 0; c < N; c++)
                if (t == pbase + beat_of(r, c)) begin
                    c_flat[16*c +: 16] = cv[r][c];
                    valid[c]           = 1'b1;
                end
        for (r = 0; r < M; r++)
            if (t == pbase + k_of(r)) begin
                c_chk     = meanv[r];
                chk_valid = 1'b1;
            end
    endtask

    task automatic drive_quiet(input integer t);
        valid     = '0;
        chk_valid = 1'b0;
        c_chk     = '0;
    endtask

    // ------------------------------------------------------------------ 检查
    task automatic check_cycle(input integer t);
        integer r, c;
        logic signed [15:0] got, exp;
        if (error_pulse === 1'b1) begin
            pulse_at[n_pulse] = t;
            n_pulse = n_pulse + 1;
        end
        for (r = 0; r < M; r++) begin
            if (t == pbase + out_of(r)) begin
                for (c = 0; c < N; c++) begin
                    if (zm_ov[c] !== 1'b1) begin
                        n_badvalid = n_badvalid + 1;
                        $display("  [C2] t=%0d out(row%0d,col%0d) out_valid=%b (期望 1)",
                                 t, r, c, zm_ov[c]);
                    end
                    got = zm_od[c];
                    if (fault_phase != 0 && r == FR) exp = 16'sd0;   // 设计意图: 整拍掩蔽
                    else                          exp = cv[r][c];
                    if (got !== exp) begin
                        if (fault_phase != 0 && r == FR) begin
                            n_leak = n_leak + 1;
                            $display("  [C3-LEAK] t=%0d 污染行%0d列%0d 输出=%0d (期望掩蔽为0, 污染值泄漏!)",
                                     t, r, c, got);
                        end else if (fault_phase != 0) begin
                            n_overmask = n_overmask + 1;
                            $display("  [C3-OVERMASK] t=%0d 干净行%0d列%0d 输出=%0d (期望 %0d, 被误掩蔽!)",
                                     t, r, c, got, exp);
                        end else begin
                            n_baddata = n_baddata + 1;
                            $display("  [C2] t=%0d out(row%0d,col%0d) 输出=%0d 期望=%0d (无故障却被置零/不符)",
                                     t, r, c, got, exp);
                        end
                    end
                end
                $display("  [trace] t=%0d out_row=%0d err_pulse=%b err_abs=%0d od=%0d %0d %0d %0d",
                         t, r, error_pulse, err_abs, zm_od[0], zm_od[1], zm_od[2], zm_od[3]);
            end
        end
        if (fault_phase == 0 && error_pulse === 1'b1) begin
            n_false_pos = n_false_pos + 1;
            $display("  [C1-FALSE-POS] t=%0d 无故障期 error_pulse 误报 (err_abs=%0d)", t, err_abs);
        end
        // ---- 审查探针: 逐拍打印 abft 内部对齐状态 (t=7..16) ----
        if (t >= 7 && t <= 16)
            $display("  [probe] t=%0d c_flat=%0d %0d %0d %0d v=%b chkv=%b c_chk=%0d | align=%0d %0d %0d %0d all=%b | sum_r=%0d chk_r=%0d vld=%b pulse=%b eabs=%0d | od=%0d %0d %0d %0d",
                     t, $signed(c_flat[15:0]), $signed(c_flat[31:16]), $signed(c_flat[47:32]), $signed(c_flat[63:48]),
                     valid, chk_valid, $signed(c_chk),
                     $signed(u_abft.align_data[0]), $signed(u_abft.align_data[1]),
                     $signed(u_abft.align_data[2]), $signed(u_abft.align_data[3]),
                     u_abft.all_aligned, $signed(u_abft.sum_r), $signed(u_abft.chk_r), u_abft.vld_r,
                     error_pulse, err_abs,
                     $signed(zm_od[0]), $signed(zm_od[1]), $signed(zm_od[2]), $signed(zm_od[3]));
    endtask

    // ------------------------------------------------------------------ 主流程
    integer j, r, c;
    initial begin
        rst_n     = 0;
        enable    = 0;
        valid     = '0;
        c_flat    = '0;
        chk_valid = 0;
        c_chk     = '0;
        pbase     = 0;
        fault_phase = 0;
        n_pulse = 0; n_false_pos = 0; n_leak = 0; n_overmask = 0; n_baddata = 0; n_badvalid = 0;

        $display("=======================================================");
        $display("tb_abft_zero_mask_align  (N=%0d, 对齐契约证据 TB)", N);
        $display("=======================================================");

        repeat (4) @(negedge clk);
        rst_n  = 1;
        enable = 1;

        // ---------------- Phase A: 无故障, 行均值精确可除, 相邻行均值不同 ----
        // base(r) = 1000 + 200*r  => 若比较器错行配对, |4*base(r)-4*base(r+1)|=800 > THRES
        for (r = 0; r < M; r++)
            for (c = 0; c < N; c++)
                cv[r][c] = 1000 + 200*r;
        for (r = 0; r < M; r++) meanv[r] = 1000 + 200*r;

        $display("--- Phase A: fault-free, exact means, varying row means ---");
        drive_cycle(1);
        for (j = 1; j <= 20; j++) begin
            @(negedge clk);
            check_cycle(j);
            drive_cycle(j + 1);
        end
        $display("--- Phase A summary: pulses=%0d false_pos=%0d baddata=%0d badvalid=%0d (期望全 0) ---",
                 n_pulse, n_false_pos, n_baddata, n_badvalid);

        // ---------------- 静默排空 -------------------------------------------
        for (j = 21; j <= 40; j++) begin
            @(negedge clk);
            check_cycle(j);
            drive_quiet(j + 1);
        end

        // ---------------- Phase B: 单列污染 (FR,FC), 校验列保持干净 -----------
        n_pulse = 0;
        for (r = 0; r < M; r++)
            for (c = 0; c < N; c++)
                cv[r][c] = 1000;
        for (r = 0; r < M; r++) meanv[r] = 1000;
        cv[FR][FC] = 1000 + FAULT_DELTA;      // 污染逻辑列数据; 校验列均值不变

        pbase      = 40;
        fault_phase = 1;
        $display("--- Phase B: single-column fault @ (row%0d,col%0d) delta=%0d ---", FR, FC, FAULT_DELTA);
        drive_cycle(41);
        for (j = 41; j <= 60; j++) begin
            @(negedge clk);
            check_cycle(j);
            drive_cycle(j + 1);
        end
        $display("--- Phase B summary: pulses=%0d leak=%0d overmask=%0d badvalid=%0d ---",
                 n_pulse, n_leak, n_overmask, n_badvalid);
        $display("    Phase B pulse beats:");
        for (j = 0; j < n_pulse; j++)
            $display("      pulse @ t=%0d (out(FR)=%0d, out(FR+1)=%0d)",
                     pulse_at[j], pbase + out_of(FR), pbase + out_of(FR+1));

        // ---------------- 总判定 ---------------------------------------------
        $display("=======================================================");
        if (n_false_pos == 0 && n_baddata == 0 && n_badvalid == 0 &&
            n_pulse == 1 && pulse_at[0] == pbase + out_of(FR) &&
            n_leak == 0 && n_overmask == 0)
            $display(">>> REVIEW-TB: PASS — 斜切对齐/同拍掩蔽契约满足 <<<");
        else
            $display(">>> REVIEW-TB: FAIL — 对齐契约被违反 (off-by-one 证据) <<<");
        $display("=======================================================");
        $finish;
    end

endmodule
