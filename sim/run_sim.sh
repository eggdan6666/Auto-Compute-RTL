#!/usr/bin/env bash
# Auto-Compute-RTL 仿真闭环一键脚本 (Git Bash)
#
# 工具链 (均为 E 盘便携安装, 免管理员):
#   E:\tools\oss-cad-suite  — Icarus Verilog / Verilator / Yosys
#   E:\tools\mingw64        — WinLibs GCC 16.2 (Verilator 的 C++ 后端)
#
# 注意:
#   1. Icarus (vvp) 与 WinLibs 的 MinGW 运行时 DLL 互不兼容, PATH 顺序必须
#      分阶段设置, 不可全局混用 (否则 vvp 静默退出 127 或直接段错误)。
#   2. VERILATOR_ROOT 必须用正斜杠路径, 反斜杠会被 make 吃掉。
#   3. 工程路径含空格/中文, GNU Make 拒绝构建 -> Verilator 阶段在
#      E:\work\ 下的纯净副本中构建运行 (源文件自动同步)。
#   4. WinLibs GCC 16 的 -Os 会触发 declone-ctor-dtor, 产生 libstdc++
#      不导出的 C4 构造符号 -> 必须加 -fno-declone-ctor-dtor。
set -e
cd "$(dirname "$0")/.."

OSS_BIN=/e/tools/oss-cad-suite/bin
OSS_LIB=/e/tools/oss-cad-suite/lib
MINGW_BIN=/e/tools/mingw64/bin
ORIG_PATH="$PATH"   # 保存初始 PATH, 收官阶段还原纯净 OSS 环境 (剥离 MINGW 以免污染 vvp)

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
      && ./obj_dir/"$out".exe )
}

echo "==> [1/16] 重新生成黄金向量 (Python Golden Oracle)"
python golden_model.py > /dev/null
python golden_array.py > /dev/null
python golden_bist.py  > /dev/null
python golden_axi.py   > /dev/null

export PATH="$OSS_BIN:$OSS_LIB:$PATH"

echo "==> [2/16] Icarus 编译 MAC + 脉动阵列 + 自愈阵列 + AXI IP + Phase2 + 瞬态/毒化回归"
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

# F0: 瞬态单拍注入分布 (Icarus)
iverilog -g2012 -o sim/tb_transient_sweep.vvp \
    rtl/systolic_pe.sv rtl/rne_sat16.sv rtl/systolic_array.sv \
    rtl/misr16.sv rtl/bist_controller.sv rtl/fault_arbiter.sv \
    rtl/abft_checksum.sv rtl/zero_mask.sv rtl/self_healing_array.sv \
    tb/tb_transient_sweep.sv 2>&1 | grep -v "sorry:" || true

# F1/F2: MTTR 毒化 + BIST 污染 (持续灌流) 回归
iverilog -g2012 -o sim/tb_mttr_poison.vvp \
    rtl/systolic_pe.sv rtl/rne_sat16.sv rtl/systolic_array.sv \
    rtl/misr16.sv rtl/bist_controller.sv rtl/fault_arbiter.sv \
    rtl/abft_checksum.sv rtl/zero_mask.sv rtl/self_healing_array.sv \
    tb/tb_mttr_poison2.sv 2>&1 | grep -v "sorry:" || true

echo "==> [3/16] Icarus 仿真: MAC 单元 (功能回归 + 黄金向量)"
vvp -N sim/tb_mac_unit.vvp

echo "==> [4/16] Icarus 仿真: 脉动阵列 (黄金向量)"
vvp -N sim/tb_systolic_array.vvp

echo "==> [5/16] Icarus 仿真: 自愈阵列 (BIST + 故障注入 + 热替换)"
vvp -N sim/tb_self_healing.vvp

echo "==> [6/16] Icarus 仿真: AXI4-Stream IP (随机反压 + 故障注入)"
vvp -N sim/tb_axi.vvp

echo "==> [7/16] Icarus 仿真: ABFT x 零掩蔽 对齐契约证据 TB (peer review 证据平台)"
vvp -N sim/tb_align.vvp

