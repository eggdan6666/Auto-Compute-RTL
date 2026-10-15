// =============================================================================
// hil_main.cpp — Auto-Compute-RTL 自愈阵列的 Verilator「哑执行器」(方案1: 逐周期闭环)
//
// 角色: 只做机械动作 —— 手工推时钟、按行协议读写信号；真正的"AI 决策"在
//       Python 宿主 (hil_client.py) 里，这样才能形成实时闭环。
//
// 注入原理: 用 Verilator 的 --public-flat-rw 直接读写内部寄存器 Q 值。
//   由于 systolic_pe 的 psum_out / w_reg 都是 always_ff 寄存器，在"negedge 后
//   写入其 Q 值"会被下游 PE 在紧接着的 posedge 采到，而该寄存器自身在同一个
//   posedge 会按 D 端重算 —— 与 SV 里 force@negedge / release@posedge 的
//   真单拍语义完全等价。永久卡死则每拍持续覆写（IP），RL/RAL 停止覆写即释放。
//
// 协议(行式, stdin 收命令 / stdout 回一行结果, 每行 flush):
//   R                             复位 -> K
//   W <64hex>                     载入权重(256bit) -> K
//   D <0|1> <16hex>               驱动 valid_in / a_flat(64bit) -> K
//   I <m> <r> <c> <hex>           单拍注入(下一次 negedge 生效一次) -> K
//   IP <m> <r> <c> <hex>          永久卡死(每拍覆写) -> K
//   RL <m> <r> <c>                停止该点永久覆写 -> K
//   RAL                           停止全部覆写 -> K
//   C <n>                         推进 n 拍 -> T <n位0/1串> <ntrig> <tcnt> <busy>
//   MON <r> <c>                   设定被监视 PE(对拍用) -> K
//   C1                            推 1 拍 -> T1 <err> <trig> <tcnt> <busy> <valid_out HEX> <c_flat HEX>
//                                            <ntcnt> <nbusy> <npsum HEX> <nwreg HEX> <perr> <ppsum HEX> <pwreg HEX>
//   O                             观测一次 -> O <valid_out> <c_flat> <fault_col> <busy> <healed> <unhealable> <detected> <tcnt> <dlat> <rlat> <mttr>
//   Q                             退出
//   m: 1=psum_out(40bit)  2=w_reg(16bit)
// =============================================================================
#include "Vself_healing_array.h"
#include "Vself_healing_array___024root.h"
#include "verilated.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

// Verilator 运行库在非 SystemC 模式下仍会引用 sc_time_stamp()（该工具链的 ld
// 不支持 verilated.mk 里的 -Wl,-U 抑制写法），这里由我们自己提供一个空实现。
// 注意：链接名是 C++ 修饰名 __Z13sc_time_stampv，故此处不加 extern "C"。
double sc_time_stamp() { return 0.0; }

static Vself_healing_array* top = nullptr;
static VerilatedContext* ctx = nullptr;

static QData* PS[4][5];          // psum_out 直址
static SData* WR[4][5];          // w_reg 直址 (16bit)
static CData* S_ABFT;            // abft_err (逐拍错误脉冲)
static CData* S_TRIG;            // arb_trigger_heal

#define PE_ADDR(r, c, f) (&top->rootp->self_healing_array__DOT__u_phy__DOT__g_row__BRA__##r##__KET____DOT__g_pe__BRA__##c##__KET____DOT__u_pe__DOT__##f)

