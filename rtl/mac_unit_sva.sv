`timescale 1ns / 1ps

// =============================================================================
// mac_unit_sva.sv — mac_unit 工业级形式化断言模块
//
// 依据《MAC_microarchitecture_spec.md》第 2/3/5/6 节定义的时序与数学不变式。
// 通过文件末尾的 bind 语句探入 DUT, 其中 valid_s1 连接 DUT 内部 PR1 有效位
// (valid_s1_reg), 用于精确检查输出保持行为。
//
// 需要支持 SVA 的仿真器 (VCS / Xcelium / Questa)。Verilator 需在 --assert
// 模式下使用, 部分 ## 延迟序列支持视版本而定。
// =============================================================================

module mac_unit_sva (
    input logic               clk,
    input logic               rst_n,
    input logic               valid_in,
    input logic               valid_s1,    // bind 探入: DUT 内部 valid_s1_reg
    input logic               Valid_out,
    input logic signed [15:0] Out
);

    default clocking cb @(posedge clk); endclocking

    // -------------------------------------------------------------------------
    // A1: 流水延迟正向性 —— valid_in 之后恰好 2 拍必须出现 Valid_out
    //     Spec 3.6: Valid_out[N+2] = valid_in[N]
    // -------------------------------------------------------------------------
    property p_latency_fwd;
        disable iff (!rst_n)
        valid_in |-> ##2 Valid_out;
    endproperty
    a_latency_fwd: assert property (p_latency_fwd)
        else $error("[SVA-A1] valid_in 后 2 拍未出现 Valid_out (t=%0t)", $time);

    // -------------------------------------------------------------------------
    // A2: 流水延迟反向充要性 —— 任何 Valid_out 必须可追溯至 2 拍前的 valid_in,
    //     禁止伪脉冲/重影脉冲。复位释放后 2 拍内 $past 无意义, 由 A3 覆盖该窗口。
    // -------------------------------------------------------------------------
    property p_latency_exact;
        (rst_n && $past(rst_n, 1) && $past(rst_n, 2))
        |-> (Valid_out === $past(valid_in, 2));
    endproperty
    a_latency_exact: assert property (p_latency_exact)
        else $error("[SVA-A2] Valid_out 与 2 拍前 valid_in 不一致 (伪脉冲或丢包, t=%0t)", $time);

    // -------------------------------------------------------------------------
    // A3: 复位约束 —— 复位期间及复位释放首拍不得产生非法输出有效脉冲
    //     Spec 2/6: rst_n=0 时 Valid_out 清零; 释放后首笔事务需完整 2 拍延迟
    // -------------------------------------------------------------------------
    property p_reset_no_valid;
        (!rst_n || !$past(rst_n)) |-> (Valid_out === 1'b0);
    endproperty
    a_reset_no_valid: assert property (p_reset_no_valid)
        else $error("[SVA-A3] 复位期间/释放首拍出现非法 Valid_out (t=%0t)", $time);

    // -------------------------------------------------------------------------
    // A4: 无效周期输出保持 —— valid_s1=0 时 Out 必须保持上一拍有效值
    //     Spec 3.5: valid_s1=0 -> Valid_out 清零, Out 保持不变
    // -------------------------------------------------------------------------
    property p_out_hold;
        disable iff (!rst_n)
        !valid_s1 |=> $stable(Out);
    endproperty
    a_out_hold: assert property (p_out_hold)
        else $error("[SVA-A4] 无效周期 Out 未保持上一拍有效值 (t=%0t)", $time);

    // -------------------------------------------------------------------------
    // A5: 输出完整性与饱和范围 —— 无 X 态, 数值严格落在 [-32768, 32767]
    //     (比较表达式对 X/Z 求值为 X, 在断言语义中等价于失败, 故兼作 X 检查)
    // -------------------------------------------------------------------------
    property p_out_range;
        disable iff (!rst_n)
        Valid_out |-> (Out >= -16'sd32768 && Out <= 16'sd32767);
    endproperty
    a_out_range: assert property (p_out_range)
        else $error("[SVA-A5] Valid_out 有效时 Out 越界或含 X 态: %h (t=%0t)", Out, $time);

    property p_no_x;
        rst_n |-> !$isunknown({Valid_out, valid_s1});
    endproperty
    a_no_x: assert property (p_no_x)
        else $error("[SVA-A5b] 有效位链路出现 X 态 (t=%0t)", $time);

    // -------------------------------------------------------------------------
    // 覆盖点 (Cover) —— 证明测试确实触及了饱和与保持路径, 防止"断言永不触发"假象
    // -------------------------------------------------------------------------
    c_sat_pos: cover property (
        disable iff (!rst_n) Valid_out && Out === 16'sh7FFF);
    c_sat_neg: cover property (
        disable iff (!rst_n) Valid_out && Out === 16'sh8000);
    c_out_hold: cover property (
        disable iff (!rst_n) !valid_s1 ##1 $stable(Out));
    c_back2back: cover property (
        disable iff (!rst_n) valid_in [*4]);

endmodule

// =============================================================================
// 绑定: 将断言模块挂载到 mac_unit 所有实例, valid_s1 探入内部 PR1 有效位
// =============================================================================
bind mac_unit mac_unit_sva u_mac_unit_sva (
    .clk       (clk),
    .rst_n     (rst_n),
    .valid_in  (valid_in),
    .valid_s1  (valid_s1_reg),
    .Valid_out (Valid_out),
    .Out       (Out)
);
