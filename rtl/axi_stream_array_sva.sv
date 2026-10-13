`timescale 1ns / 1ps

// =============================================================================
// axi_stream_array_sva.sv — AXI4-Stream 封装层形式化断言
//
//   S1/S2: stall 期间 payload 与控制信号必须稳定 (s_axis / m_axis)
//   S3:    tvalid 不依赖 tready 组合回路的间接证明 —— m_axis 在无 tready 时
//          tvalid 必须保持 (valid 一旦拉起不得自行撤下)
//   S4:    BIST (sh_busy) 期间 s_axis 不得接受任何拍 (tready 必须为 0)
//   S5:    复位期间 s_axis_tready / m_axis_tvalid 均为 0
// =============================================================================

module axi_stream_array_sva (
    input logic        clk,
    input logic        rst_n,
    input logic [63:0] s_axis_tdata,
    input logic [1:0]  s_axis_tuser,
    input logic        s_axis_tvalid,
    input logic        s_axis_tready,
    input logic        s_axis_tlast,
    input logic [63:0] m_axis_tdata,
    input logic        m_axis_tvalid,
    input logic        m_axis_tready,
    input logic        m_axis_tlast,
    input logic        sh_busy
);

    default clocking cb @(posedge clk); endclocking

    // S1: s_axis stall 稳定性
    a_s_stall: assert property (
        disable iff (!rst_n)
        (s_axis_tvalid && !s_axis_tready)
        |=> (s_axis_tvalid && $stable({s_axis_tdata, s_axis_tuser, s_axis_tlast})))
        else $error("[SVA-S1] s_axis stall 期间 payload 变化 (t=%0t)", $time);

    // S2: m_axis stall 稳定性
    a_m_stall: assert property (
        disable iff (!rst_n)
        (m_axis_tvalid && !m_axis_tready)
        |=> (m_axis_tvalid && $stable({m_axis_tdata, m_axis_tlast})))
        else $error("[SVA-S2] m_axis stall 期间 payload 变化 (t=%0t)", $time);

    // S3: m_axis_tvalid 一旦拉起, 在未握手前不得撤下
    a_m_valid_hold: assert property (
        disable iff (!rst_n)
        (m_axis_tvalid && !m_axis_tready) |=> m_axis_tvalid)
        else $error("[SVA-S3] m_axis_tvalid 未握手即撤下 (t=%0t)", $time);

    // S4: BIST 独占
    a_bist_lockout: assert property (
        disable iff (!rst_n)
        sh_busy |-> !s_axis_tready)
        else $error("[SVA-S4] BIST 期间 s_axis_tready 未拉低 (t=%0t)", $time);

    // S5: 复位期间接口静默
    a_reset_idle: assert property (
        !rst_n |-> (!s_axis_tready && !m_axis_tvalid))
        else $error("[SVA-S5] 复位期间接口未静默 (t=%0t)", $time);

    // 覆盖: 见证反压真的发生过 (stall 至少一次) —— 否则 S1/S2 形同虚设
    c_s_stall: cover property (disable iff (!rst_n) s_axis_tvalid && !s_axis_tready);
    c_m_stall: cover property (disable iff (!rst_n) m_axis_tvalid && !m_axis_tready);

endmodule

bind axi_stream_array axi_stream_array_sva u_axi_stream_array_sva (
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
    .sh_busy        (sh_busy)
);
