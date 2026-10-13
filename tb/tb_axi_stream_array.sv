`timescale 1ns/1ps

// =============================================================================
// tb_axi_stream_array.sv — AXI4-Stream 算力核 IP 闭环验证
//
// 验证序列 (激励/黄金由 golden_axi.py 独立生成):
//   P1: 16 权重拍 + 6 行矩阵1 (随机输入间隙 + 随机下行反压) -> 比对黄金
//   P2: force 注入物理列 2 PE 卡死故障 -> 发送自愈命令拍 -> 等待复验完成
//       -> 检查 fault_detected / fault_col==2 / healed
//   P3: 16 权重拍(重装) + 6 行矩阵2 -> 输出必须与矩阵1逐位一致
// =============================================================================

module tb_axi_stream_array;

    localparam N = 4;
    localparam M = 6;
    localparam N_IN  = 45;   // 16W + 6act + 1cmd + 16W + 6act
    localparam N_OUT = 12;   // 2 矩阵 x 6 行
    localparam CLK_PERIOD = 10;

    logic        clk;
    logic        rst_n;
    logic [63:0] s_axis_tdata;
    logic [1:0]  s_axis_tuser;
    logic        s_axis_tvalid;
    logic        s_axis_tready;
    logic        s_axis_tlast;
    logic [63:0] m_axis_tdata;
    logic        m_axis_tvalid;
    logic        m_axis_tready;
    logic        m_axis_tlast;
    logic        sh_busy;
    logic        fault_detected;
    logic        healed;
    logic        unhealable;
    logic [4:0]  fault_col;

    int total_tests  = 0;
    int tests_passed = 0;
    int tests_failed = 0;
    int out_idx      = 0;
    int seed         = 42;

    axi_stream_array #(.N(N)) dut (
        .clk            (clk),
        .rst_n          (rst_n),
        .s_axis_tdata   (s_axis_tdata),
        .s_axis_tuser   (s_axis_tuser),
        .s_axis_tvalid  (s_axis_tvalid),
        .s_axis_tready  (s_axis_tready),
        .s_axis_tlast   (s_axis_tlast),
        .m_axis_tdata   (m_axis_tdata),
        .m_axis_tvalid  (m_axis_tvalid),
        .m_axis_tready  (m_axis_tready),
        .m_axis_tlast   (m_axis_tlast),
        .sh_busy        (sh_busy),
        .fault_detected (fault_detected),
        .healed         (healed),
        .unhealable     (unhealable),
        .fault_col      (fault_col)
    );

    initial begin
        clk = 0;
        forever #(CLK_PERIOD / 2.0) clk = ~clk;
    end

    logic [71:0] in_mem  [0:N_IN-1];
    logic [71:0] out_mem [0:N_OUT-1];

    // -------------------------------------------------------------------------
    // 下行反压: 每拍 70% 概率 ready (随机反压压力测试)
    // -------------------------------------------------------------------------
    always @(negedge clk) begin
        if (!rst_n) m_axis_tready <= 1'b0;
        else        m_axis_tready <= ($urandom(seed) % 100) < 70;
    end

    // -------------------------------------------------------------------------
    // 输出采集与比对 (posedge 采样握手当拍的数据, NBA 更新前读取)
    // -------------------------------------------------------------------------
    always @(posedge clk) begin
        if (rst_n && m_axis_tvalid === 1'b1 && m_axis_tready === 1'b1) begin
            total_tests++;
            if ({m_axis_tlast, m_axis_tdata} === out_mem[out_idx][64:0]) begin
                tests_passed++;
                $display("[PASS] out beat %0d | tdata=%h tlast=%b",
                         out_idx, m_axis_tdata, m_axis_tlast);
            end else begin
                tests_failed++;
                $error("[FAIL] out beat %0d | Expected tdata=%h tlast=%b | Got tdata=%h tlast=%b",
                       out_idx, out_mem[out_idx][63:0], out_mem[out_idx][64],
                       m_axis_tdata, m_axis_tlast);
            end
            out_idx++;
        end
    end

    // -------------------------------------------------------------------------
    // 发送单拍 (随机前置间隙; posedge 判定握手)
    // -------------------------------------------------------------------------
    task automatic send_beat(input int i, input bit gap);
        if (gap) begin
            @(negedge clk); s_axis_tvalid <= 1'b0;
            if (($urandom(seed) % 100) < 30) @(negedge clk);  // 30% 概率额外间隙
        end
        @(negedge clk);
        s_axis_tdata  <= in_mem[i][63:0];
        s_axis_tuser  <= in_mem[i][65:64];
        s_axis_tlast  <= in_mem[i][66];
        s_axis_tvalid <= 1'b1;
        @(posedge clk);
        while (s_axis_tready !== 1'b1) @(posedge clk);
        @(negedge clk);
        s_axis_tvalid <= 1'b0;
    endtask

    task automatic check_flag(input string name, input logic cond);
        total_tests++;
        if (cond) begin
            $display("[PASS] %s", name);
            tests_passed++;
        end else begin
            $error("[FAIL] %s", name);
            tests_failed++;
        end
    endtask

    initial begin
        rst_n = 1;
        s_axis_tvalid = 0;
        s_axis_tdata = '0;
        s_axis_tuser = '0;
        s_axis_tlast = 0;
        m_axis_tready = 0;

        $display("\n========================================");
        $display("AXI4-Stream Array IP Testbench (N=%0d)", N);
        $display("========================================\n");

        $readmemh("axi_in.hex", in_mem);
        $readmemh("axi_out_golden.hex", out_mem);

        // 复位
        @(negedge clk); rst_n = 0;
        repeat(2) @(negedge clk);
        rst_n = 1;
        @(negedge clk);

        // P1: 权重 + 矩阵 1
        $display("--- P1: weights + matrix 1 ---");
        for (int i = 0; i < 22; i++) send_beat(i, i > 0);

        // 排空矩阵 1 输出后再注入故障 (避免污染在飞行中的行)
        begin : drain_m1
            int t;
            t = 0;
            while (out_idx < M && t < 500) begin @(negedge clk); t++; end
            if (out_idx < M) begin
                total_tests++; tests_failed++;
                $error("[FAIL] matrix 1 drain timeout (out_idx=%0d)", out_idx);
            end
        end

        // P2: 注入故障 -> 自愈命令
        $display("--- P2: inject fault @ physical col 2, heal via command beat ---");
        force dut.u_core.u_phy.g_row[1].g_pe[2].u_pe.psum_out = 40'h12_3456_789A;
        repeat(2) @(negedge clk);
        send_beat(22, 1'b0);

        // 等待自愈流程完成
        begin : wait_heal
            int t;
            t = 0;
            while (sh_busy !== 1'b1 && t < 10)  begin @(negedge clk); t++; end
            while (sh_busy === 1'b1 && t < 500) begin @(negedge clk); t++; end
            if (t >= 500) begin
                total_tests++; tests_failed++;
                $error("[FAIL] heal timeout");
            end
        end
        @(negedge clk);
        check_flag("P2.1 fault detected", fault_detected === 1'b1);
        check_flag("P2.2 fault_col == 2", fault_col === 5'd2);
        check_flag("P2.3 healed",         healed === 1'b1);
        check_flag("P2.4 not unhealable", unhealable === 1'b0);

        // P3: 权重重装 + 矩阵 2 (修复后)
        $display("--- P3: reload weights + matrix 2 (healed) ---");
        for (int i = 23; i < N_IN; i++) send_beat(i, 1'b1);

        // 排空
        repeat(20) @(negedge clk);

        total_tests++;
        if (out_idx == N_OUT) begin
            $display("[PASS] output beat count == %0d", N_OUT);
            tests_passed++;
        end else begin
            tests_failed++;
            $error("[FAIL] output beat count = %0d, expected %0d", out_idx, N_OUT);
        end

        $display("\n========================================");
        $display("Test Summary:");
        $display("Total Checks: %0d", total_tests);
        $display("Passed      : %0d", tests_passed);
        $display("Failed      : %0d", tests_failed);
        $display("========================================");

        if (tests_failed == 0) $display(">>> ALL TESTS PASSED <<<\n");
        else $display(">>> SOME TESTS FAILED <<<\n");

        $finish;
    end

endmodule
