#!/usr/bin/env bash
# Auto-Compute-RTL 仿真闭环一键脚本 — Linux 版 (SCNet Notebook / 任意 Linux x64)
# 工具链: /root/tools/oss-cad-suite (Linux x64 便携版) + 系统 g++/make
set -e
cd "$(dirname "$0")/.."

OSS=/root/tools/oss-cad-suite
export PATH="$OSS/bin:$PATH"
export VERILATOR_ROOT="$OSS/share/verilator"

verilate_and_run () {  # $1=工作目录 $2=顶层模块 $3=输出名 $4...=源文件(.sv)+数据文件(.hex)
    local vdir=$1 top=$2 out=$3; shift 3
    mkdir -p "$vdir"
    cp "$@" "$vdir"/
    local srcs=()
    for f in "$@"; do
        case "$f" in *.sv) srcs+=("$(basename "$f")");; esac
    done
    ( cd "$vdir" \
      && verilator_bin --binary --timing --assert -Wno-fatal \
           -CFLAGS "-fno-declone-ctor-dtor" -o "$out" \
           "${srcs[@]}" --top-module "$top" > /dev/null \
      && ./obj_dir/"$out" )
}

echo "==> [1/12] 重新生成黄金向量 (Python Golden Oracle)"
python3 golden_model.py > /dev/null
python3 golden_array.py > /dev/null
python3 golden_bist.py  > /dev/null
python3 golden_axi.py   > /dev/null

echo "==> [2/12] Icarus 编译 MAC + 脉动阵列 + 自愈阵列 + AXI IP + Phase2"
mkdir -p sim
iverilog -g2012 -o sim/tb_mac_unit.vvp rtl/mac_unit.sv tb/tb_mac_unit.sv 2>&1 | grep -v "sorry:" || true
iverilog -g2012 -o sim/tb_systolic_array.vvp \
    rtl/systolic_pe.sv rtl/rne_sat16.sv rtl/systolic_array.sv tb/tb_systolic_array.sv 2>&1 | grep -v "sorry:" || true
iverilog -g2012 -o sim/tb_self_healing.vvp \
    rtl/systolic_pe.sv rtl/rne_sat16.sv rtl/systolic_array.sv \
    rtl/misr16.sv rtl/bist_controller.sv rtl/self_healing_array.sv \
    rtl/abft_checksum.sv rtl/zero_mask.sv rtl/fault_arbiter.sv \
    tb/tb_self_healing.sv 2>&1 | grep -v "sorry:" || true
iverilog -g2012 -o sim/tb_axi.vvp \
    rtl/systolic_pe.sv rtl/rne_sat16.sv rtl/systolic_array.sv \
    rtl/misr16.sv rtl/bist_controller.sv rtl/self_healing_array.sv \
    rtl/abft_checksum.sv rtl/zero_mask.sv rtl/fault_arbiter.sv \
    rtl/axi_stream_array.sv tb/tb_axi_stream_array.sv 2>&1 | grep -v "sorry:" || true

iverilog -g2012 -o sim/tb_align.vvp \
    rtl/abft_checksum.sv rtl/zero_mask.sv \
    tb/tb_abft_zero_mask_align.sv 2>&1 | grep -v "sorry:" || true

iverilog -g2012 -o sim/tb_phase2.vvp \
    rtl/systolic_pe.sv rtl/rne_sat16.sv rtl/systolic_array.sv \
    rtl/misr16.sv rtl/bist_controller.sv rtl/fault_arbiter.sv \
    rtl/abft_checksum.sv rtl/zero_mask.sv rtl/self_healing_array.sv \
    tb/tb_phase2_closed_loop.sv 2>&1 | grep -v "sorry:" || true

echo "==> [3/12] Icarus 仿真: MAC 单元 (功能回归 + 黄金向量)"
vvp -N sim/tb_mac_unit.vvp

echo "==> [4/12] Icarus 仿真: 脉动阵列 (黄金向量)"
vvp -N sim/tb_systolic_array.vvp

echo "==> [5/12] Icarus 仿真: 自愈阵列 (BIST + 故障注入 + 热替换)"
vvp -N sim/tb_self_healing.vvp

echo "==> [6/12] Icarus 仿真: AXI4-Stream IP (随机反压 + 故障注入)"
vvp -N sim/tb_axi.vvp

echo "==> [7/12] Icarus 仿真: ABFT x 零掩蔽 对齐契约证据 TB (peer review 证据平台)"
vvp -N sim/tb_align.vvp

echo "==> [8/12] Icarus 仿真: Phase 2 外循环闭环自愈 + MTTR 量化 (瞬态吸收 + 永久硬替换)"
vvp -N sim/tb_phase2.vvp

echo "==> [9/12] Verilator 动态断言仿真: MAC 单元 (SVA)"
verilate_and_run /root/work/mac_vl tb_mac_unit tb_vl \
    rtl/mac_unit.sv rtl/mac_unit_sva.sv tb/tb_mac_unit.sv test_vectors.hex

echo "==> [10/12] Verilator 动态断言仿真: 脉动阵列 (SVA)"
verilate_and_run /root/work/array_vl tb_systolic_array tb_arr \
    rtl/systolic_pe.sv rtl/rne_sat16.sv rtl/systolic_array.sv rtl/rne_sat16_sva.sv rtl/systolic_array_sva.sv \
    tb/tb_systolic_array.sv array_weights.hex array_vectors.hex

echo "==> [11/12] Verilator 动态断言仿真: 自愈阵列 (SVA + 故障注入)"
verilate_and_run /root/work/heal_vl tb_self_healing tb_heal \
    rtl/systolic_pe.sv rtl/rne_sat16.sv rtl/systolic_array.sv rtl/rne_sat16_sva.sv rtl/systolic_array_sva.sv \
    rtl/misr16.sv rtl/bist_controller.sv rtl/self_healing_array.sv \
    rtl/abft_checksum.sv rtl/zero_mask.sv rtl/fault_arbiter.sv \
    tb/tb_self_healing.sv bist_weights.hex bist_rows.hex bist_golden.hex \
    mission_weights.hex mission_vectors.hex

echo "==> [12/12] Verilator 动态断言仿真: AXI4-Stream IP (全部 SVA + 随机反压 + 故障注入)"
verilate_and_run /root/work/axi_vl tb_axi_stream_array tb_axi \
    rtl/systolic_pe.sv rtl/rne_sat16.sv rtl/systolic_array.sv \
    rtl/rne_sat16_sva.sv rtl/systolic_array_sva.sv \
    rtl/misr16.sv rtl/bist_controller.sv rtl/self_healing_array.sv \
    rtl/abft_checksum.sv rtl/zero_mask.sv rtl/fault_arbiter.sv \
    rtl/axi_stream_array.sv rtl/axi_stream_array_sva.sv \
    tb/tb_axi_stream_array.sv bist_weights.hex bist_rows.hex bist_golden.hex \
    axi_in.hex axi_out_golden.hex

echo "==> 全部阶段完成"