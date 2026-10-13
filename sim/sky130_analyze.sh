#!/usr/bin/env bash
# 汇总 Sky130 签核数据: 映射面积 + DFF 计数 + 门级逻辑深度(ltp, pre-abc)
set -u
cd /root/autocompute
export PATH=/root/tools/oss-cad-suite/bin:$PATH

declare -A SRCS CP
SRCS[mac_unit]="rtl/mac_unit.sv"
CP[mac_unit]=""
SRCS[array4]="rtl/systolic_pe.sv rtl/rne_sat16.sv rtl/systolic_array.sv"
CP[array4]="chparam -set NR 4 -set NC 4 systolic_array"
SRCS[array8]="rtl/systolic_pe.sv rtl/rne_sat16.sv rtl/systolic_array.sv"
CP[array8]="chparam -set NR 8 -set NC 8 systolic_array"
SRCS[self_healing]="rtl/systolic_pe.sv rtl/rne_sat16.sv rtl/systolic_array.sv rtl/misr16.sv rtl/bist_controller.sv rtl/self_healing_array.sv"
CP[self_healing]=""
SRCS[axi_ip]="${SRCS[self_healing]} rtl/axi_stream_array.sv"
CP[axi_ip]=""

declare -A TOPS
TOPS[mac_unit]=mac_unit; TOPS[array4]=systolic_array; TOPS[array8]=systolic_array
TOPS[self_healing]=self_healing_array; TOPS[axi_ip]=axi_stream_array

for t in mac_unit array4 array8 self_healing axi_ip; do
    area=$(grep -h 'Chip area' sim/sky130_logs/$t.log | grep -oE '[0-9.]+')
    dffc=$(grep -E '^ +[0-9]+ +\$_DFF' sim/sky130_logs/$t.log | awk '{s+=$1} END {print s+0}')
    # ltp on pre-abc netlist (generic gates)
    depth=$(yosys -p "read_verilog -sv ${SRCS[$t]}; ${CP[$t]}; hierarchy -top ${TOPS[$t]}; proc; opt; techmap; opt; ltp -noff" 2>/dev/null | grep -oE 'length=[0-9]+' | head -1 | cut -d= -f2)
    printf "%-14s area_comb=%-14s dff_count=%-6s depth_gates=%s\n" "$t" "$area" "$dffc" "$depth"
done