static void bind_tabs() {
    PS[0][0]=PE_ADDR(0,0,psum_out); PS[0][1]=PE_ADDR(0,1,psum_out); PS[0][2]=PE_ADDR(0,2,psum_out); PS[0][3]=PE_ADDR(0,3,psum_out); PS[0][4]=PE_ADDR(0,4,psum_out);
    PS[1][0]=PE_ADDR(1,0,psum_out); PS[1][1]=PE_ADDR(1,1,psum_out); PS[1][2]=PE_ADDR(1,2,psum_out); PS[1][3]=PE_ADDR(1,3,psum_out); PS[1][4]=PE_ADDR(1,4,psum_out);
    PS[2][0]=PE_ADDR(2,0,psum_out); PS[2][1]=PE_ADDR(2,1,psum_out); PS[2][2]=PE_ADDR(2,2,psum_out); PS[2][3]=PE_ADDR(2,3,psum_out); PS[2][4]=PE_ADDR(2,4,psum_out);
    PS[3][0]=PE_ADDR(3,0,psum_out); PS[3][1]=PE_ADDR(3,1,psum_out); PS[3][2]=PE_ADDR(3,2,psum_out); PS[3][3]=PE_ADDR(3,3,psum_out); PS[3][4]=PE_ADDR(3,4,psum_out);
    WR[0][0]=PE_ADDR(0,0,w_reg); WR[0][1]=PE_ADDR(0,1,w_reg); WR[0][2]=PE_ADDR(0,2,w_reg); WR[0][3]=PE_ADDR(0,3,w_reg); WR[0][4]=PE_ADDR(0,4,w_reg);
    WR[1][0]=PE_ADDR(1,0,w_reg); WR[1][1]=PE_ADDR(1,1,w_reg); WR[1][2]=PE_ADDR(1,2,w_reg); WR[1][3]=PE_ADDR(1,3,w_reg); WR[1][4]=PE_ADDR(1,4,w_reg);
    WR[2][0]=PE_ADDR(2,0,w_reg); WR[2][1]=PE_ADDR(2,1,w_reg); WR[2][2]=PE_ADDR(2,2,w_reg); WR[2][3]=PE_ADDR(2,3,w_reg); WR[2][4]=PE_ADDR(2,4,w_reg);
    WR[3][0]=PE_ADDR(3,0,w_reg); WR[3][1]=PE_ADDR(3,1,w_reg); WR[3][2]=PE_ADDR(3,2,w_reg); WR[3][3]=PE_ADDR(3,3,w_reg); WR[3][4]=PE_ADDR(3,4,w_reg);
    S_ABFT = &top->rootp->self_healing_array__DOT__abft_err;
    S_TRIG = &top->rootp->self_healing_array__DOT__arb_trigger_heal;
}

// ---- 注入状态 ----
struct Inj { int model, row, col; QData val; bool persist; };
static Inj single_inj;  static bool single_armed = false;
static Inj persist_inj[8]; static int n_persist = 0;

// ---- 对拍用监视点 (MON <r> <c>): C1 顺带回吐该 PE 的 psum_out / w_reg ----
static int mon_r = 0, mon_c = 0;

static void set_wide(VlWide<8>& w, const char* hex) {   // 256bit little-endian 字序
    int n = (int)strlen(hex);
    for (int i = 0; i < 8; i++) {
        unsigned v = 0;
        for (int k = 0; k < 8; k++) {
            int pos = n - 1 - (i * 8 + k);
            if (pos < 0) break;
            char ch = hex[pos];
            unsigned d = (ch >= '0' && ch <= '9') ? (unsigned)(ch - '0')
                                                  : (unsigned)((ch | 32) - 'a' + 10);
            v |= d << (k * 4);
        }
        w[i] = v;
    }
}

static void write_inj(const Inj& j) {
    if (j.row < 0 || j.row > 3 || j.col < 0 || j.col > 4) return;
    if (j.model == 1) *PS[j.row][j.col] = (QData)(j.val & 0xFFFFFFFFFFULL);
    else if (j.model == 2) *WR[j.row][j.col] = (SData)(j.val & 0xFFFFULL);
}

// 一拍: negedge -> 采样 abft_err -> 应用注入/驱动 -> posedge
// 采样口径与 Icarus 侧 tb_inj_trace.sv 严格对齐:
//   n* = negedge 采样, 且发生在 write_inj 之前 (等价于 SV TB 的 obs 分支)
//   p* = posedge eval 之后的寄存器终值 (等价于 SV TB release 后的读回)
static int g_err, g_trig, g_tcnt, g_busy;
static int g_ntcnt, g_nbusy, g_perr, g_ptrg;
static QData g_npsum, g_ppsum;
static SData g_nwreg, g_pwreg;

static void tick() {
    top->clk = 0; top->eval();
    g_err  = (*S_ABFT) ? 1 : 0;
    g_trig = (*S_TRIG) ? 1 : 0;
    g_ntcnt = (int)top->transient_cnt;
    g_nbusy = (int)top->busy;
    g_npsum = *PS[mon_r][mon_c];
    g_nwreg = *WR[mon_r][mon_c];

    if (single_armed) { write_inj(single_inj); single_armed = false; }
    for (int i = 0; i < n_persist; i++) write_inj(persist_inj[i]);

    top->clk = 1; top->eval();
    g_tcnt = (int)top->transient_cnt;
    g_busy = (int)top->busy;
    g_perr = (*S_ABFT) ? 1 : 0;
    g_ptrg = (*S_TRIG) ? 1 : 0;
    g_ppsum = *PS[mon_r][mon_c];
    g_pwreg = *WR[mon_r][mon_c];
}

