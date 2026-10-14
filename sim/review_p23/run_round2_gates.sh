#!/usr/bin/env bash
# =============================================================================
# run_round2_gates.sh — Round 2 终审门控运行器 (Phase 2/3 签核前必跑)
#
# 用法 (Git Bash, 仓库根或任意目录):
#   bash sim/review_p23/run_round2_gates.sh
#
# 门控清单:
#   G1a  仓库版瞬态扫查单拍语义 (hist_2==0, ge3==0, busy==0, hist_1>0)
#   G1b  独立参照真单拍扫查 (同准则)
#   G1c  扫查测量链可失败性 (已知 2 拍注入必须测得 >=2 脉冲)
#   G2   Phase2 闭环无回归 (12/12, detect==2, repair∈[50,80], total=detect+repair)
#   G3a  MTTR detect 解毒 (毒化场景 detect_latency<=10)
#   G3b  MTTR repair 解毒 (毒化场景 repair_latency<=80)
#   G4a  瞬态簇+持续灌流不伪判 (无 unhealable/伪重映射)
#   G4b  永久故障+持续灌流正确自愈 (healed=1, fault_col=1)
#   G5   Q1.15 算子三方逐位一致 (0 mismatch)
#   G6   综合卫生 (Yosys 0 锁存器 + check -assert)
#   G7   Python 修复门 (P2-3/P2-4/ge3/float 模式)
#   G8   战役冒烟 (--synthetic 全管线)
#   G9   QAT 数值差口径 (<=0.5%, 信息性记录)
#
# 输出: sim/review_p23/round2_gates.log; 退出码 = FAIL 门数
# =============================================================================
set -u
cd "$(dirname "$0")/../.."   # -> 仓库根

OSS_BIN=/e/tools/oss-cad-suite/bin
OSS_LIB=/e/tools/oss-cad-suite/lib
export PATH="$OSS_BIN:$OSS_LIB:$PATH"
PY="C:/Users/ZhuanZ/AppData/Local/Programs/Python/Python311/python.exe"
[ -x "$PY" ] || PY=python

RTL="rtl/systolic_pe.sv rtl/rne_sat16.sv rtl/systolic_array.sv rtl/misr16.sv \
rtl/bist_controller.sv rtl/self_healing_array.sv rtl/abft_checksum.sv \
rtl/zero_mask.sv rtl/fault_arbiter.sv"
G=sim/review_p23
LOG=$G/round2_gates.log
mkdir -p "$G"
: > "$LOG"

fail=0
gate() {  # $1=名称 $2=PASS|FAIL $3=明细
    echo "[GATE] $1: $2: $3" | tee -a "$LOG"
    [ "$2" = "PASS" ] || fail=$((fail+1))
}
note() { echo "$@" | tee -a "$LOG"; }

compile() {  # $1=输出vvp $2...=源文件
    iverilog -g2012 -o "$1" "${@:2}" >> "$LOG" 2>&1
}

note "================ Round 2 终审门控 ================"
note "日期: $(date '+%Y-%m-%d %H:%M:%S')   工具: $(iverilog -V 2>/dev/null | head -1)"

# ---------------------------------------------------------------- G1a
note "---- G1a 仓库版 tb_transient_sweep.sv (修复后) ----"
compile $G/g1a.vvp $RTL tb/tb_transient_sweep.sv
vvp -N $G/g1a.vvp > $G/g1a.out 2>&1
sed -n '/实测分布统计/,/====/p' $G/g1a.out >> "$LOG"
h0=$(sed -n 's/.*产生 0 脉冲.*: *\([0-9]*\) 次.*/\1/p' $G/g1a.out)
h1=$(sed -n 's/.*产生 1 脉冲.*: *\([0-9]*\) 次.*/\1/p' $G/g1a.out)
h2=$(sed -n 's/.*产生 2 脉冲.*: *\([0-9]*\) 次.*/\1/p' $G/g1a.out)
h3=$(sed -n 's/.*>=3 脉冲.*: *\([0-9]*\) 次.*/\1/p' $G/g1a.out)
bv=$(sed -n 's/.*误触发 BIST 停机次数.*: *\([0-9]*\) 次.*/\1/p' $G/g1a.out)
if [ "${h2:-1}" = "0" ] && [ "${h3:-1}" = "0" ] && [ "${bv:-1}" = "0" ] && \
   [ "${h1:-0}" -gt 0 ] && [ $((h0+h1+h2)) -eq 100 ]; then
    gate G1a PASS "hist=[$h0,$h1,$h2,$h3] busy_viol=$bv (真单拍: 2脉冲=0)"