echo "==> [8/16] Icarus 仿真: Phase 2 外循环闭环自愈 + MTTR 量化 (瞬态吸收 + 永久硬替换)"
vvp -N sim/tb_phase2.vvp

echo "==> [9/16] Icarus 仿真: 瞬态单拍注入分布实测 (F0 修复 + 反向校验)"
vvp -N sim/tb_transient_sweep.vvp

echo "==> [10/16] Icarus 仿真: MTTR 毒化回归 SCEN=1 (detect 40 拍清哨)"
vvp -N sim/tb_mttr_poison.vvp +SCEN=1

echo "==> [11/16] Icarus 仿真: MTTR 毒化回归 SCEN=2 (repair H_IDLE 清哨)"
vvp -N sim/tb_mttr_poison.vvp +SCEN=2

echo "==> [12/16] Icarus 仿真: BIST 污染回归 SCEN=3 (持续灌流 + 排空隔离)"
vvp -N sim/tb_mttr_poison.vvp +SCEN=3

export PATH="$MINGW_BIN:$PATH"
export VERILATOR_ROOT="E:/tools/oss-cad-suite/share/verilator"

echo "==> [13/16] Verilator 动态断言仿真: MAC 单元 (SVA)"
verilate_and_run /e/work/mac_vl tb_mac_unit tb_vl \
    rtl/mac_unit.sv rtl/mac_unit_sva.sv tb/tb_mac_unit.sv test_vectors.hex

echo "==> [14/16] Verilator 动态断言仿真: 脉动阵列 (SVA)"
verilate_and_run /e/work/array_vl tb_systolic_array tb_arr \
    rtl/systolic_pe.sv rtl/rne_sat16.sv rtl/systolic_array.sv rtl/rne_sat16_sva.sv rtl/systolic_array_sva.sv \
    tb/tb_systolic_array.sv array_weights.hex array_vectors.hex

echo "==> [15/16] Verilator 动态断言仿真: 自愈阵列 (SVA + 故障注入)"
verilate_and_run /e/work/heal_vl tb_self_healing tb_heal \
    rtl/systolic_pe.sv rtl/rne_sat16.sv rtl/systolic_array.sv rtl/rne_sat16_sva.sv rtl/systolic_array_sva.sv \
    rtl/misr16.sv rtl/bist_controller.sv rtl/self_healing_array.sv \
    rtl/abft_checksum.sv rtl/zero_mask.sv rtl/fault_arbiter.sv \
    tb/tb_self_healing.sv bist_weights.hex bist_rows.hex bist_golden.hex \
    mission_weights.hex mission_vectors.hex

echo "==> [16/16] Verilator 动态断言仿真: AXI4-Stream IP (全部 SVA + 随机反压 + 故障注入)"
verilate_and_run /e/work/axi_vl tb_axi_stream_array tb_axi \
    rtl/systolic_pe.sv rtl/rne_sat16.sv rtl/systolic_array.sv \
    rtl/rne_sat16_sva.sv rtl/systolic_array_sva.sv \
    rtl/misr16.sv rtl/bist_controller.sv rtl/self_healing_array.sv \
    rtl/abft_checksum.sv rtl/zero_mask.sv rtl/fault_arbiter.sv \
    rtl/axi_stream_array.sv rtl/axi_stream_array_sva.sv \
    tb/tb_axi_stream_array.sv bist_weights.hex bist_rows.hex bist_golden.hex \
    axi_in.hex axi_out_golden.hex

# 收官阶段: 还原纯净 OSS PATH (剥离 Verilator 阶段混入的 MINGW), 再跑全部 16 项终审门控
export PATH="$OSS_BIN:$OSS_LIB:$ORIG_PATH"

echo "==> [收官] Round 2 终审门控 (G1~G9 共 16 项硬门控; 任一 FAIL 退出码非 0 即整体失败)"
bash sim/review_p23/run_round2_gates.sh

echo "==> 全部阶段完成"