static void do_reset() {
    top->rst_n = 0; top->load_w = 0; top->valid_in = 0; top->heal_start = 0;
    top->a_flat = 0; top->w_flat[0] = top->w_flat[1] = top->w_flat[2] = top->w_flat[3] = 0;
    top->w_flat[4] = top->w_flat[5] = top->w_flat[6] = top->w_flat[7] = 0;
    single_armed = false; n_persist = 0;
    for (int i = 0; i < 4; i++) { top->clk = 0; top->eval(); top->clk = 1; top->eval(); }
    top->rst_n = 1; top->eval();
}

int main(int argc, char** argv) {
    ctx = new VerilatedContext;
    ctx->commandArgs(argc, argv);
    top = new Vself_healing_array(ctx);
    bind_tabs();

    top->clk = 0; top->rst_n = 1; top->load_w = 0; top->valid_in = 0;
    top->heal_start = 0; top->a_flat = 0;
    for (int i = 0; i < 8; i++) top->w_flat[i] = 0;
    top->eval();

    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;

        if (!strcmp(cmd, "Q")) { printf("K\n"); fflush(stdout); break; }
        else if (!strcmp(cmd, "R")) { do_reset(); printf("K\n"); }
        else if (!strcmp(cmd, "W")) {
            char hx[256] = {0}; sscanf(line, "%*s %255s", hx);
            set_wide(top->w_flat, hx);
            top->load_w = 1; tick(); top->load_w = 0; top->w_flat[0]=0;  // 一个载入拍
            printf("K\n");
        }
        else if (!strcmp(cmd, "D")) {
            int v; unsigned long long a = 0;
            sscanf(line, "%*s %d %llx", &v, &a);
            top->valid_in = v ? 1 : 0; top->a_flat = a;
            printf("K\n");
        }
        else if (!strcmp(cmd, "HEAL")) { int v=0; sscanf(line,"%*s %d",&v); top->heal_start = v?1:0; tick(); top->heal_start=0; printf("K\n"); }
        else if (!strcmp(cmd, "I")) {
            int m, r, c; unsigned long long v = 0;
            sscanf(line, "%*s %d %d %d %llx", &m, &r, &c, &v);
            single_inj = { m, r, c, (QData)v, false }; single_armed = true;
            printf("K\n");
        }
        else if (!strcmp(cmd, "IP")) {
            int m, r, c; unsigned long long v = 0;
            sscanf(line, "%*s %d %d %d %llx", &m, &r, &c, &v);
            if (n_persist < 8) persist_inj[n_persist++] = { m, r, c, (QData)v, true };
            printf("K\n");
        }
        else if (!strcmp(cmd, "RL")) { n_persist = 0; printf("K\n"); }
        else if (!strcmp(cmd, "RAL")) { n_persist = 0; single_armed = false; printf("K\n"); }
        else if (!strcmp(cmd, "MON")) {     // 对拍: 设定被监视 PE
            int r = 0, c = 0; sscanf(line, "%*s %d %d", &r, &c);
            if (r >= 0 && r <= 3 && c >= 0 && c <= 4) { mon_r = r; mon_c = c; }
            printf("K\n");
        }
        else if (!strcmp(cmd, "C1")) {      // 单拍全观测（AI 逐周期决策用 + 对拍）
            tick();
            printf("T1 %d %d %d %d %X %llX %d %d %llX %X %d %llX %X %d\n",
                   g_err, g_trig, g_tcnt, g_busy,
                   (unsigned)top->valid_out, (unsigned long long)top->c_flat,
                   g_ntcnt, g_nbusy,
                   (unsigned long long)g_npsum, (unsigned)g_nwreg,
                   g_perr, (unsigned long long)g_ppsum, (unsigned)g_pwreg, g_ptrg);
        }
        else if (!strcmp(cmd, "C")) {
            int n = 1; sscanf(line, "%*s %d", &n);
            std::string bits; int ntrig = 0;
            for (int i = 0; i < n; i++) {
                tick();
                bits.push_back(g_err ? '1' : '0');
                if (g_trig) ntrig++;
            }
            printf("T %s %d %d %d\n", bits.c_str(), ntrig, g_tcnt, g_busy);
        }
        else if (!strcmp(cmd, "O")) {
            top->eval();
            printf("O %u %llu %u %u %u %u %u %u %u %u %u\n",
                   (unsigned)top->valid_out, (unsigned long long)top->c_flat,
                   (unsigned)top->fault_col, (unsigned)top->busy,
                   (unsigned)top->healed, (unsigned)top->unhealable,
                   (unsigned)top->fault_detected, (unsigned)top->transient_cnt,
                   (unsigned)top->detect_latency, (unsigned)top->repair_latency,
                   (unsigned)top->total_mttr);
        }
        else printf("E unknown\n");
        fflush(stdout);
    }
    top->final();
    delete top;
    delete ctx;
    return 0;
}