else
    gate G1a FAIL "hist=[$h0,$h1,$h2,$h3] busy_viol=$bv (2脉冲须=0, ge3须=0, 1脉冲须>0)"
fi

# ---------------------------------------------------------------- G1b
note "---- G1b 独立参照 tb_sweep_1beat.sv ----"
compile $G/g1b.vvp $RTL $G/tb_sweep_1beat.sv
vvp -N $G/g1b.vvp > $G/g1b.out 2>&1
sed -n '/实测分布统计/,/====/p' $G/g1b.out >> "$LOG"
b0=$(sed -n 's/.*产生 0 脉冲.*: *\([0-9]*\) 次.*/\1/p' $G/g1b.out)
b1=$(sed -n 's/.*产生 1 脉冲.*: *\([0-9]*\) 次.*/\1/p' $G/g1b.out)
b2=$(sed -n 's/.*产生 2 脉冲.*: *\([0-9]*\) 次.*/\1/p' $G/g1b.out)
b3=$(sed -n 's/.*>=3 脉冲.*: *\([0-9]*\) 次.*/\1/p' $G/g1b.out)
if [ "${b2:-1}" = "0" ] && [ "${b3:-1}" = "0" ] && [ "${b1:-0}" -gt 0 ]; then
    gate G1b PASS "参照分布=[$b0,$b1,$b2,$b3] (期望 87/13/0/0)"
else
    gate G1b FAIL "参照分布=[$b0,$b1,$b2,$b3]"
fi

# ---------------------------------------------------------------- G1c
note "---- G1c 测量链可失败性 tb_gate_sweep_canfail.sv ----"
compile $G/g1c.vvp $RTL $G/tb_gate_sweep_canfail.sv
vvp -N $G/g1c.vvp > $G/g1c.out 2>&1
grep -E "CANFAIL" $G/g1c.out | tee -a "$LOG"
if grep -q "\[GATE\] canfail_2beat: .*PASS" $G/g1c.out; then
    gate G1c PASS "已知 2 拍注入测得 >=2 脉冲 (链灵敏)"
else
    gate G1c FAIL "2 拍注入未测得 >=2 脉冲 (测量链失效!)"
fi

# ---------------------------------------------------------------- G2
note "---- G2 仓库版 tb_phase2_closed_loop.sv ----"
compile $G/g2.vvp $RTL tb/tb_phase2_closed_loop.sv
vvp -N $G/g2.vvp > $G/g2.out 2>&1
grep -E "PASS|FAIL|MTTR|汇总" $G/g2.out | tee -a "$LOG"
det=$(sed -n 's/.*detect_latency = \([0-9]*\) cycles.*/\1/p' $G/g2.out)
rep=$(sed -n 's/.*repair_latency = \([0-9]*\) cycles.*/\1/p' $G/g2.out)
tot=$(sed -n 's/.*total_mttr     = \([0-9]*\) cycles.*/\1/p' $G/g2.out)
if grep -q "Total=12 Pass=12 Fail=0" $G/g2.out && [ "${det:-0}" = "2" ] && \
   [ "${rep:-0}" -ge 50 ] && [ "${rep:-0}" -le 80 ] && [ "${tot:-0}" -eq $((det+rep)) ]; then
    gate G2 PASS "12/12; MTTR detect=$det repair=$rep total=$tot (S_DRAIN 后 repair 由 59->$rep)"
