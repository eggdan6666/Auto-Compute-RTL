`timescale 1ns / 1ps

// =============================================================================
// fault_arbiter.sv — 故障去抖裁决器 (Phase 2 P2-1)
//
// 目的: 在 WINDOW_SIZE 周期滑动窗内统计 ABFT 错误脉冲次数, 区分
//   (a) 可吸收瞬态软错误 (窗内脉冲数 < THRESH):
//       由 zero_mask 优雅降级吸收, 逐拍累加 32 位 transient_cnt 供量化;
//   (b) 致命永久硬故障 (窗内脉冲数 >= THRESH):
//       拉高 1 拍 trigger_heal, 拉起外循环自愈 (BIST + 热备用重映射)。
//   busy=1 (自愈进行中) 时屏蔽错误脉冲, 防止仲裁器重复触发 (反震荡)。
//
// 实现要点:
//   - WINDOW_SIZE 位移位寄存器 + running-sum 计数, 无多级加法组合级联,
//     不破坏关键路径时序 (与内循环 1 级流水求和树的约束一致)。
//   - trigger_heal 由 "窗内计数跨过 THRESH 的上升沿" 提取, 严格 1 拍。
// =============================================================================

module fault_arbiter #(
    parameter int WINDOW_SIZE = 32,   // 滑动观察窗深度
    parameter int THRESH      = 3     // 硬故障触发阈值 (窗内脉冲数)
) (
    input  logic        clk,
    input  logic        rst_n,
    input  logic        err_pulse,     // ABFT 错误单拍脉冲
    input  logic        busy,          // 自愈进行中, 抑制裁决
    output logic        trigger_heal,  // 1 拍硬故障触发脉冲
    output logic [31:0] transient_cnt  // 已吸收瞬态脉冲累计 (论文可测)
);

    localparam int  CNT_W   = $clog2(WINDOW_SIZE + 1);
    localparam logic [CNT_W-1:0] THRESH_W = THRESH;

    logic [WINDOW_SIZE-1:0] window;      // 最近 WINDOW_SIZE 拍的 err 标志 (0/1)
    logic [CNT_W-1:0]       count;       // 窗内 1 的个数 (running sum)

    // 当前拍有效错误 (busy 屏蔽)
    logic err_in;
    assign err_in = err_pulse && !busy;

    // 下一拍窗内容计数: 滑出最旧位, 滑入 err_in (无下溢, count>=window[MSB])
    logic [CNT_W-1:0] count_next;
    assign count_next = count
                      + {{(CNT_W-1){1'b0}}, err_in}
                      - {{(CNT_W-1){1'b0}}, window[WINDOW_SIZE-1]};

    // 硬故障判定 + 上升沿提取 1 拍脉冲
    logic hard_cur;
    logic hard_q;
    assign hard_cur     = (count_next >= THRESH_W);
    assign trigger_heal = hard_cur && !hard_q;

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            window        <= '0;
            count         <= '0;
            hard_q        <= 1'b0;
            transient_cnt <= 32'h0;
        end else begin
            window <= {window[WINDOW_SIZE-2:0], err_in};
            count  <= count_next;
            hard_q <= hard_cur;
            // 未达硬故障阈值的错误脉冲视为瞬态, 被零掩蔽吸收并累计
            if (err_in && !hard_cur)
                transient_cnt <= transient_cnt + 32'd1;
        end
    end

endmodule