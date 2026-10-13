#!/usr/bin/env bash
# E3 v5: Sky130 签核 — 快稳流 (abc -liberty 无 dfflibmap) + 层次化 stat + DFF 计数
# 方法学说明:
#   - 组合逻辑面积: abc -liberty 映射 (实测)
#   - 寄存器面积: DFF 数量 × sky130_fd_sc_hd__dfrtp_1 单元面积 25.02 um^2 (解析估算)
#   - 层次化 stat 提供各模块局部单元数 (映射无关, 可复现)
set -u
cd "$(dirname "$0")/.."
export PATH="/root/tools/oss-cad-suite/bin:$PATH"
LIB=/root/pdk/sky130_fd_sc_hd__tt_025C_1v80.lib
mkdir -p sim/sky130_logs

run_synth () {  # $1=顶层 $2=日志名 $3=chparam(可空) $4...=源文件
    local top=$1 tag=$2 cp=$3; shift 3
    # 层次化 stat (不 flatten): 各模块局部分解
    yosys -p "read_verilog -sv $*; $cp; hierarchy -top $top; proc; opt; techmap; opt; stat" \
          > "sim/sky130_logs/${tag}_hier.log" 2>&1 &
    # 映射总面积 + DFF 计数
    yosys -p "read_verilog -sv $*; $cp; hierarchy -top $top; proc; opt; flatten; techmap; opt; \
              abc -liberty $LIB; stat -liberty $LIB" \
          > "sim/sky130_logs/$tag.log" 2>&1 &
    wait
    echo "== $tag done"
}

run_synth mac_unit            mac_unit      "" rtl/mac_unit.sv
run_synth systolic_array      array4        "chparam -set NR 4 -set NC 4 systolic_array" rtl/systolic_pe.sv rtl/rne_sat16.sv rtl/systolic_array.sv
run_synth systolic_array      array8        "chparam -set NR 8 -set NC 8 systolic_array" rtl/systolic_pe.sv rtl/rne_sat16.sv rtl/systolic_array.sv
run_synth self_healing_array  self_healing  "" rtl/systolic_pe.sv rtl/rne_sat16.sv rtl/systolic_array.sv rtl/misr16.sv rtl/bist_controller.sv rtl/self_healing_array.sv
run_synth axi_stream_array    axi_ip        "" rtl/systolic_pe.sv rtl/rne_sat16.sv rtl/systolic_array.sv rtl/misr16.sv rtl/bist_controller.sv rtl/self_healing_array.sv rtl/axi_stream_array.sv

echo "==> 完成: sim/sky130_logs/"