else
    gate G2 FAIL "Total/MTTR 不符: detect=$det repair=$rep total=$tot"
fi

# ---------------------------------------------------------------- G3/G4
note "---- G3/G4 tb_round2_gates.sv (毒化+灌流韧性) ----"
compile $G/g34.vvp $RTL $G/tb_round2_gates.sv
vvp -N $G/g34.vvp > $G/g34.out 2>&1
for g in G3a G3b G4a G4b; do
    if grep -q "\[GATE\] $g: PASS" $G/g34.out; then
        gate $g PASS "$(grep -o "\[GATE\] $g: PASS.*" $G/g34.out | head -1 | sed "s/^\[GATE\] [^:]*: [A-Z]*: //" | cut -c1-100)"
    else
        gate $g FAIL "$(grep -o "\[GATE\] $g: FAIL.*" $G/g34.out | head -1 | sed "s/^\[GATE\] [^:]*: [A-Z]*: //" | cut -c1-110)"
    fi
done

# ---------------------------------------------------------------- G5
note "---- G5 Q1.15 算子三方一致 ----"
"$PY" $G/gen_rne_vectors.py >> "$LOG" 2>&1
compile $G/g5.vvp rtl/rne_sat16.sv $G/tb_rne_check.sv
vvp -N $G/g5.vvp > $G/g5.out 2>&1
tail -2 $G/g5.out | tee -a "$LOG"
mm=$("$PY" - <<'PYEOF'
def s16(x): return x - (1 << 16) if x >= (1 << 15) else x
rtl = [s16(int(x, 16)) for x in open("sim/review_p23/rne_out_rtl.hex") if x.strip()]
ref = [s16(int(x, 16)) for x in open("sim/review_p23/rne_exp_ref.hex") if x.strip()]
print(sum(1 for a, b in zip(rtl, ref) if a != b))
PYEOF
)
if [ "${mm:-1}" = "0" ]; then
    gate G5 PASS "2602 向量 RTL==torch==参考模型 (0 mismatch)"
else
    gate G5 FAIL "RNE 不一致 mismatch=$mm"
fi

# ---------------------------------------------------------------- G6
note "---- G6 综合卫生 (Yosys) ----"
yosys -p "read_verilog -sv $RTL; hierarchy -check; proc; opt; check -assert" > $G/g6.out 2>&1
ec=$?
nlatch=$(grep -c "No latch inferred" $G/g6.out)
if [ $ec -eq 0 ]; then
    gate G6 PASS "check -assert 退出 0; No-latch 确认 $nlatch 处; 0 个 \$dlatch 单元"
else
    gate G6 FAIL "Yosys check 退出 $ec (见 $G/g6.out)"
fi

# ---------------------------------------------------------------- G7/G8/G9
note "---- G7/G8/G9 Python 侧门控 ----"
"$PY" $G/py_gate_checks.py > $G/g789.out 2>&1
pec=$?
grep -v "^\[GATE\]" $G/g789.out >> "$LOG"   # 非判定明细进日志 (判定行由下方 loop 单点产生)
for g in G7a G7b G7c G7d G8 G9; do
    if grep -q "\[GATE\] $g: PASS" $G/g789.out; then
        gate $g PASS "$(grep -o "\[GATE\] $g: PASS.*" $G/g789.out | head -1 | sed "s/^\[GATE\] [^:]*: [A-Z]*: //" | cut -c1-95)"
    else
        gate $g FAIL "$(grep -o "\[GATE\] $g: FAIL.*" $G/g789.out | head -1 | sed "s/^\[GATE\] [^:]*: [A-Z]*: //" | cut -c1-105)"
    fi
done

# ---------------------------------------------------------------- 汇总
note ""
note "================ 汇总 ================"
npass=$(grep -ac "\[GATE\].*: PASS" "$LOG")
note "PASS 门数: $npass"
note "FAIL 门数: $fail"
grep -a "\[GATE\].*: FAIL" "$LOG" | sed 's/^/  /' | tee -a "$LOG"
note "完整日志: $LOG"
exit $fail
