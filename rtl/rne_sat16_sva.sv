`timescale 1ns / 1ps

// =============================================================================
// rne_sat16_sva.sv — 列出口单元 rne_sat16 的形式化断言
// 不变式与 mac_unit_sva 同源 (1 拍延迟、复位约束、输出保持、范围与 X 检查),
// 通过 bind 自动挂载到 systolic_array 的每一个列出口实例。
// =============================================================================

module rne_sat16_sva (
    input logic               clk,
    input logic               rst_n,
    input logic               in_valid,
    input logic               out_valid,
    input logic signed [15:0] out_data
);

    default clocking cb @(posedge clk); endclocking

    // A1: 1 拍延迟正向性
    property p_latency_fwd;
        disable iff (!rst_n)
        in_valid |-> ##1 out_valid;
    endproperty
    a_latency_fwd: assert property (p_latency_fwd)
        else $error("[SVA-R1] in_valid 后 1 拍未出现 out_valid (t=%0t)", $time);

    // A2: 延迟反向充要性 —— 禁止伪脉冲/丢包
    property p_latency_exact;
        (rst_n && $past(rst_n, 1)) |-> (out_valid === $past(in_valid, 1));
    endproperty
    a_latency_exact: assert property (p_latency_exact)
        else $error("[SVA-R2] out_valid 与 1 拍前 in_valid 不一致 (t=%0t)", $time);

    // A3: 复位期间及释放首拍不得产生非法输出有效脉冲
    property p_reset_no_valid;
        (!rst_n || !$past(rst_n)) |-> (out_valid === 1'b0);
    endproperty
    a_reset_no_valid: assert property (p_reset_no_valid)
        else $error("[SVA-R3] 复位期间/释放首拍出现非法 out_valid (t=%0t)", $time);

    // A4: 无效周期输出保持
    property p_out_hold;
        disable iff (!rst_n)
        !in_valid |=> $stable(out_data);
    endproperty
    a_out_hold: assert property (p_out_hold)
        else $error("[SVA-R4] 无效周期 out_data 未保持 (t=%0t)", $time);

    // A5: 输出范围与 X 态 (X 使比较求值为 X, 断言语义下等价于失败)
    property p_out_range;
        disable iff (!rst_n)
        out_valid |-> (out_data >= -16'sd32768 && out_data <= 16'sd32767);
    endproperty
    a_out_range: assert property (p_out_range)
        else $error("[SVA-R5] out_data 越界或含 X 态: %h (t=%0t)", out_data, $time);

    // 覆盖点
    c_sat_pos: cover property (disable iff (!rst_n) out_valid && out_data === 16'sh7FFF);
    c_sat_neg: cover property (disable iff (!rst_n) out_valid && out_data === 16'sh8000);

endmodule

bind rne_sat16 rne_sat16_sva u_rne_sat16_sva (
    .clk       (clk),
    .rst_n     (rst_n),
    .in_valid  (in_valid),
    .out_valid (out_valid),
    .out_data  (out_data)
);
