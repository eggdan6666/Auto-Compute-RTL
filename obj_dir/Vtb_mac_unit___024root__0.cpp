// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtb_mac_unit.h for the primary calling header

#include "Vtb_mac_unit__pch.h"

VlCoroutine Vtb_mac_unit___024root___eval_initial__TOP__Vtiming__0(Vtb_mac_unit___024root* vlSelf);
VlCoroutine Vtb_mac_unit___024root___eval_initial__TOP__Vtiming__1(Vtb_mac_unit___024root* vlSelf);

void Vtb_mac_unit___024root___eval_initial(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_initial\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vtb_mac_unit___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vtb_mac_unit___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

void Vtb_mac_unit___024root___eval_sample(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_sample\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__rst_n 
        = vlSelfRef.tb_mac_unit__DOT__rst_n;
    vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__Valid_out 
        = vlSelfRef.tb_mac_unit__DOT__Valid_out;
    vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__Out 
        = vlSelfRef.tb_mac_unit__DOT__Out;
    vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__valid_in 
        = vlSelfRef.tb_mac_unit__DOT__valid_in;
    vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__uut__DOT__valid_s1_reg 
        = vlSelfRef.tb_mac_unit__DOT__uut__DOT__valid_s1_reg;
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_mac_unit___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vtb_mac_unit___024root___eval_ico(Vtb_mac_unit___024root* vlSelf, CData/*0:0*/ firstIteration) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_ico\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VicoTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VicoTriggered[0U]) 
                                     | (IData)((IData)(firstIteration)));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtb_mac_unit___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
    }
#endif
    return (0U);
}

void Vtb_mac_unit___024root___timing_ready(Vtb_mac_unit___024root* vlSelf);
void Vtb_mac_unit___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in);
#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_mac_unit___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
bool Vtb_mac_unit___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);
void Vtb_mac_unit___024root___timing_resume(Vtb_mac_unit___024root* vlSelf);

bool Vtb_mac_unit___024root___eval_act(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_act\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    {
        // Inlined CFunc: _eval_triggers_vec__act
        vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                        (((((~ (IData)(vlSelfRef.tb_mac_unit__DOT__clk)) 
                                                            & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tb_mac_unit__DOT__clk__0)) 
                                                           << 3U) 
                                                          | (vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                             << 2U)) 
                                                         | ((((~ (IData)(vlSelfRef.tb_mac_unit__DOT__rst_n)) 
                                                              & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tb_mac_unit__DOT__rst_n__0)) 
                                                             << 1U) 
                                                            | ((IData)(vlSelfRef.tb_mac_unit__DOT__clk) 
                                                               & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tb_mac_unit__DOT__clk__0)))))));
        vlSelfRef.__Vtrigprevexpr___TOP__tb_mac_unit__DOT__clk__0 
            = vlSelfRef.tb_mac_unit__DOT__clk;
        vlSelfRef.__Vtrigprevexpr___TOP__tb_mac_unit__DOT__rst_n__0 
            = vlSelfRef.tb_mac_unit__DOT__rst_n;
    }
    Vtb_mac_unit___024root___timing_ready(vlSelf);
    Vtb_mac_unit___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VactTriggered, vlSelfRef.__VactTriggeredAcc);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtb_mac_unit___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vtb_mac_unit___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vtb_mac_unit___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        vlSelfRef.__VactTriggeredAcc.fill(0ULL);
        Vtb_mac_unit___024root___timing_resume(vlSelf);
        {
            // Inlined CFunc: _eval_body__act
            if ((8ULL & vlSelfRef.__VactTriggered[0U])) {
                {
                    // Inlined CFunc: _act_sequent__TOP__0
                    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2 
                        = VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(vlSelfRef.tb_mac_unit__DOT__A)), 
                                      VL_EXTENDS_II(32,16, (IData)(vlSelfRef.tb_mac_unit__DOT__B)));
                }
            }
        }
    }
    return (__VactExecute);
}

bool Vtb_mac_unit___024root___eval_inact(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_inact\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VinactExecute;
    // Body
    __VinactExecute = vlSelfRef.__VdlySched.awaitingZeroDelay();
    if (__VinactExecute) {
        VL_FATAL_MT("tb\\tb_mac_unit.sv", 3, "", "ZERODLY: Design Verilated with '--no-sched-zero-delay', but #0 delay executed at runtime");
    }
    return (__VinactExecute);
}

void Vtb_mac_unit___024root___nba_sequent__TOP__0(Vtb_mac_unit___024root* vlSelf);
void Vtb_mac_unit___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out);

bool Vtb_mac_unit___024root___eval_nba(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_nba\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vtb_mac_unit___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        {
            // Inlined CFunc: _eval_body__nba
            if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
                Vtb_mac_unit___024root___nba_sequent__TOP__0(vlSelf);
            }
            if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
                {
                    // Inlined CFunc: _nba_sequent__TOP__1
                    QData/*39:0*/ __Vinline_0__eval_body__nba___Vinline_0__nba_sequent__TOP__1___VdfgRegularize_h6e95ff9d_0_0;
                    __Vinline_0__eval_body__nba___Vinline_0__nba_sequent__TOP__1___VdfgRegularize_h6e95ff9d_0_0 = 0;
                    QData/*40:0*/ __Vinline_0__eval_body__nba___Vinline_0__nba_sequent__TOP__1___VdfgRegularize_h6e95ff9d_0_3;
                    __Vinline_0__eval_body__nba___Vinline_0__nba_sequent__TOP__1___VdfgRegularize_h6e95ff9d_0_3 = 0;
                    if (vlSelfRef.tb_mac_unit__DOT__rst_n) {
                        vlSelfRef.tb_mac_unit__DOT__uut__DOT__acc_s1_reg 
                            = vlSelfRef.tb_mac_unit__DOT__Acc_in;
                        vlSelfRef.tb_mac_unit__DOT__uut__DOT__prod_ext_s1_reg 
                            = (((QData)((IData)((0x000000ffU 
                                                 & (- (IData)(
                                                              (vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2 
                                                               >> 0x0000001fU)))))) 
                                << 0x00000020U) | (QData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2)));
                        if (vlSelfRef.tb_mac_unit__DOT__uut__DOT__valid_s1_reg) {
                            vlSelfRef.tb_mac_unit__DOT__Out 
                                = (((~ (IData)((vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 
                                                >> 0x00000028U))) 
                                    & (0U != (0x01ffffffU 
                                              & (IData)(
                                                        (vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 
                                                         >> 0x0000000fU)))))
                                    ? 0x00007fffU : 
                                   ((IData)(((vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 
                                              >> 0x00000028U) 
                                             & (~ (0x01ffffffU 
                                                   == 
                                                   (0x01ffffffU 
                                                    & (IData)(
                                                              (vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 
                                                               >> 0x0000000fU)))))))
                                     ? 0x00008000U : 
                                    (0x0000ffffU & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))));
                        }
                    } else {
                        vlSelfRef.tb_mac_unit__DOT__uut__DOT__acc_s1_reg = 0ULL;
                        vlSelfRef.tb_mac_unit__DOT__uut__DOT__prod_ext_s1_reg = 0ULL;
                        vlSelfRef.tb_mac_unit__DOT__Out = 0U;
                    }
                    vlSelfRef.tb_mac_unit__DOT__Valid_out 
                        = ((IData)(vlSelfRef.tb_mac_unit__DOT__rst_n) 
                           && (IData)(vlSelfRef.tb_mac_unit__DOT__uut__DOT__valid_s1_reg));
                    __Vinline_0__eval_body__nba___Vinline_0__nba_sequent__TOP__1___VdfgRegularize_h6e95ff9d_0_3 
                        = (0x000001ffffffffffULL & 
                           ((((QData)((IData)((1U & (IData)(
                                                            (vlSelfRef.tb_mac_unit__DOT__uut__DOT__acc_s1_reg 
                                                             >> 0x00000027U))))) 
                              << 0x00000028U) | vlSelfRef.tb_mac_unit__DOT__uut__DOT__acc_s1_reg) 
                            + (((QData)((IData)((1U 
                                                 & (IData)(
                                                           (vlSelfRef.tb_mac_unit__DOT__uut__DOT__prod_ext_s1_reg 
                                                            >> 0x00000027U))))) 
                                << 0x00000028U) | vlSelfRef.tb_mac_unit__DOT__uut__DOT__prod_ext_s1_reg)));
                    vlSelfRef.tb_mac_unit__DOT__uut__DOT__valid_s1_reg 
                        = ((IData)(vlSelfRef.tb_mac_unit__DOT__rst_n) 
                           && (IData)(vlSelfRef.tb_mac_unit__DOT__valid_in));
                    __Vinline_0__eval_body__nba___Vinline_0__nba_sequent__TOP__1___VdfgRegularize_h6e95ff9d_0_0 
                        = ((1U == (3U & (IData)((__Vinline_0__eval_body__nba___Vinline_0__nba_sequent__TOP__1___VdfgRegularize_h6e95ff9d_0_3 
                                                 >> 0x00000027U))))
                            ? 0x0000007fffffffffULL
                            : ((2U == (3U & (IData)(
                                                    (__Vinline_0__eval_body__nba___Vinline_0__nba_sequent__TOP__1___VdfgRegularize_h6e95ff9d_0_3 
                                                     >> 0x00000027U))))
                                ? 0x0000008000000000ULL
                                : (0x000000ffffffffffULL 
                                   & __Vinline_0__eval_body__nba___Vinline_0__nba_sequent__TOP__1___VdfgRegularize_h6e95ff9d_0_3)));
                    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 
                        = (0x000001ffffffffffULL & 
                           (VL_SHIFTRS_QQI(41,41,32, 
                                           (0x000001ffffffffffULL 
                                            & VL_EXTENDS_QQ(41,40, __Vinline_0__eval_body__nba___Vinline_0__nba_sequent__TOP__1___VdfgRegularize_h6e95ff9d_0_0)), 0x0000000fU) 
                            + (QData)((IData)(((__Vinline_0__eval_body__nba___Vinline_0__nba_sequent__TOP__1___VdfgRegularize_h6e95ff9d_0_0 
                                                >> 0x0000000eU) 
                                               & (0ULL 
                                                  != 
                                                  (0x000000000000bfffULL 
                                                   & __Vinline_0__eval_body__nba___Vinline_0__nba_sequent__TOP__1___VdfgRegularize_h6e95ff9d_0_0)))))));
                }
            }
            if ((8ULL & vlSelfRef.__VnbaTriggered[0U])) {
                {
                    // Inlined CFunc: _act_sequent__TOP__0
                    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2 
                        = VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(vlSelfRef.tb_mac_unit__DOT__A)), 
                                      VL_EXTENDS_II(32,16, (IData)(vlSelfRef.tb_mac_unit__DOT__B)));
                }
            }
        }
        Vtb_mac_unit___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

bool Vtb_mac_unit___024root___eval_obs(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_obs\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    return (0U);
}

bool Vtb_mac_unit___024root___eval_react(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_react\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    return (0U);
}

void Vtb_mac_unit___024root___eval_postponed(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_postponed\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VlCoroutine Vtb_mac_unit___024root___eval_initial__TOP__Vtiming__0(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.tb_mac_unit__DOT__clk = 0U;
    while (true) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000001388ULL, 
                                             nullptr, 
                                             "tb\\tb_mac_unit.sv", 
                                             39);
        vlSelfRef.tb_mac_unit__DOT__clk = (1U & (~ (IData)(vlSelfRef.tb_mac_unit__DOT__clk)));
    }
    co_return;
}

void Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(Vtb_mac_unit___024root* vlSelf, const char* __VeventDescription);
VlCoroutine Vtb_mac_unit___024root___eval_initial__TOP__Vtiming__1____Vfork_1__0(Vtb_mac_unit___024root* vlSelf, VlForkSync __Vfork_1__sync);
VlCoroutine Vtb_mac_unit___024root___eval_initial__TOP__Vtiming__1____Vfork_1__1(Vtb_mac_unit___024root* vlSelf, VlForkSync __Vfork_1__sync);

VlCoroutine Vtb_mac_unit___024root___eval_initial__TOP__Vtiming__1(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __Vtask_tb_mac_unit__DOT__apply_reset__0__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_tb_mac_unit__DOT__apply_reset__0__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__1__a_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__1__a_in = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__1__b_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__1__b_in = 0;
    QData/*39:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__1__acc;
    __Vtask_tb_mac_unit__DOT__run_single_test__1__acc = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__1__exp;
    __Vtask_tb_mac_unit__DOT__run_single_test__1__exp = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__2__Vfuncout;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__Vfuncout = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__2__a;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__a = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__2__b;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__b = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__2__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__acc = 0;
    IData/*31:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__2__prod;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__prod = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__2__sum41;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__sum41 = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__2__sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__sum = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__2__G;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__G = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__2__L;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__L = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__2__St;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__St = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__2__inc;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__inc = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__2__shifted_sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__shifted_sum = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__check_result__3__exp;
    __Vtask_tb_mac_unit__DOT__check_result__3__exp = 0;
    CData/*0:0*/ __Vtask_tb_mac_unit__DOT__check_result__3__exp_valid;
    __Vtask_tb_mac_unit__DOT__check_result__3__exp_valid = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__4__a_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__4__a_in = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__4__b_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__4__b_in = 0;
    QData/*39:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__4__acc;
    __Vtask_tb_mac_unit__DOT__run_single_test__4__acc = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__4__exp;
    __Vtask_tb_mac_unit__DOT__run_single_test__4__exp = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__5__Vfuncout;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__Vfuncout = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__5__a;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__a = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__5__b;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__b = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__5__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__acc = 0;
    IData/*31:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__5__prod;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__prod = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__5__sum41;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__sum41 = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__5__sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__sum = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__5__G;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__G = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__5__L;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__L = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__5__St;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__St = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__5__inc;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__inc = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__5__shifted_sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__shifted_sum = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__check_result__6__exp;
    __Vtask_tb_mac_unit__DOT__check_result__6__exp = 0;
    CData/*0:0*/ __Vtask_tb_mac_unit__DOT__check_result__6__exp_valid;
    __Vtask_tb_mac_unit__DOT__check_result__6__exp_valid = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__7__a_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__7__a_in = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__7__b_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__7__b_in = 0;
    QData/*39:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__7__acc;
    __Vtask_tb_mac_unit__DOT__run_single_test__7__acc = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__7__exp;
    __Vtask_tb_mac_unit__DOT__run_single_test__7__exp = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__8__Vfuncout;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__Vfuncout = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__8__a;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__a = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__8__b;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__b = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__8__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__acc = 0;
    IData/*31:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__8__prod;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__prod = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__8__sum41;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__sum41 = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__8__sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__sum = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__8__G;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__G = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__8__L;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__L = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__8__St;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__St = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__8__inc;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__inc = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__8__shifted_sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__shifted_sum = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__check_result__9__exp;
    __Vtask_tb_mac_unit__DOT__check_result__9__exp = 0;
    CData/*0:0*/ __Vtask_tb_mac_unit__DOT__check_result__9__exp_valid;
    __Vtask_tb_mac_unit__DOT__check_result__9__exp_valid = 0;
    IData/*31:0*/ __Vtask_tb_mac_unit__DOT__apply_reset__10__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_tb_mac_unit__DOT__apply_reset__10__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__11__a_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__11__a_in = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__11__b_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__11__b_in = 0;
    QData/*39:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__11__acc;
    __Vtask_tb_mac_unit__DOT__run_single_test__11__acc = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__11__exp;
    __Vtask_tb_mac_unit__DOT__run_single_test__11__exp = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__12__Vfuncout;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__Vfuncout = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__12__a;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__a = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__12__b;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__b = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__12__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__acc = 0;
    IData/*31:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__12__prod;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__prod = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__12__sum41;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__sum41 = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__12__sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__sum = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__12__G;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__G = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__12__L;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__L = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__12__St;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__St = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__12__inc;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__inc = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__12__shifted_sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__shifted_sum = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__check_result__13__exp;
    __Vtask_tb_mac_unit__DOT__check_result__13__exp = 0;
    CData/*0:0*/ __Vtask_tb_mac_unit__DOT__check_result__13__exp_valid;
    __Vtask_tb_mac_unit__DOT__check_result__13__exp_valid = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__14__a_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__14__a_in = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__14__b_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__14__b_in = 0;
    QData/*39:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__14__acc;
    __Vtask_tb_mac_unit__DOT__run_single_test__14__acc = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__14__exp;
    __Vtask_tb_mac_unit__DOT__run_single_test__14__exp = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__15__Vfuncout;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__Vfuncout = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__15__a;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__a = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__15__b;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__b = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__15__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__acc = 0;
    IData/*31:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__15__prod;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__prod = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__15__sum41;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__sum41 = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__15__sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__sum = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__15__G;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__G = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__15__L;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__L = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__15__St;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__St = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__15__inc;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__inc = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__15__shifted_sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__shifted_sum = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__check_result__16__exp;
    __Vtask_tb_mac_unit__DOT__check_result__16__exp = 0;
    CData/*0:0*/ __Vtask_tb_mac_unit__DOT__check_result__16__exp_valid;
    __Vtask_tb_mac_unit__DOT__check_result__16__exp_valid = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__17__a_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__17__a_in = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__17__b_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__17__b_in = 0;
    QData/*39:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__17__acc;
    __Vtask_tb_mac_unit__DOT__run_single_test__17__acc = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__17__exp;
    __Vtask_tb_mac_unit__DOT__run_single_test__17__exp = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__18__Vfuncout;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__Vfuncout = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__18__a;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__a = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__18__b;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__b = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__18__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__acc = 0;
    IData/*31:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__18__prod;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__prod = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__18__sum41;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__sum41 = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__18__sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__sum = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__18__G;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__G = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__18__L;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__L = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__18__St;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__St = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__18__inc;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__inc = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__18__shifted_sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__shifted_sum = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__check_result__19__exp;
    __Vtask_tb_mac_unit__DOT__check_result__19__exp = 0;
    CData/*0:0*/ __Vtask_tb_mac_unit__DOT__check_result__19__exp_valid;
    __Vtask_tb_mac_unit__DOT__check_result__19__exp_valid = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__20__a_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__20__a_in = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__20__b_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__20__b_in = 0;
    QData/*39:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__20__acc;
    __Vtask_tb_mac_unit__DOT__run_single_test__20__acc = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__20__exp;
    __Vtask_tb_mac_unit__DOT__run_single_test__20__exp = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__21__Vfuncout;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__Vfuncout = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__21__a;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__a = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__21__b;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__b = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__21__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__acc = 0;
    IData/*31:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__21__prod;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__prod = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__21__sum41;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__sum41 = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__21__sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__sum = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__21__G;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__G = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__21__L;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__L = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__21__St;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__St = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__21__inc;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__inc = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__21__shifted_sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__shifted_sum = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__check_result__22__exp;
    __Vtask_tb_mac_unit__DOT__check_result__22__exp = 0;
    CData/*0:0*/ __Vtask_tb_mac_unit__DOT__check_result__22__exp_valid;
    __Vtask_tb_mac_unit__DOT__check_result__22__exp_valid = 0;
    IData/*31:0*/ __Vtask_tb_mac_unit__DOT__apply_reset__23__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_tb_mac_unit__DOT__apply_reset__23__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__test_pipeline__24____VlefCall_0__ref_model;
    __Vtask_tb_mac_unit__DOT__test_pipeline__24____VlefCall_0__ref_model = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__25__Vfuncout;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__Vfuncout = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__25__a;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__a = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__25__b;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__b = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__25__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__acc = 0;
    IData/*31:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__25__prod;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__prod = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__25__sum41;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__sum41 = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__25__sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__sum = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__25__G;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__G = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__25__L;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__L = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__25__St;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__St = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__25__inc;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__inc = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__test_valid_gating__27__exp_prev;
    __Vtask_tb_mac_unit__DOT__test_valid_gating__27__exp_prev = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__28__Vfuncout;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__Vfuncout = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__28__a;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__a = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__28__b;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__b = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__28__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__acc = 0;
    IData/*31:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__28__prod;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__prod = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__28__sum41;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__sum41 = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__28__sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__sum = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__28__G;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__G = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__28__L;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__L = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__28__St;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__St = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__28__inc;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__inc = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__28__shifted_sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__shifted_sum = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__29__a_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__29__a_in = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__29__b_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__29__b_in = 0;
    QData/*39:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__29__acc;
    __Vtask_tb_mac_unit__DOT__run_single_test__29__acc = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__29__exp;
    __Vtask_tb_mac_unit__DOT__run_single_test__29__exp = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__30__Vfuncout;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__Vfuncout = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__30__a;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__a = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__30__b;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__b = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__30__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__acc = 0;
    IData/*31:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__30__prod;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__prod = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__30__sum41;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__sum41 = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__30__sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__sum = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__30__G;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__G = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__30__L;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__L = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__30__St;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__St = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__30__inc;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__inc = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__30__shifted_sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__shifted_sum = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__check_result__31__exp;
    __Vtask_tb_mac_unit__DOT__check_result__31__exp = 0;
    CData/*0:0*/ __Vtask_tb_mac_unit__DOT__check_result__31__exp_valid;
    __Vtask_tb_mac_unit__DOT__check_result__31__exp_valid = 0;
    IData/*31:0*/ __Vtask_tb_mac_unit__DOT__apply_reset__32__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_tb_mac_unit__DOT__apply_reset__32__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__33__a_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__33__a_in = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__33__b_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__33__b_in = 0;
    QData/*39:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__33__acc;
    __Vtask_tb_mac_unit__DOT__run_single_test__33__acc = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__33__exp;
    __Vtask_tb_mac_unit__DOT__run_single_test__33__exp = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__34__Vfuncout;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__Vfuncout = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__34__a;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__a = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__34__b;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__b = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__34__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__acc = 0;
    IData/*31:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__34__prod;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__prod = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__34__sum41;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__sum41 = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__34__sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__sum = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__34__G;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__G = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__34__L;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__L = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__34__St;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__St = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__34__inc;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__inc = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__34__shifted_sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__shifted_sum = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__check_result__35__exp;
    __Vtask_tb_mac_unit__DOT__check_result__35__exp = 0;
    CData/*0:0*/ __Vtask_tb_mac_unit__DOT__check_result__35__exp_valid;
    __Vtask_tb_mac_unit__DOT__check_result__35__exp_valid = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__36__a_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__36__a_in = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__36__b_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__36__b_in = 0;
    QData/*39:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__36__acc;
    __Vtask_tb_mac_unit__DOT__run_single_test__36__acc = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__36__exp;
    __Vtask_tb_mac_unit__DOT__run_single_test__36__exp = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__37__Vfuncout;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__Vfuncout = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__37__a;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__a = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__37__b;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__b = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__37__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__acc = 0;
    IData/*31:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__37__prod;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__prod = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__37__sum41;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__sum41 = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__37__sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__sum = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__37__G;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__G = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__37__L;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__L = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__37__St;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__St = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__37__inc;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__inc = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__37__shifted_sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__shifted_sum = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__check_result__38__exp;
    __Vtask_tb_mac_unit__DOT__check_result__38__exp = 0;
    CData/*0:0*/ __Vtask_tb_mac_unit__DOT__check_result__38__exp_valid;
    __Vtask_tb_mac_unit__DOT__check_result__38__exp_valid = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__39__a_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__39__a_in = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__39__b_in;
    __Vtask_tb_mac_unit__DOT__run_single_test__39__b_in = 0;
    QData/*39:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__39__acc;
    __Vtask_tb_mac_unit__DOT__run_single_test__39__acc = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__run_single_test__39__exp;
    __Vtask_tb_mac_unit__DOT__run_single_test__39__exp = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__40__Vfuncout;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__Vfuncout = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__40__a;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__a = 0;
    SData/*15:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__40__b;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__b = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__40__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__acc = 0;
    IData/*31:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__40__prod;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__prod = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__40__sum41;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__sum41 = 0;
    QData/*39:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__40__sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__sum = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__40__G;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__G = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__40__L;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__L = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__40__St;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__St = 0;
    CData/*0:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__40__inc;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__inc = 0;
    QData/*40:0*/ __Vfunc_tb_mac_unit__DOT__ref_model__40__shifted_sum;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__shifted_sum = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__check_result__41__exp;
    __Vtask_tb_mac_unit__DOT__check_result__41__exp = 0;
    CData/*0:0*/ __Vtask_tb_mac_unit__DOT__check_result__41__exp_valid;
    __Vtask_tb_mac_unit__DOT__check_result__41__exp_valid = 0;
    IData/*31:0*/ __Vtask_tb_mac_unit__DOT__apply_reset__42__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_tb_mac_unit__DOT__apply_reset__42__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n;
    __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n = 0;
    VlUnpacked<VlWide<3>/*87:0*/, 15> __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__vec_mem;
    for (int __Vi0 = 0; __Vi0 < 15; ++__Vi0) {
        VL_ZERO_W(88, __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__vec_mem[__Vi0]);
    }
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__exp;
    __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__exp = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__check_result__44__exp;
    __Vtask_tb_mac_unit__DOT__check_result__44__exp = 0;
    CData/*0:0*/ __Vtask_tb_mac_unit__DOT__check_result__44__exp_valid;
    __Vtask_tb_mac_unit__DOT__check_result__44__exp_valid = 0;
    VlWide<3>/*95:0*/ __Vtemp_1;
    VlWide<3>/*95:0*/ __Vtemp_2;
    VlWide<3>/*95:0*/ __Vtemp_3;
    VlWide<3>/*95:0*/ __Vtemp_4;
    // Body
    vlSelfRef.tb_mac_unit__DOT__clk = 0U;
    vlSelfRef.tb_mac_unit__DOT__rst_n = 1U;
    vlSelfRef.tb_mac_unit__DOT__valid_in = 0U;
    vlSelfRef.tb_mac_unit__DOT__A = 0U;
    vlSelfRef.tb_mac_unit__DOT__B = 0U;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = 0ULL;
    VL_WRITEF_NX("\n========================================\nStarting MAC Unit Testbench...\n========================================\n\n",0);
    __Vtask_tb_mac_unit__DOT__apply_reset__0__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         109);
    vlSelfRef.tb_mac_unit__DOT__rst_n = 0U;
    vlSelfRef.tb_mac_unit__DOT__valid_in = 0U;
    vlSelfRef.tb_mac_unit__DOT__A = 0U;
    vlSelfRef.tb_mac_unit__DOT__B = 0U;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = 0ULL;
    __Vtask_tb_mac_unit__DOT__apply_reset__0__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_tb_mac_unit__DOT__apply_reset__0__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                           "@(negedge tb_mac_unit.clk)");
        co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge tb_mac_unit.clk)", 
                                                             "tb\\tb_mac_unit.sv", 
                                                             115);
        __Vtask_tb_mac_unit__DOT__apply_reset__0__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_tb_mac_unit__DOT__apply_reset__0__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    vlSelfRef.tb_mac_unit__DOT__rst_n = 1U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         117);
    vlSelfRef.tb_mac_unit__DOT__total_tests = ((IData)(1U) 
                                               + vlSelfRef.tb_mac_unit__DOT__total_tests);
    if (VL_LIKELY((((0U == (IData)(vlSelfRef.tb_mac_unit__DOT__Out)) 
                    & (~ (IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out)))))) {
        VL_WRITEF_NX("[PASS] 1. Reset Test\n",0);
        vlSelfRef.tb_mac_unit__DOT__tests_passed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_passed);
    } else {
        VL_WRITEF_NX("[%0t] %%Error: tb_mac_unit.sv:248: Assertion failed in %m: [FAIL] 1. Reset Test | Out=%h Valid=%b\n",5, 'M',vlSymsp->name(),"tb_mac_unit", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        VL_STOP_MT("tb\\tb_mac_unit.sv", 248, "");
        vlSelfRef.tb_mac_unit__DOT__tests_failed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_failed);
    }
    __Vtask_tb_mac_unit__DOT__run_single_test__1__acc = 0ULL;
    __Vtask_tb_mac_unit__DOT__run_single_test__1__b_in = 0x4000U;
    __Vtask_tb_mac_unit__DOT__run_single_test__1__a_in = 0x4000U;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__1__name = "2. Basic Multiply (0.5 * 0.5)"s;
    __Vtask_tb_mac_unit__DOT__run_single_test__1__exp = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__acc = __Vtask_tb_mac_unit__DOT__run_single_test__1__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__b = __Vtask_tb_mac_unit__DOT__run_single_test__1__b_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__a = __Vtask_tb_mac_unit__DOT__run_single_test__1__a_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__Vfuncout = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__prod = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__sum41 = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__G = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__L = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__St = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__inc = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__shifted_sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__2__prod = 
        VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__2__a)), 
                    VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__2__b)));
    __Vfunc_tb_mac_unit__DOT__ref_model__2__sum41 = 
        (0x000001ffffffffffULL & (VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__2__acc) 
                                  + VL_EXTENDS_QI(41,32, __Vfunc_tb_mac_unit__DOT__ref_model__2__prod)));
    __Vfunc_tb_mac_unit__DOT__ref_model__2__sum = (
                                                   (1U 
                                                    == 
                                                    (3U 
                                                     & (IData)(
                                                               (__Vfunc_tb_mac_unit__DOT__ref_model__2__sum41 
                                                                >> 0x27U))))
                                                    ? 0x0000007fffffffffULL
                                                    : 
                                                   ((2U 
                                                     == 
                                                     (3U 
                                                      & (IData)(
                                                                (__Vfunc_tb_mac_unit__DOT__ref_model__2__sum41 
                                                                 >> 0x27U))))
                                                     ? 0x0000008000000000ULL
                                                     : 
                                                    (0x000000ffffffffffULL 
                                                     & __Vfunc_tb_mac_unit__DOT__ref_model__2__sum41)));
    __Vfunc_tb_mac_unit__DOT__ref_model__2__G = (1U 
                                                 & (IData)(
                                                           (__Vfunc_tb_mac_unit__DOT__ref_model__2__sum 
                                                            >> 0x0eU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__2__St = (0U 
                                                  != 
                                                  (0x00003fffU 
                                                   & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__2__sum)));
    __Vfunc_tb_mac_unit__DOT__ref_model__2__L = (1U 
                                                 & (IData)(
                                                           (__Vfunc_tb_mac_unit__DOT__ref_model__2__sum 
                                                            >> 0x0fU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__2__inc = ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__2__G) 
                                                   & ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__2__St) 
                                                      | (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__2__L)));
    __Vfunc_tb_mac_unit__DOT__ref_model__2__shifted_sum 
        = (0x000001ffffffffffULL & (VL_SHIFTRS_QQI(41,41,32, 
                                                   (0x000001ffffffffffULL 
                                                    & VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__2__sum)), 0x0000000fU) 
                                    + (QData)((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__2__inc))));
    __Vfunc_tb_mac_unit__DOT__ref_model__2__Vfuncout 
        = (VL_LTS_IQQ(41, 0x0000000000007fffULL, __Vfunc_tb_mac_unit__DOT__ref_model__2__shifted_sum)
            ? 0x00007fffU : (VL_GTS_IQQ(41, 0x000001ffffff8000ULL, __Vfunc_tb_mac_unit__DOT__ref_model__2__shifted_sum)
                              ? 0x00008000U : (0x0000ffffU 
                                               & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__2__shifted_sum))));
    __Vtask_tb_mac_unit__DOT__run_single_test__1__exp 
        = __Vfunc_tb_mac_unit__DOT__ref_model__2__Vfuncout;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         130);
    vlSelfRef.tb_mac_unit__DOT__A = __Vtask_tb_mac_unit__DOT__run_single_test__1__a_in;
    vlSelfRef.tb_mac_unit__DOT__B = __Vtask_tb_mac_unit__DOT__run_single_test__1__b_in;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = __Vtask_tb_mac_unit__DOT__run_single_test__1__acc;
    vlSelfRef.tb_mac_unit__DOT__valid_in = 1U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         136);
    vlSelfRef.tb_mac_unit__DOT__valid_in = 0U;
    vlSelfRef.tb_mac_unit__DOT__A = 0U;
    vlSelfRef.tb_mac_unit__DOT__B = 0U;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = 0ULL;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         140);
    __Vtask_tb_mac_unit__DOT__check_result__3__exp_valid = 1U;
    __Vtask_tb_mac_unit__DOT__check_result__3__exp 
        = __Vtask_tb_mac_unit__DOT__run_single_test__1__exp;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__3__name 
        = vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__1__name;
    vlSelfRef.tb_mac_unit__DOT__total_tests = ((IData)(1U) 
                                               + vlSelfRef.tb_mac_unit__DOT__total_tests);
    if (VL_LIKELY(((((IData)(vlSelfRef.tb_mac_unit__DOT__Out) 
                     == (IData)(__Vtask_tb_mac_unit__DOT__check_result__3__exp)) 
                    & ((IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out) 
                       == (IData)(__Vtask_tb_mac_unit__DOT__check_result__3__exp_valid)))))) {
        VL_WRITEF_NX("[PASS] %s | Out=%h Valid=%b\n",3
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__3__name)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        vlSelfRef.tb_mac_unit__DOT__tests_passed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_passed);
    } else {
        VL_WRITEF_NX("[%0t] %%Error: tb_mac_unit.sv:102: Assertion failed in %m: [FAIL] %s | Expected: Out=%h Valid=%b | Got: Out=%h Valid=%b\n",8, 'M',vlSymsp->name(),"tb_mac_unit.check_result", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__3__name)
                     , '~',16,(IData)(__Vtask_tb_mac_unit__DOT__check_result__3__exp)
                     , '#',1,(IData)(__Vtask_tb_mac_unit__DOT__check_result__3__exp_valid)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        VL_STOP_MT("tb\\tb_mac_unit.sv", 102, "");
        vlSelfRef.tb_mac_unit__DOT__tests_failed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_failed);
    }
    __Vtask_tb_mac_unit__DOT__run_single_test__4__acc = 0ULL;
    __Vtask_tb_mac_unit__DOT__run_single_test__4__b_in = 0x4000U;
    __Vtask_tb_mac_unit__DOT__run_single_test__4__a_in = 0xc000U;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__4__name = "3. Negative Multiply (-0.5 * 0.5)"s;
    __Vtask_tb_mac_unit__DOT__run_single_test__4__exp = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__acc = __Vtask_tb_mac_unit__DOT__run_single_test__4__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__b = __Vtask_tb_mac_unit__DOT__run_single_test__4__b_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__a = __Vtask_tb_mac_unit__DOT__run_single_test__4__a_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__Vfuncout = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__prod = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__sum41 = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__G = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__L = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__St = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__inc = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__shifted_sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__5__prod = 
        VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__5__a)), 
                    VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__5__b)));
    __Vfunc_tb_mac_unit__DOT__ref_model__5__sum41 = 
        (0x000001ffffffffffULL & (VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__5__acc) 
                                  + VL_EXTENDS_QI(41,32, __Vfunc_tb_mac_unit__DOT__ref_model__5__prod)));
    __Vfunc_tb_mac_unit__DOT__ref_model__5__sum = (
                                                   (1U 
                                                    == 
                                                    (3U 
                                                     & (IData)(
                                                               (__Vfunc_tb_mac_unit__DOT__ref_model__5__sum41 
                                                                >> 0x27U))))
                                                    ? 0x0000007fffffffffULL
                                                    : 
                                                   ((2U 
                                                     == 
                                                     (3U 
                                                      & (IData)(
                                                                (__Vfunc_tb_mac_unit__DOT__ref_model__5__sum41 
                                                                 >> 0x27U))))
                                                     ? 0x0000008000000000ULL
                                                     : 
                                                    (0x000000ffffffffffULL 
                                                     & __Vfunc_tb_mac_unit__DOT__ref_model__5__sum41)));
    __Vfunc_tb_mac_unit__DOT__ref_model__5__G = (1U 
                                                 & (IData)(
                                                           (__Vfunc_tb_mac_unit__DOT__ref_model__5__sum 
                                                            >> 0x0eU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__5__St = (0U 
                                                  != 
                                                  (0x00003fffU 
                                                   & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__5__sum)));
    __Vfunc_tb_mac_unit__DOT__ref_model__5__L = (1U 
                                                 & (IData)(
                                                           (__Vfunc_tb_mac_unit__DOT__ref_model__5__sum 
                                                            >> 0x0fU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__5__inc = ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__5__G) 
                                                   & ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__5__St) 
                                                      | (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__5__L)));
    __Vfunc_tb_mac_unit__DOT__ref_model__5__shifted_sum 
        = (0x000001ffffffffffULL & (VL_SHIFTRS_QQI(41,41,32, 
                                                   (0x000001ffffffffffULL 
                                                    & VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__5__sum)), 0x0000000fU) 
                                    + (QData)((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__5__inc))));
    __Vfunc_tb_mac_unit__DOT__ref_model__5__Vfuncout 
        = (VL_LTS_IQQ(41, 0x0000000000007fffULL, __Vfunc_tb_mac_unit__DOT__ref_model__5__shifted_sum)
            ? 0x00007fffU : (VL_GTS_IQQ(41, 0x000001ffffff8000ULL, __Vfunc_tb_mac_unit__DOT__ref_model__5__shifted_sum)
                              ? 0x00008000U : (0x0000ffffU 
                                               & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__5__shifted_sum))));
    __Vtask_tb_mac_unit__DOT__run_single_test__4__exp 
        = __Vfunc_tb_mac_unit__DOT__ref_model__5__Vfuncout;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         130);
    vlSelfRef.tb_mac_unit__DOT__A = __Vtask_tb_mac_unit__DOT__run_single_test__4__a_in;
    vlSelfRef.tb_mac_unit__DOT__B = __Vtask_tb_mac_unit__DOT__run_single_test__4__b_in;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = __Vtask_tb_mac_unit__DOT__run_single_test__4__acc;
    vlSelfRef.tb_mac_unit__DOT__valid_in = 1U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         136);
    vlSelfRef.tb_mac_unit__DOT__valid_in = 0U;
    vlSelfRef.tb_mac_unit__DOT__A = 0U;
    vlSelfRef.tb_mac_unit__DOT__B = 0U;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = 0ULL;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         140);
    __Vtask_tb_mac_unit__DOT__check_result__6__exp_valid = 1U;
    __Vtask_tb_mac_unit__DOT__check_result__6__exp 
        = __Vtask_tb_mac_unit__DOT__run_single_test__4__exp;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__6__name 
        = vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__4__name;
    vlSelfRef.tb_mac_unit__DOT__total_tests = ((IData)(1U) 
                                               + vlSelfRef.tb_mac_unit__DOT__total_tests);
    if (VL_LIKELY(((((IData)(vlSelfRef.tb_mac_unit__DOT__Out) 
                     == (IData)(__Vtask_tb_mac_unit__DOT__check_result__6__exp)) 
                    & ((IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out) 
                       == (IData)(__Vtask_tb_mac_unit__DOT__check_result__6__exp_valid)))))) {
        VL_WRITEF_NX("[PASS] %s | Out=%h Valid=%b\n",3
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__6__name)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        vlSelfRef.tb_mac_unit__DOT__tests_passed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_passed);
    } else {
        VL_WRITEF_NX("[%0t] %%Error: tb_mac_unit.sv:102: Assertion failed in %m: [FAIL] %s | Expected: Out=%h Valid=%b | Got: Out=%h Valid=%b\n",8, 'M',vlSymsp->name(),"tb_mac_unit.check_result", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__6__name)
                     , '~',16,(IData)(__Vtask_tb_mac_unit__DOT__check_result__6__exp)
                     , '#',1,(IData)(__Vtask_tb_mac_unit__DOT__check_result__6__exp_valid)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        VL_STOP_MT("tb\\tb_mac_unit.sv", 102, "");
        vlSelfRef.tb_mac_unit__DOT__tests_failed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_failed);
    }
    __Vtask_tb_mac_unit__DOT__run_single_test__7__acc = 0x0000000020000000ULL;
    __Vtask_tb_mac_unit__DOT__run_single_test__7__b_in = 0x4000U;
    __Vtask_tb_mac_unit__DOT__run_single_test__7__a_in = 0x4000U;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__7__name = "4. Accumulate Test (0.25 + 0.5)"s;
    __Vtask_tb_mac_unit__DOT__run_single_test__7__exp = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__acc = __Vtask_tb_mac_unit__DOT__run_single_test__7__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__b = __Vtask_tb_mac_unit__DOT__run_single_test__7__b_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__a = __Vtask_tb_mac_unit__DOT__run_single_test__7__a_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__Vfuncout = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__prod = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__sum41 = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__G = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__L = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__St = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__inc = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__shifted_sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__8__prod = 
        VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__8__a)), 
                    VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__8__b)));
    __Vfunc_tb_mac_unit__DOT__ref_model__8__sum41 = 
        (0x000001ffffffffffULL & (VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__8__acc) 
                                  + VL_EXTENDS_QI(41,32, __Vfunc_tb_mac_unit__DOT__ref_model__8__prod)));
    __Vfunc_tb_mac_unit__DOT__ref_model__8__sum = (
                                                   (1U 
                                                    == 
                                                    (3U 
                                                     & (IData)(
                                                               (__Vfunc_tb_mac_unit__DOT__ref_model__8__sum41 
                                                                >> 0x27U))))
                                                    ? 0x0000007fffffffffULL
                                                    : 
                                                   ((2U 
                                                     == 
                                                     (3U 
                                                      & (IData)(
                                                                (__Vfunc_tb_mac_unit__DOT__ref_model__8__sum41 
                                                                 >> 0x27U))))
                                                     ? 0x0000008000000000ULL
                                                     : 
                                                    (0x000000ffffffffffULL 
                                                     & __Vfunc_tb_mac_unit__DOT__ref_model__8__sum41)));
    __Vfunc_tb_mac_unit__DOT__ref_model__8__G = (1U 
                                                 & (IData)(
                                                           (__Vfunc_tb_mac_unit__DOT__ref_model__8__sum 
                                                            >> 0x0eU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__8__St = (0U 
                                                  != 
                                                  (0x00003fffU 
                                                   & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__8__sum)));
    __Vfunc_tb_mac_unit__DOT__ref_model__8__L = (1U 
                                                 & (IData)(
                                                           (__Vfunc_tb_mac_unit__DOT__ref_model__8__sum 
                                                            >> 0x0fU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__8__inc = ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__8__G) 
                                                   & ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__8__St) 
                                                      | (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__8__L)));
    __Vfunc_tb_mac_unit__DOT__ref_model__8__shifted_sum 
        = (0x000001ffffffffffULL & (VL_SHIFTRS_QQI(41,41,32, 
                                                   (0x000001ffffffffffULL 
                                                    & VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__8__sum)), 0x0000000fU) 
                                    + (QData)((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__8__inc))));
    __Vfunc_tb_mac_unit__DOT__ref_model__8__Vfuncout 
        = (VL_LTS_IQQ(41, 0x0000000000007fffULL, __Vfunc_tb_mac_unit__DOT__ref_model__8__shifted_sum)
            ? 0x00007fffU : (VL_GTS_IQQ(41, 0x000001ffffff8000ULL, __Vfunc_tb_mac_unit__DOT__ref_model__8__shifted_sum)
                              ? 0x00008000U : (0x0000ffffU 
                                               & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__8__shifted_sum))));
    __Vtask_tb_mac_unit__DOT__run_single_test__7__exp 
        = __Vfunc_tb_mac_unit__DOT__ref_model__8__Vfuncout;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         130);
    vlSelfRef.tb_mac_unit__DOT__A = __Vtask_tb_mac_unit__DOT__run_single_test__7__a_in;
    vlSelfRef.tb_mac_unit__DOT__B = __Vtask_tb_mac_unit__DOT__run_single_test__7__b_in;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = __Vtask_tb_mac_unit__DOT__run_single_test__7__acc;
    vlSelfRef.tb_mac_unit__DOT__valid_in = 1U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         136);
    vlSelfRef.tb_mac_unit__DOT__valid_in = 0U;
    vlSelfRef.tb_mac_unit__DOT__A = 0U;
    vlSelfRef.tb_mac_unit__DOT__B = 0U;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = 0ULL;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         140);
    __Vtask_tb_mac_unit__DOT__check_result__9__exp_valid = 1U;
    __Vtask_tb_mac_unit__DOT__check_result__9__exp 
        = __Vtask_tb_mac_unit__DOT__run_single_test__7__exp;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__9__name 
        = vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__7__name;
    vlSelfRef.tb_mac_unit__DOT__total_tests = ((IData)(1U) 
                                               + vlSelfRef.tb_mac_unit__DOT__total_tests);
    if (VL_LIKELY(((((IData)(vlSelfRef.tb_mac_unit__DOT__Out) 
                     == (IData)(__Vtask_tb_mac_unit__DOT__check_result__9__exp)) 
                    & ((IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out) 
                       == (IData)(__Vtask_tb_mac_unit__DOT__check_result__9__exp_valid)))))) {
        VL_WRITEF_NX("[PASS] %s | Out=%h Valid=%b\n",3
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__9__name)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        vlSelfRef.tb_mac_unit__DOT__tests_passed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_passed);
    } else {
        VL_WRITEF_NX("[%0t] %%Error: tb_mac_unit.sv:102: Assertion failed in %m: [FAIL] %s | Expected: Out=%h Valid=%b | Got: Out=%h Valid=%b\n",8, 'M',vlSymsp->name(),"tb_mac_unit.check_result", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__9__name)
                     , '~',16,(IData)(__Vtask_tb_mac_unit__DOT__check_result__9__exp)
                     , '#',1,(IData)(__Vtask_tb_mac_unit__DOT__check_result__9__exp_valid)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        VL_STOP_MT("tb\\tb_mac_unit.sv", 102, "");
        vlSelfRef.tb_mac_unit__DOT__tests_failed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_failed);
    }
    __Vtask_tb_mac_unit__DOT__apply_reset__10__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         109);
    vlSelfRef.tb_mac_unit__DOT__rst_n = 0U;
    vlSelfRef.tb_mac_unit__DOT__valid_in = 0U;
    vlSelfRef.tb_mac_unit__DOT__A = 0U;
    vlSelfRef.tb_mac_unit__DOT__B = 0U;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = 0ULL;
    __Vtask_tb_mac_unit__DOT__apply_reset__10__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_tb_mac_unit__DOT__apply_reset__10__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                           "@(negedge tb_mac_unit.clk)");
        co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge tb_mac_unit.clk)", 
                                                             "tb\\tb_mac_unit.sv", 
                                                             115);
        __Vtask_tb_mac_unit__DOT__apply_reset__10__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_tb_mac_unit__DOT__apply_reset__10__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    vlSelfRef.tb_mac_unit__DOT__rst_n = 1U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         117);
    __Vtask_tb_mac_unit__DOT__run_single_test__11__acc = 0x0000003fffffffffULL;
    __Vtask_tb_mac_unit__DOT__run_single_test__11__b_in = 0x7fffU;
    __Vtask_tb_mac_unit__DOT__run_single_test__11__a_in = 0x7fffU;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__11__name = "5. Positive Saturation"s;
    __Vtask_tb_mac_unit__DOT__run_single_test__11__exp = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__acc = __Vtask_tb_mac_unit__DOT__run_single_test__11__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__b = __Vtask_tb_mac_unit__DOT__run_single_test__11__b_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__a = __Vtask_tb_mac_unit__DOT__run_single_test__11__a_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__Vfuncout = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__prod = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__sum41 = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__G = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__L = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__St = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__inc = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__shifted_sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__12__prod = 
        VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__12__a)), 
                    VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__12__b)));
    __Vfunc_tb_mac_unit__DOT__ref_model__12__sum41 
        = (0x000001ffffffffffULL & (VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__12__acc) 
                                    + VL_EXTENDS_QI(41,32, __Vfunc_tb_mac_unit__DOT__ref_model__12__prod)));
    __Vfunc_tb_mac_unit__DOT__ref_model__12__sum = 
        ((1U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__12__sum41 
                               >> 0x27U)))) ? 0x0000007fffffffffULL
          : ((2U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__12__sum41 
                                   >> 0x27U)))) ? 0x0000008000000000ULL
              : (0x000000ffffffffffULL & __Vfunc_tb_mac_unit__DOT__ref_model__12__sum41)));
    __Vfunc_tb_mac_unit__DOT__ref_model__12__G = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__12__sum 
                                                             >> 0x0eU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__12__St = (0U 
                                                   != 
                                                   (0x00003fffU 
                                                    & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__12__sum)));
    __Vfunc_tb_mac_unit__DOT__ref_model__12__L = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__12__sum 
                                                             >> 0x0fU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__12__inc = 
        ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__12__G) 
         & ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__12__St) 
            | (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__12__L)));
    __Vfunc_tb_mac_unit__DOT__ref_model__12__shifted_sum 
        = (0x000001ffffffffffULL & (VL_SHIFTRS_QQI(41,41,32, 
                                                   (0x000001ffffffffffULL 
                                                    & VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__12__sum)), 0x0000000fU) 
                                    + (QData)((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__12__inc))));
    __Vfunc_tb_mac_unit__DOT__ref_model__12__Vfuncout 
        = (VL_LTS_IQQ(41, 0x0000000000007fffULL, __Vfunc_tb_mac_unit__DOT__ref_model__12__shifted_sum)
            ? 0x00007fffU : (VL_GTS_IQQ(41, 0x000001ffffff8000ULL, __Vfunc_tb_mac_unit__DOT__ref_model__12__shifted_sum)
                              ? 0x00008000U : (0x0000ffffU 
                                               & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__12__shifted_sum))));
    __Vtask_tb_mac_unit__DOT__run_single_test__11__exp 
        = __Vfunc_tb_mac_unit__DOT__ref_model__12__Vfuncout;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         130);
    vlSelfRef.tb_mac_unit__DOT__A = __Vtask_tb_mac_unit__DOT__run_single_test__11__a_in;
    vlSelfRef.tb_mac_unit__DOT__B = __Vtask_tb_mac_unit__DOT__run_single_test__11__b_in;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = __Vtask_tb_mac_unit__DOT__run_single_test__11__acc;
    vlSelfRef.tb_mac_unit__DOT__valid_in = 1U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         136);
    vlSelfRef.tb_mac_unit__DOT__valid_in = 0U;
    vlSelfRef.tb_mac_unit__DOT__A = 0U;
    vlSelfRef.tb_mac_unit__DOT__B = 0U;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = 0ULL;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         140);
    __Vtask_tb_mac_unit__DOT__check_result__13__exp_valid = 1U;
    __Vtask_tb_mac_unit__DOT__check_result__13__exp 
        = __Vtask_tb_mac_unit__DOT__run_single_test__11__exp;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__13__name 
        = vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__11__name;
    vlSelfRef.tb_mac_unit__DOT__total_tests = ((IData)(1U) 
                                               + vlSelfRef.tb_mac_unit__DOT__total_tests);
    if (VL_LIKELY(((((IData)(vlSelfRef.tb_mac_unit__DOT__Out) 
                     == (IData)(__Vtask_tb_mac_unit__DOT__check_result__13__exp)) 
                    & ((IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out) 
                       == (IData)(__Vtask_tb_mac_unit__DOT__check_result__13__exp_valid)))))) {
        VL_WRITEF_NX("[PASS] %s | Out=%h Valid=%b\n",3
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__13__name)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        vlSelfRef.tb_mac_unit__DOT__tests_passed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_passed);
    } else {
        VL_WRITEF_NX("[%0t] %%Error: tb_mac_unit.sv:102: Assertion failed in %m: [FAIL] %s | Expected: Out=%h Valid=%b | Got: Out=%h Valid=%b\n",8, 'M',vlSymsp->name(),"tb_mac_unit.check_result", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__13__name)
                     , '~',16,(IData)(__Vtask_tb_mac_unit__DOT__check_result__13__exp)
                     , '#',1,(IData)(__Vtask_tb_mac_unit__DOT__check_result__13__exp_valid)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        VL_STOP_MT("tb\\tb_mac_unit.sv", 102, "");
        vlSelfRef.tb_mac_unit__DOT__tests_failed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_failed);
    }
    __Vtask_tb_mac_unit__DOT__run_single_test__14__acc = 0x0000008000000000ULL;
    __Vtask_tb_mac_unit__DOT__run_single_test__14__b_in = 0x7fffU;
    __Vtask_tb_mac_unit__DOT__run_single_test__14__a_in = 0x8000U;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__14__name = "6. Negative Saturation"s;
    __Vtask_tb_mac_unit__DOT__run_single_test__14__exp = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__acc = __Vtask_tb_mac_unit__DOT__run_single_test__14__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__b = __Vtask_tb_mac_unit__DOT__run_single_test__14__b_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__a = __Vtask_tb_mac_unit__DOT__run_single_test__14__a_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__Vfuncout = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__prod = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__sum41 = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__G = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__L = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__St = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__inc = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__shifted_sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__15__prod = 
        VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__15__a)), 
                    VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__15__b)));
    __Vfunc_tb_mac_unit__DOT__ref_model__15__sum41 
        = (0x000001ffffffffffULL & (VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__15__acc) 
                                    + VL_EXTENDS_QI(41,32, __Vfunc_tb_mac_unit__DOT__ref_model__15__prod)));
    __Vfunc_tb_mac_unit__DOT__ref_model__15__sum = 
        ((1U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__15__sum41 
                               >> 0x27U)))) ? 0x0000007fffffffffULL
          : ((2U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__15__sum41 
                                   >> 0x27U)))) ? 0x0000008000000000ULL
              : (0x000000ffffffffffULL & __Vfunc_tb_mac_unit__DOT__ref_model__15__sum41)));
    __Vfunc_tb_mac_unit__DOT__ref_model__15__G = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__15__sum 
                                                             >> 0x0eU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__15__St = (0U 
                                                   != 
                                                   (0x00003fffU 
                                                    & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__15__sum)));
    __Vfunc_tb_mac_unit__DOT__ref_model__15__L = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__15__sum 
                                                             >> 0x0fU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__15__inc = 
        ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__15__G) 
         & ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__15__St) 
            | (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__15__L)));
    __Vfunc_tb_mac_unit__DOT__ref_model__15__shifted_sum 
        = (0x000001ffffffffffULL & (VL_SHIFTRS_QQI(41,41,32, 
                                                   (0x000001ffffffffffULL 
                                                    & VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__15__sum)), 0x0000000fU) 
                                    + (QData)((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__15__inc))));
    __Vfunc_tb_mac_unit__DOT__ref_model__15__Vfuncout 
        = (VL_LTS_IQQ(41, 0x0000000000007fffULL, __Vfunc_tb_mac_unit__DOT__ref_model__15__shifted_sum)
            ? 0x00007fffU : (VL_GTS_IQQ(41, 0x000001ffffff8000ULL, __Vfunc_tb_mac_unit__DOT__ref_model__15__shifted_sum)
                              ? 0x00008000U : (0x0000ffffU 
                                               & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__15__shifted_sum))));
    __Vtask_tb_mac_unit__DOT__run_single_test__14__exp 
        = __Vfunc_tb_mac_unit__DOT__ref_model__15__Vfuncout;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         130);
    vlSelfRef.tb_mac_unit__DOT__A = __Vtask_tb_mac_unit__DOT__run_single_test__14__a_in;
    vlSelfRef.tb_mac_unit__DOT__B = __Vtask_tb_mac_unit__DOT__run_single_test__14__b_in;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = __Vtask_tb_mac_unit__DOT__run_single_test__14__acc;
    vlSelfRef.tb_mac_unit__DOT__valid_in = 1U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         136);
    vlSelfRef.tb_mac_unit__DOT__valid_in = 0U;
    vlSelfRef.tb_mac_unit__DOT__A = 0U;
    vlSelfRef.tb_mac_unit__DOT__B = 0U;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = 0ULL;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         140);
    __Vtask_tb_mac_unit__DOT__check_result__16__exp_valid = 1U;
    __Vtask_tb_mac_unit__DOT__check_result__16__exp 
        = __Vtask_tb_mac_unit__DOT__run_single_test__14__exp;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__16__name 
        = vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__14__name;
    vlSelfRef.tb_mac_unit__DOT__total_tests = ((IData)(1U) 
                                               + vlSelfRef.tb_mac_unit__DOT__total_tests);
    if (VL_LIKELY(((((IData)(vlSelfRef.tb_mac_unit__DOT__Out) 
                     == (IData)(__Vtask_tb_mac_unit__DOT__check_result__16__exp)) 
                    & ((IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out) 
                       == (IData)(__Vtask_tb_mac_unit__DOT__check_result__16__exp_valid)))))) {
        VL_WRITEF_NX("[PASS] %s | Out=%h Valid=%b\n",3
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__16__name)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        vlSelfRef.tb_mac_unit__DOT__tests_passed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_passed);
    } else {
        VL_WRITEF_NX("[%0t] %%Error: tb_mac_unit.sv:102: Assertion failed in %m: [FAIL] %s | Expected: Out=%h Valid=%b | Got: Out=%h Valid=%b\n",8, 'M',vlSymsp->name(),"tb_mac_unit.check_result", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__16__name)
                     , '~',16,(IData)(__Vtask_tb_mac_unit__DOT__check_result__16__exp)
                     , '#',1,(IData)(__Vtask_tb_mac_unit__DOT__check_result__16__exp_valid)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        VL_STOP_MT("tb\\tb_mac_unit.sv", 102, "");
        vlSelfRef.tb_mac_unit__DOT__tests_failed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_failed);
    }
    __Vtask_tb_mac_unit__DOT__run_single_test__17__acc = 0ULL;
    __Vtask_tb_mac_unit__DOT__run_single_test__17__b_in = 0x8000U;
    __Vtask_tb_mac_unit__DOT__run_single_test__17__a_in = 0x8000U;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__17__name = "7. Special Case A=B=16'h8000"s;
    __Vtask_tb_mac_unit__DOT__run_single_test__17__exp = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__acc = __Vtask_tb_mac_unit__DOT__run_single_test__17__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__b = __Vtask_tb_mac_unit__DOT__run_single_test__17__b_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__a = __Vtask_tb_mac_unit__DOT__run_single_test__17__a_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__Vfuncout = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__prod = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__sum41 = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__G = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__L = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__St = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__inc = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__shifted_sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__18__prod = 
        VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__18__a)), 
                    VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__18__b)));
    __Vfunc_tb_mac_unit__DOT__ref_model__18__sum41 
        = (0x000001ffffffffffULL & (VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__18__acc) 
                                    + VL_EXTENDS_QI(41,32, __Vfunc_tb_mac_unit__DOT__ref_model__18__prod)));
    __Vfunc_tb_mac_unit__DOT__ref_model__18__sum = 
        ((1U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__18__sum41 
                               >> 0x27U)))) ? 0x0000007fffffffffULL
          : ((2U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__18__sum41 
                                   >> 0x27U)))) ? 0x0000008000000000ULL
              : (0x000000ffffffffffULL & __Vfunc_tb_mac_unit__DOT__ref_model__18__sum41)));
    __Vfunc_tb_mac_unit__DOT__ref_model__18__G = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__18__sum 
                                                             >> 0x0eU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__18__St = (0U 
                                                   != 
                                                   (0x00003fffU 
                                                    & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__18__sum)));
    __Vfunc_tb_mac_unit__DOT__ref_model__18__L = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__18__sum 
                                                             >> 0x0fU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__18__inc = 
        ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__18__G) 
         & ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__18__St) 
            | (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__18__L)));
    __Vfunc_tb_mac_unit__DOT__ref_model__18__shifted_sum 
        = (0x000001ffffffffffULL & (VL_SHIFTRS_QQI(41,41,32, 
                                                   (0x000001ffffffffffULL 
                                                    & VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__18__sum)), 0x0000000fU) 
                                    + (QData)((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__18__inc))));
    __Vfunc_tb_mac_unit__DOT__ref_model__18__Vfuncout 
        = (VL_LTS_IQQ(41, 0x0000000000007fffULL, __Vfunc_tb_mac_unit__DOT__ref_model__18__shifted_sum)
            ? 0x00007fffU : (VL_GTS_IQQ(41, 0x000001ffffff8000ULL, __Vfunc_tb_mac_unit__DOT__ref_model__18__shifted_sum)
                              ? 0x00008000U : (0x0000ffffU 
                                               & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__18__shifted_sum))));
    __Vtask_tb_mac_unit__DOT__run_single_test__17__exp 
        = __Vfunc_tb_mac_unit__DOT__ref_model__18__Vfuncout;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         130);
    vlSelfRef.tb_mac_unit__DOT__A = __Vtask_tb_mac_unit__DOT__run_single_test__17__a_in;
    vlSelfRef.tb_mac_unit__DOT__B = __Vtask_tb_mac_unit__DOT__run_single_test__17__b_in;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = __Vtask_tb_mac_unit__DOT__run_single_test__17__acc;
    vlSelfRef.tb_mac_unit__DOT__valid_in = 1U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         136);
    vlSelfRef.tb_mac_unit__DOT__valid_in = 0U;
    vlSelfRef.tb_mac_unit__DOT__A = 0U;
    vlSelfRef.tb_mac_unit__DOT__B = 0U;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = 0ULL;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         140);
    __Vtask_tb_mac_unit__DOT__check_result__19__exp_valid = 1U;
    __Vtask_tb_mac_unit__DOT__check_result__19__exp 
        = __Vtask_tb_mac_unit__DOT__run_single_test__17__exp;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__19__name 
        = vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__17__name;
    vlSelfRef.tb_mac_unit__DOT__total_tests = ((IData)(1U) 
                                               + vlSelfRef.tb_mac_unit__DOT__total_tests);
    if (VL_LIKELY(((((IData)(vlSelfRef.tb_mac_unit__DOT__Out) 
                     == (IData)(__Vtask_tb_mac_unit__DOT__check_result__19__exp)) 
                    & ((IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out) 
                       == (IData)(__Vtask_tb_mac_unit__DOT__check_result__19__exp_valid)))))) {
        VL_WRITEF_NX("[PASS] %s | Out=%h Valid=%b\n",3
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__19__name)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        vlSelfRef.tb_mac_unit__DOT__tests_passed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_passed);
    } else {
        VL_WRITEF_NX("[%0t] %%Error: tb_mac_unit.sv:102: Assertion failed in %m: [FAIL] %s | Expected: Out=%h Valid=%b | Got: Out=%h Valid=%b\n",8, 'M',vlSymsp->name(),"tb_mac_unit.check_result", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__19__name)
                     , '~',16,(IData)(__Vtask_tb_mac_unit__DOT__check_result__19__exp)
                     , '#',1,(IData)(__Vtask_tb_mac_unit__DOT__check_result__19__exp_valid)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        VL_STOP_MT("tb\\tb_mac_unit.sv", 102, "");
        vlSelfRef.tb_mac_unit__DOT__tests_failed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_failed);
    }
    __Vtask_tb_mac_unit__DOT__run_single_test__20__acc = 0ULL;
    __Vtask_tb_mac_unit__DOT__run_single_test__20__b_in = 0U;
    __Vtask_tb_mac_unit__DOT__run_single_test__20__a_in = 0U;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__20__name = "8. Zero Inputs"s;
    __Vtask_tb_mac_unit__DOT__run_single_test__20__exp = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__acc = __Vtask_tb_mac_unit__DOT__run_single_test__20__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__b = __Vtask_tb_mac_unit__DOT__run_single_test__20__b_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__a = __Vtask_tb_mac_unit__DOT__run_single_test__20__a_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__Vfuncout = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__prod = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__sum41 = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__G = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__L = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__St = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__inc = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__shifted_sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__21__prod = 
        VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__21__a)), 
                    VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__21__b)));
    __Vfunc_tb_mac_unit__DOT__ref_model__21__sum41 
        = (0x000001ffffffffffULL & (VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__21__acc) 
                                    + VL_EXTENDS_QI(41,32, __Vfunc_tb_mac_unit__DOT__ref_model__21__prod)));
    __Vfunc_tb_mac_unit__DOT__ref_model__21__sum = 
        ((1U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__21__sum41 
                               >> 0x27U)))) ? 0x0000007fffffffffULL
          : ((2U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__21__sum41 
                                   >> 0x27U)))) ? 0x0000008000000000ULL
              : (0x000000ffffffffffULL & __Vfunc_tb_mac_unit__DOT__ref_model__21__sum41)));
    __Vfunc_tb_mac_unit__DOT__ref_model__21__G = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__21__sum 
                                                             >> 0x0eU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__21__St = (0U 
                                                   != 
                                                   (0x00003fffU 
                                                    & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__21__sum)));
    __Vfunc_tb_mac_unit__DOT__ref_model__21__L = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__21__sum 
                                                             >> 0x0fU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__21__inc = 
        ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__21__G) 
         & ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__21__St) 
            | (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__21__L)));
    __Vfunc_tb_mac_unit__DOT__ref_model__21__shifted_sum 
        = (0x000001ffffffffffULL & (VL_SHIFTRS_QQI(41,41,32, 
                                                   (0x000001ffffffffffULL 
                                                    & VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__21__sum)), 0x0000000fU) 
                                    + (QData)((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__21__inc))));
    __Vfunc_tb_mac_unit__DOT__ref_model__21__Vfuncout 
        = (VL_LTS_IQQ(41, 0x0000000000007fffULL, __Vfunc_tb_mac_unit__DOT__ref_model__21__shifted_sum)
            ? 0x00007fffU : (VL_GTS_IQQ(41, 0x000001ffffff8000ULL, __Vfunc_tb_mac_unit__DOT__ref_model__21__shifted_sum)
                              ? 0x00008000U : (0x0000ffffU 
                                               & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__21__shifted_sum))));
    __Vtask_tb_mac_unit__DOT__run_single_test__20__exp 
        = __Vfunc_tb_mac_unit__DOT__ref_model__21__Vfuncout;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         130);
    vlSelfRef.tb_mac_unit__DOT__A = __Vtask_tb_mac_unit__DOT__run_single_test__20__a_in;
    vlSelfRef.tb_mac_unit__DOT__B = __Vtask_tb_mac_unit__DOT__run_single_test__20__b_in;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = __Vtask_tb_mac_unit__DOT__run_single_test__20__acc;
    vlSelfRef.tb_mac_unit__DOT__valid_in = 1U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         136);
    vlSelfRef.tb_mac_unit__DOT__valid_in = 0U;
    vlSelfRef.tb_mac_unit__DOT__A = 0U;
    vlSelfRef.tb_mac_unit__DOT__B = 0U;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = 0ULL;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         140);
    __Vtask_tb_mac_unit__DOT__check_result__22__exp_valid = 1U;
    __Vtask_tb_mac_unit__DOT__check_result__22__exp 
        = __Vtask_tb_mac_unit__DOT__run_single_test__20__exp;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__22__name 
        = vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__20__name;
    vlSelfRef.tb_mac_unit__DOT__total_tests = ((IData)(1U) 
                                               + vlSelfRef.tb_mac_unit__DOT__total_tests);
    if (VL_LIKELY(((((IData)(vlSelfRef.tb_mac_unit__DOT__Out) 
                     == (IData)(__Vtask_tb_mac_unit__DOT__check_result__22__exp)) 
                    & ((IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out) 
                       == (IData)(__Vtask_tb_mac_unit__DOT__check_result__22__exp_valid)))))) {
        VL_WRITEF_NX("[PASS] %s | Out=%h Valid=%b\n",3
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__22__name)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        vlSelfRef.tb_mac_unit__DOT__tests_passed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_passed);
    } else {
        VL_WRITEF_NX("[%0t] %%Error: tb_mac_unit.sv:102: Assertion failed in %m: [FAIL] %s | Expected: Out=%h Valid=%b | Got: Out=%h Valid=%b\n",8, 'M',vlSymsp->name(),"tb_mac_unit.check_result", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__22__name)
                     , '~',16,(IData)(__Vtask_tb_mac_unit__DOT__check_result__22__exp)
                     , '#',1,(IData)(__Vtask_tb_mac_unit__DOT__check_result__22__exp_valid)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        VL_STOP_MT("tb\\tb_mac_unit.sv", 102, "");
        vlSelfRef.tb_mac_unit__DOT__tests_failed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_failed);
    }
    __Vtask_tb_mac_unit__DOT__apply_reset__23__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         109);
    vlSelfRef.tb_mac_unit__DOT__rst_n = 0U;
    vlSelfRef.tb_mac_unit__DOT__valid_in = 0U;
    vlSelfRef.tb_mac_unit__DOT__A = 0U;
    vlSelfRef.tb_mac_unit__DOT__B = 0U;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = 0ULL;
    __Vtask_tb_mac_unit__DOT__apply_reset__23__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_tb_mac_unit__DOT__apply_reset__23__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                           "@(negedge tb_mac_unit.clk)");
        co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge tb_mac_unit.clk)", 
                                                             "tb\\tb_mac_unit.sv", 
                                                             115);
        __Vtask_tb_mac_unit__DOT__apply_reset__23__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_tb_mac_unit__DOT__apply_reset__23__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    vlSelfRef.tb_mac_unit__DOT__rst_n = 1U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         117);
    vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__tb_mac_unit__DOT__chk__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0U;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__a_arr[0U] = 0x4000U;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__a_arr[1U] = 0xc000U;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__a_arr[2U] = 0x2000U;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__a_arr[3U] = 0xe000U;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__b_arr[0U] = 0x4000U;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__b_arr[1U] = 0x4000U;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__b_arr[2U] = 0x2000U;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__b_arr[3U] = 0x2000U;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__acc_arr[0U] = 0ULL;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__acc_arr[1U] = 0ULL;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__acc_arr[2U] = 0ULL;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__acc_arr[3U] = 0ULL;
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__exp_arr[__Vi0] = 0;
    }
    __Vfunc_tb_mac_unit__DOT__ref_model__25__acc = vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__acc_arr[0U];
    __Vfunc_tb_mac_unit__DOT__ref_model__25__b = vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__b_arr[0U];
    __Vfunc_tb_mac_unit__DOT__ref_model__25__a = vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__a_arr[0U];
    __Vfunc_tb_mac_unit__DOT__ref_model__25__Vfuncout = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__prod = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__sum41 = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__G = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__L = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__St = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__inc = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__prod = 
        VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__a)), 
                    VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__b)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__sum41 
        = (0x000001ffffffffffULL & (VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__25__acc) 
                                    + VL_EXTENDS_QI(41,32, __Vfunc_tb_mac_unit__DOT__ref_model__25__prod)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__sum = 
        ((1U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__25__sum41 
                               >> 0x27U)))) ? 0x0000007fffffffffULL
          : ((2U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__25__sum41 
                                   >> 0x27U)))) ? 0x0000008000000000ULL
              : (0x000000ffffffffffULL & __Vfunc_tb_mac_unit__DOT__ref_model__25__sum41)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__G = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__25__sum 
                                                             >> 0x0eU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__St = (0U 
                                                   != 
                                                   (0x00003fffU 
                                                    & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__sum)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__L = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__25__sum 
                                                             >> 0x0fU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__inc = 
        ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__G) 
         & ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__St) 
            | (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__L)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum 
        = (0x000001ffffffffffULL & (VL_SHIFTRS_QQI(41,41,32, 
                                                   (0x000001ffffffffffULL 
                                                    & VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__25__sum)), 0x0000000fU) 
                                    + (QData)((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__inc))));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__Vfuncout 
        = (VL_LTS_IQQ(41, 0x0000000000007fffULL, __Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum)
            ? 0x00007fffU : (VL_GTS_IQQ(41, 0x000001ffffff8000ULL, __Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum)
                              ? 0x00008000U : (0x0000ffffU 
                                               & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum))));
    __Vtask_tb_mac_unit__DOT__test_pipeline__24____VlefCall_0__ref_model 
        = __Vfunc_tb_mac_unit__DOT__ref_model__25__Vfuncout;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__exp_arr[0U] 
        = __Vtask_tb_mac_unit__DOT__test_pipeline__24____VlefCall_0__ref_model;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__acc = vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__acc_arr[1U];
    __Vfunc_tb_mac_unit__DOT__ref_model__25__b = vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__b_arr[1U];
    __Vfunc_tb_mac_unit__DOT__ref_model__25__a = vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__a_arr[1U];
    __Vfunc_tb_mac_unit__DOT__ref_model__25__Vfuncout = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__prod = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__sum41 = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__G = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__L = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__St = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__inc = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__prod = 
        VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__a)), 
                    VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__b)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__sum41 
        = (0x000001ffffffffffULL & (VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__25__acc) 
                                    + VL_EXTENDS_QI(41,32, __Vfunc_tb_mac_unit__DOT__ref_model__25__prod)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__sum = 
        ((1U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__25__sum41 
                               >> 0x27U)))) ? 0x0000007fffffffffULL
          : ((2U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__25__sum41 
                                   >> 0x27U)))) ? 0x0000008000000000ULL
              : (0x000000ffffffffffULL & __Vfunc_tb_mac_unit__DOT__ref_model__25__sum41)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__G = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__25__sum 
                                                             >> 0x0eU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__St = (0U 
                                                   != 
                                                   (0x00003fffU 
                                                    & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__sum)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__L = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__25__sum 
                                                             >> 0x0fU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__inc = 
        ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__G) 
         & ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__St) 
            | (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__L)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum 
        = (0x000001ffffffffffULL & (VL_SHIFTRS_QQI(41,41,32, 
                                                   (0x000001ffffffffffULL 
                                                    & VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__25__sum)), 0x0000000fU) 
                                    + (QData)((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__inc))));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__Vfuncout 
        = (VL_LTS_IQQ(41, 0x0000000000007fffULL, __Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum)
            ? 0x00007fffU : (VL_GTS_IQQ(41, 0x000001ffffff8000ULL, __Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum)
                              ? 0x00008000U : (0x0000ffffU 
                                               & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum))));
    __Vtask_tb_mac_unit__DOT__test_pipeline__24____VlefCall_0__ref_model 
        = __Vfunc_tb_mac_unit__DOT__ref_model__25__Vfuncout;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__exp_arr[1U] 
        = __Vtask_tb_mac_unit__DOT__test_pipeline__24____VlefCall_0__ref_model;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__acc = vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__acc_arr[2U];
    __Vfunc_tb_mac_unit__DOT__ref_model__25__b = vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__b_arr[2U];
    __Vfunc_tb_mac_unit__DOT__ref_model__25__a = vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__a_arr[2U];
    __Vfunc_tb_mac_unit__DOT__ref_model__25__Vfuncout = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__prod = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__sum41 = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__G = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__L = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__St = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__inc = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__prod = 
        VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__a)), 
                    VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__b)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__sum41 
        = (0x000001ffffffffffULL & (VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__25__acc) 
                                    + VL_EXTENDS_QI(41,32, __Vfunc_tb_mac_unit__DOT__ref_model__25__prod)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__sum = 
        ((1U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__25__sum41 
                               >> 0x27U)))) ? 0x0000007fffffffffULL
          : ((2U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__25__sum41 
                                   >> 0x27U)))) ? 0x0000008000000000ULL
              : (0x000000ffffffffffULL & __Vfunc_tb_mac_unit__DOT__ref_model__25__sum41)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__G = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__25__sum 
                                                             >> 0x0eU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__St = (0U 
                                                   != 
                                                   (0x00003fffU 
                                                    & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__sum)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__L = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__25__sum 
                                                             >> 0x0fU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__inc = 
        ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__G) 
         & ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__St) 
            | (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__L)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum 
        = (0x000001ffffffffffULL & (VL_SHIFTRS_QQI(41,41,32, 
                                                   (0x000001ffffffffffULL 
                                                    & VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__25__sum)), 0x0000000fU) 
                                    + (QData)((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__inc))));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__Vfuncout 
        = (VL_LTS_IQQ(41, 0x0000000000007fffULL, __Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum)
            ? 0x00007fffU : (VL_GTS_IQQ(41, 0x000001ffffff8000ULL, __Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum)
                              ? 0x00008000U : (0x0000ffffU 
                                               & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum))));
    __Vtask_tb_mac_unit__DOT__test_pipeline__24____VlefCall_0__ref_model 
        = __Vfunc_tb_mac_unit__DOT__ref_model__25__Vfuncout;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__exp_arr[2U] 
        = __Vtask_tb_mac_unit__DOT__test_pipeline__24____VlefCall_0__ref_model;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__acc = vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__acc_arr[3U];
    __Vfunc_tb_mac_unit__DOT__ref_model__25__b = vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__b_arr[3U];
    __Vfunc_tb_mac_unit__DOT__ref_model__25__a = vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__a_arr[3U];
    __Vfunc_tb_mac_unit__DOT__ref_model__25__Vfuncout = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__prod = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__sum41 = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__G = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__L = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__St = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__inc = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__25__prod = 
        VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__a)), 
                    VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__b)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__sum41 
        = (0x000001ffffffffffULL & (VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__25__acc) 
                                    + VL_EXTENDS_QI(41,32, __Vfunc_tb_mac_unit__DOT__ref_model__25__prod)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__sum = 
        ((1U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__25__sum41 
                               >> 0x27U)))) ? 0x0000007fffffffffULL
          : ((2U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__25__sum41 
                                   >> 0x27U)))) ? 0x0000008000000000ULL
              : (0x000000ffffffffffULL & __Vfunc_tb_mac_unit__DOT__ref_model__25__sum41)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__G = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__25__sum 
                                                             >> 0x0eU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__St = (0U 
                                                   != 
                                                   (0x00003fffU 
                                                    & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__sum)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__L = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__25__sum 
                                                             >> 0x0fU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__inc = 
        ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__G) 
         & ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__St) 
            | (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__L)));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum 
        = (0x000001ffffffffffULL & (VL_SHIFTRS_QQI(41,41,32, 
                                                   (0x000001ffffffffffULL 
                                                    & VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__25__sum)), 0x0000000fU) 
                                    + (QData)((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__inc))));
    __Vfunc_tb_mac_unit__DOT__ref_model__25__Vfuncout 
        = (VL_LTS_IQQ(41, 0x0000000000007fffULL, __Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum)
            ? 0x00007fffU : (VL_GTS_IQQ(41, 0x000001ffffff8000ULL, __Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum)
                              ? 0x00008000U : (0x0000ffffU 
                                               & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__25__shifted_sum))));
    __Vtask_tb_mac_unit__DOT__test_pipeline__24____VlefCall_0__ref_model 
        = __Vfunc_tb_mac_unit__DOT__ref_model__25__Vfuncout;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__exp_arr[3U] 
        = __Vtask_tb_mac_unit__DOT__test_pipeline__24____VlefCall_0__ref_model;
    {
        VlForkSync __Vfork_1__sync;
        __Vfork_1__sync.init(2U, nullptr);
        Vtb_mac_unit___024root___eval_initial__TOP__Vtiming__1____Vfork_1__0(vlSelf, __Vfork_1__sync);
        Vtb_mac_unit___024root___eval_initial__TOP__Vtiming__1____Vfork_1__1(vlSelf, __Vfork_1__sync);
        co_await __Vfork_1__sync.join(nullptr, "tb\\tb_mac_unit.sv", 
                                      154);
    }
    __Vtask_tb_mac_unit__DOT__test_valid_gating__27__exp_prev = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__acc = 0ULL;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__b = 0x4000U;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__a = 0x4000U;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__Vfuncout = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__prod = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__sum41 = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__G = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__L = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__St = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__inc = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__shifted_sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__28__prod = 
        VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__28__a)), 
                    VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__28__b)));
    __Vfunc_tb_mac_unit__DOT__ref_model__28__sum41 
        = (0x000001ffffffffffULL & (VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__28__acc) 
                                    + VL_EXTENDS_QI(41,32, __Vfunc_tb_mac_unit__DOT__ref_model__28__prod)));
    __Vfunc_tb_mac_unit__DOT__ref_model__28__sum = 
        ((1U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__28__sum41 
                               >> 0x27U)))) ? 0x0000007fffffffffULL
          : ((2U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__28__sum41 
                                   >> 0x27U)))) ? 0x0000008000000000ULL
              : (0x000000ffffffffffULL & __Vfunc_tb_mac_unit__DOT__ref_model__28__sum41)));
    __Vfunc_tb_mac_unit__DOT__ref_model__28__G = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__28__sum 
                                                             >> 0x0eU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__28__St = (0U 
                                                   != 
                                                   (0x00003fffU 
                                                    & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__28__sum)));
    __Vfunc_tb_mac_unit__DOT__ref_model__28__L = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__28__sum 
                                                             >> 0x0fU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__28__inc = 
        ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__28__G) 
         & ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__28__St) 
            | (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__28__L)));
    __Vfunc_tb_mac_unit__DOT__ref_model__28__shifted_sum 
        = (0x000001ffffffffffULL & (VL_SHIFTRS_QQI(41,41,32, 
                                                   (0x000001ffffffffffULL 
                                                    & VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__28__sum)), 0x0000000fU) 
                                    + (QData)((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__28__inc))));
    __Vfunc_tb_mac_unit__DOT__ref_model__28__Vfuncout 
        = (VL_LTS_IQQ(41, 0x0000000000007fffULL, __Vfunc_tb_mac_unit__DOT__ref_model__28__shifted_sum)
            ? 0x00007fffU : (VL_GTS_IQQ(41, 0x000001ffffff8000ULL, __Vfunc_tb_mac_unit__DOT__ref_model__28__shifted_sum)
                              ? 0x00008000U : (0x0000ffffU 
                                               & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__28__shifted_sum))));
    __Vtask_tb_mac_unit__DOT__test_valid_gating__27__exp_prev 
        = __Vfunc_tb_mac_unit__DOT__ref_model__28__Vfuncout;
    __Vtask_tb_mac_unit__DOT__run_single_test__29__acc = 0ULL;
    __Vtask_tb_mac_unit__DOT__run_single_test__29__b_in = 0x4000U;
    __Vtask_tb_mac_unit__DOT__run_single_test__29__a_in = 0x4000U;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__29__name = "10. Valid Gating - Setup"s;
    __Vtask_tb_mac_unit__DOT__run_single_test__29__exp = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__acc = __Vtask_tb_mac_unit__DOT__run_single_test__29__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__b = __Vtask_tb_mac_unit__DOT__run_single_test__29__b_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__a = __Vtask_tb_mac_unit__DOT__run_single_test__29__a_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__Vfuncout = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__prod = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__sum41 = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__G = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__L = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__St = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__inc = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__shifted_sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__30__prod = 
        VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__30__a)), 
                    VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__30__b)));
    __Vfunc_tb_mac_unit__DOT__ref_model__30__sum41 
        = (0x000001ffffffffffULL & (VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__30__acc) 
                                    + VL_EXTENDS_QI(41,32, __Vfunc_tb_mac_unit__DOT__ref_model__30__prod)));
    __Vfunc_tb_mac_unit__DOT__ref_model__30__sum = 
        ((1U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__30__sum41 
                               >> 0x27U)))) ? 0x0000007fffffffffULL
          : ((2U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__30__sum41 
                                   >> 0x27U)))) ? 0x0000008000000000ULL
              : (0x000000ffffffffffULL & __Vfunc_tb_mac_unit__DOT__ref_model__30__sum41)));
    __Vfunc_tb_mac_unit__DOT__ref_model__30__G = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__30__sum 
                                                             >> 0x0eU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__30__St = (0U 
                                                   != 
                                                   (0x00003fffU 
                                                    & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__30__sum)));
    __Vfunc_tb_mac_unit__DOT__ref_model__30__L = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__30__sum 
                                                             >> 0x0fU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__30__inc = 
        ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__30__G) 
         & ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__30__St) 
            | (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__30__L)));
    __Vfunc_tb_mac_unit__DOT__ref_model__30__shifted_sum 
        = (0x000001ffffffffffULL & (VL_SHIFTRS_QQI(41,41,32, 
                                                   (0x000001ffffffffffULL 
                                                    & VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__30__sum)), 0x0000000fU) 
                                    + (QData)((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__30__inc))));
    __Vfunc_tb_mac_unit__DOT__ref_model__30__Vfuncout 
        = (VL_LTS_IQQ(41, 0x0000000000007fffULL, __Vfunc_tb_mac_unit__DOT__ref_model__30__shifted_sum)
            ? 0x00007fffU : (VL_GTS_IQQ(41, 0x000001ffffff8000ULL, __Vfunc_tb_mac_unit__DOT__ref_model__30__shifted_sum)
                              ? 0x00008000U : (0x0000ffffU 
                                               & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__30__shifted_sum))));
    __Vtask_tb_mac_unit__DOT__run_single_test__29__exp 
        = __Vfunc_tb_mac_unit__DOT__ref_model__30__Vfuncout;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         130);
    vlSelfRef.tb_mac_unit__DOT__A = __Vtask_tb_mac_unit__DOT__run_single_test__29__a_in;
    vlSelfRef.tb_mac_unit__DOT__B = __Vtask_tb_mac_unit__DOT__run_single_test__29__b_in;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = __Vtask_tb_mac_unit__DOT__run_single_test__29__acc;
    vlSelfRef.tb_mac_unit__DOT__valid_in = 1U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         136);
    vlSelfRef.tb_mac_unit__DOT__valid_in = 0U;
    vlSelfRef.tb_mac_unit__DOT__A = 0U;
    vlSelfRef.tb_mac_unit__DOT__B = 0U;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = 0ULL;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         140);
    __Vtask_tb_mac_unit__DOT__check_result__31__exp_valid = 1U;
    __Vtask_tb_mac_unit__DOT__check_result__31__exp 
        = __Vtask_tb_mac_unit__DOT__run_single_test__29__exp;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__31__name 
        = vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__29__name;
    vlSelfRef.tb_mac_unit__DOT__total_tests = ((IData)(1U) 
                                               + vlSelfRef.tb_mac_unit__DOT__total_tests);
    if (VL_LIKELY(((((IData)(vlSelfRef.tb_mac_unit__DOT__Out) 
                     == (IData)(__Vtask_tb_mac_unit__DOT__check_result__31__exp)) 
                    & ((IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out) 
                       == (IData)(__Vtask_tb_mac_unit__DOT__check_result__31__exp_valid)))))) {
        VL_WRITEF_NX("[PASS] %s | Out=%h Valid=%b\n",3
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__31__name)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        vlSelfRef.tb_mac_unit__DOT__tests_passed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_passed);
    } else {
        VL_WRITEF_NX("[%0t] %%Error: tb_mac_unit.sv:102: Assertion failed in %m: [FAIL] %s | Expected: Out=%h Valid=%b | Got: Out=%h Valid=%b\n",8, 'M',vlSymsp->name(),"tb_mac_unit.check_result", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__31__name)
                     , '~',16,(IData)(__Vtask_tb_mac_unit__DOT__check_result__31__exp)
                     , '#',1,(IData)(__Vtask_tb_mac_unit__DOT__check_result__31__exp_valid)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        VL_STOP_MT("tb\\tb_mac_unit.sv", 102, "");
        vlSelfRef.tb_mac_unit__DOT__tests_failed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_failed);
    }
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         184);
    vlSelfRef.tb_mac_unit__DOT__A = 0x7fffU;
    vlSelfRef.tb_mac_unit__DOT__B = 0x7fffU;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = 0x0000003fffffffffULL;
    vlSelfRef.tb_mac_unit__DOT__valid_in = 0U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         188);
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         189);
    vlSelfRef.tb_mac_unit__DOT__total_tests = ((IData)(1U) 
                                               + vlSelfRef.tb_mac_unit__DOT__total_tests);
    if (VL_LIKELY(((((IData)(vlSelfRef.tb_mac_unit__DOT__Out) 
                     == (IData)(__Vtask_tb_mac_unit__DOT__test_valid_gating__27__exp_prev)) 
                    & (~ (IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out)))))) {
        VL_WRITEF_NX("[PASS] 10. Valid Gating | Out held=%h\n",1
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out));
        vlSelfRef.tb_mac_unit__DOT__tests_passed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_passed);
    } else {
        VL_WRITEF_NX("[%0t] %%Error: tb_mac_unit.sv:196: Assertion failed in %m: [FAIL] 10. Valid Gating | Expected Out=%h Valid=0 | Got Out=%h Valid=%b\n",6, 'M',vlSymsp->name(),"tb_mac_unit.test_valid_gating", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '~',16,(IData)(__Vtask_tb_mac_unit__DOT__test_valid_gating__27__exp_prev)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        VL_STOP_MT("tb\\tb_mac_unit.sv", 196, "");
        vlSelfRef.tb_mac_unit__DOT__tests_failed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_failed);
    }
    __Vtask_tb_mac_unit__DOT__apply_reset__32__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         109);
    vlSelfRef.tb_mac_unit__DOT__rst_n = 0U;
    vlSelfRef.tb_mac_unit__DOT__valid_in = 0U;
    vlSelfRef.tb_mac_unit__DOT__A = 0U;
    vlSelfRef.tb_mac_unit__DOT__B = 0U;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = 0ULL;
    __Vtask_tb_mac_unit__DOT__apply_reset__32__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_tb_mac_unit__DOT__apply_reset__32__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                           "@(negedge tb_mac_unit.clk)");
        co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge tb_mac_unit.clk)", 
                                                             "tb\\tb_mac_unit.sv", 
                                                             115);
        __Vtask_tb_mac_unit__DOT__apply_reset__32__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_tb_mac_unit__DOT__apply_reset__32__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    vlSelfRef.tb_mac_unit__DOT__rst_n = 1U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         117);
    __Vtask_tb_mac_unit__DOT__run_single_test__33__acc = 0x0000000000014000ULL;
    __Vtask_tb_mac_unit__DOT__run_single_test__33__b_in = 0U;
    __Vtask_tb_mac_unit__DOT__run_single_test__33__a_in = 0U;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__33__name = "11.1 RNE: G=1, St=0, L=0 (Round even down)"s;
    __Vtask_tb_mac_unit__DOT__run_single_test__33__exp = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__acc = __Vtask_tb_mac_unit__DOT__run_single_test__33__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__b = __Vtask_tb_mac_unit__DOT__run_single_test__33__b_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__a = __Vtask_tb_mac_unit__DOT__run_single_test__33__a_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__Vfuncout = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__prod = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__sum41 = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__G = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__L = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__St = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__inc = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__shifted_sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__34__prod = 
        VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__34__a)), 
                    VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__34__b)));
    __Vfunc_tb_mac_unit__DOT__ref_model__34__sum41 
        = (0x000001ffffffffffULL & (VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__34__acc) 
                                    + VL_EXTENDS_QI(41,32, __Vfunc_tb_mac_unit__DOT__ref_model__34__prod)));
    __Vfunc_tb_mac_unit__DOT__ref_model__34__sum = 
        ((1U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__34__sum41 
                               >> 0x27U)))) ? 0x0000007fffffffffULL
          : ((2U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__34__sum41 
                                   >> 0x27U)))) ? 0x0000008000000000ULL
              : (0x000000ffffffffffULL & __Vfunc_tb_mac_unit__DOT__ref_model__34__sum41)));
    __Vfunc_tb_mac_unit__DOT__ref_model__34__G = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__34__sum 
                                                             >> 0x0eU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__34__St = (0U 
                                                   != 
                                                   (0x00003fffU 
                                                    & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__34__sum)));
    __Vfunc_tb_mac_unit__DOT__ref_model__34__L = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__34__sum 
                                                             >> 0x0fU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__34__inc = 
        ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__34__G) 
         & ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__34__St) 
            | (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__34__L)));
    __Vfunc_tb_mac_unit__DOT__ref_model__34__shifted_sum 
        = (0x000001ffffffffffULL & (VL_SHIFTRS_QQI(41,41,32, 
                                                   (0x000001ffffffffffULL 
                                                    & VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__34__sum)), 0x0000000fU) 
                                    + (QData)((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__34__inc))));
    __Vfunc_tb_mac_unit__DOT__ref_model__34__Vfuncout 
        = (VL_LTS_IQQ(41, 0x0000000000007fffULL, __Vfunc_tb_mac_unit__DOT__ref_model__34__shifted_sum)
            ? 0x00007fffU : (VL_GTS_IQQ(41, 0x000001ffffff8000ULL, __Vfunc_tb_mac_unit__DOT__ref_model__34__shifted_sum)
                              ? 0x00008000U : (0x0000ffffU 
                                               & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__34__shifted_sum))));
    __Vtask_tb_mac_unit__DOT__run_single_test__33__exp 
        = __Vfunc_tb_mac_unit__DOT__ref_model__34__Vfuncout;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         130);
    vlSelfRef.tb_mac_unit__DOT__A = __Vtask_tb_mac_unit__DOT__run_single_test__33__a_in;
    vlSelfRef.tb_mac_unit__DOT__B = __Vtask_tb_mac_unit__DOT__run_single_test__33__b_in;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = __Vtask_tb_mac_unit__DOT__run_single_test__33__acc;
    vlSelfRef.tb_mac_unit__DOT__valid_in = 1U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         136);
    vlSelfRef.tb_mac_unit__DOT__valid_in = 0U;
    vlSelfRef.tb_mac_unit__DOT__A = 0U;
    vlSelfRef.tb_mac_unit__DOT__B = 0U;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = 0ULL;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         140);
    __Vtask_tb_mac_unit__DOT__check_result__35__exp_valid = 1U;
    __Vtask_tb_mac_unit__DOT__check_result__35__exp 
        = __Vtask_tb_mac_unit__DOT__run_single_test__33__exp;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__35__name 
        = vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__33__name;
    vlSelfRef.tb_mac_unit__DOT__total_tests = ((IData)(1U) 
                                               + vlSelfRef.tb_mac_unit__DOT__total_tests);
    if (VL_LIKELY(((((IData)(vlSelfRef.tb_mac_unit__DOT__Out) 
                     == (IData)(__Vtask_tb_mac_unit__DOT__check_result__35__exp)) 
                    & ((IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out) 
                       == (IData)(__Vtask_tb_mac_unit__DOT__check_result__35__exp_valid)))))) {
        VL_WRITEF_NX("[PASS] %s | Out=%h Valid=%b\n",3
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__35__name)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        vlSelfRef.tb_mac_unit__DOT__tests_passed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_passed);
    } else {
        VL_WRITEF_NX("[%0t] %%Error: tb_mac_unit.sv:102: Assertion failed in %m: [FAIL] %s | Expected: Out=%h Valid=%b | Got: Out=%h Valid=%b\n",8, 'M',vlSymsp->name(),"tb_mac_unit.check_result", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__35__name)
                     , '~',16,(IData)(__Vtask_tb_mac_unit__DOT__check_result__35__exp)
                     , '#',1,(IData)(__Vtask_tb_mac_unit__DOT__check_result__35__exp_valid)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        VL_STOP_MT("tb\\tb_mac_unit.sv", 102, "");
        vlSelfRef.tb_mac_unit__DOT__tests_failed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_failed);
    }
    __Vtask_tb_mac_unit__DOT__run_single_test__36__acc = 0x000000000001c000ULL;
    __Vtask_tb_mac_unit__DOT__run_single_test__36__b_in = 0U;
    __Vtask_tb_mac_unit__DOT__run_single_test__36__a_in = 0U;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__36__name = "11.2 RNE: G=1, St=0, L=1 (Round even up)"s;
    __Vtask_tb_mac_unit__DOT__run_single_test__36__exp = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__acc = __Vtask_tb_mac_unit__DOT__run_single_test__36__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__b = __Vtask_tb_mac_unit__DOT__run_single_test__36__b_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__a = __Vtask_tb_mac_unit__DOT__run_single_test__36__a_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__Vfuncout = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__prod = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__sum41 = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__G = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__L = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__St = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__inc = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__shifted_sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__37__prod = 
        VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__37__a)), 
                    VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__37__b)));
    __Vfunc_tb_mac_unit__DOT__ref_model__37__sum41 
        = (0x000001ffffffffffULL & (VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__37__acc) 
                                    + VL_EXTENDS_QI(41,32, __Vfunc_tb_mac_unit__DOT__ref_model__37__prod)));
    __Vfunc_tb_mac_unit__DOT__ref_model__37__sum = 
        ((1U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__37__sum41 
                               >> 0x27U)))) ? 0x0000007fffffffffULL
          : ((2U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__37__sum41 
                                   >> 0x27U)))) ? 0x0000008000000000ULL
              : (0x000000ffffffffffULL & __Vfunc_tb_mac_unit__DOT__ref_model__37__sum41)));
    __Vfunc_tb_mac_unit__DOT__ref_model__37__G = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__37__sum 
                                                             >> 0x0eU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__37__St = (0U 
                                                   != 
                                                   (0x00003fffU 
                                                    & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__37__sum)));
    __Vfunc_tb_mac_unit__DOT__ref_model__37__L = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__37__sum 
                                                             >> 0x0fU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__37__inc = 
        ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__37__G) 
         & ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__37__St) 
            | (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__37__L)));
    __Vfunc_tb_mac_unit__DOT__ref_model__37__shifted_sum 
        = (0x000001ffffffffffULL & (VL_SHIFTRS_QQI(41,41,32, 
                                                   (0x000001ffffffffffULL 
                                                    & VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__37__sum)), 0x0000000fU) 
                                    + (QData)((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__37__inc))));
    __Vfunc_tb_mac_unit__DOT__ref_model__37__Vfuncout 
        = (VL_LTS_IQQ(41, 0x0000000000007fffULL, __Vfunc_tb_mac_unit__DOT__ref_model__37__shifted_sum)
            ? 0x00007fffU : (VL_GTS_IQQ(41, 0x000001ffffff8000ULL, __Vfunc_tb_mac_unit__DOT__ref_model__37__shifted_sum)
                              ? 0x00008000U : (0x0000ffffU 
                                               & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__37__shifted_sum))));
    __Vtask_tb_mac_unit__DOT__run_single_test__36__exp 
        = __Vfunc_tb_mac_unit__DOT__ref_model__37__Vfuncout;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         130);
    vlSelfRef.tb_mac_unit__DOT__A = __Vtask_tb_mac_unit__DOT__run_single_test__36__a_in;
    vlSelfRef.tb_mac_unit__DOT__B = __Vtask_tb_mac_unit__DOT__run_single_test__36__b_in;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = __Vtask_tb_mac_unit__DOT__run_single_test__36__acc;
    vlSelfRef.tb_mac_unit__DOT__valid_in = 1U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         136);
    vlSelfRef.tb_mac_unit__DOT__valid_in = 0U;
    vlSelfRef.tb_mac_unit__DOT__A = 0U;
    vlSelfRef.tb_mac_unit__DOT__B = 0U;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = 0ULL;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         140);
    __Vtask_tb_mac_unit__DOT__check_result__38__exp_valid = 1U;
    __Vtask_tb_mac_unit__DOT__check_result__38__exp 
        = __Vtask_tb_mac_unit__DOT__run_single_test__36__exp;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__38__name 
        = vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__36__name;
    vlSelfRef.tb_mac_unit__DOT__total_tests = ((IData)(1U) 
                                               + vlSelfRef.tb_mac_unit__DOT__total_tests);
    if (VL_LIKELY(((((IData)(vlSelfRef.tb_mac_unit__DOT__Out) 
                     == (IData)(__Vtask_tb_mac_unit__DOT__check_result__38__exp)) 
                    & ((IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out) 
                       == (IData)(__Vtask_tb_mac_unit__DOT__check_result__38__exp_valid)))))) {
        VL_WRITEF_NX("[PASS] %s | Out=%h Valid=%b\n",3
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__38__name)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        vlSelfRef.tb_mac_unit__DOT__tests_passed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_passed);
    } else {
        VL_WRITEF_NX("[%0t] %%Error: tb_mac_unit.sv:102: Assertion failed in %m: [FAIL] %s | Expected: Out=%h Valid=%b | Got: Out=%h Valid=%b\n",8, 'M',vlSymsp->name(),"tb_mac_unit.check_result", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__38__name)
                     , '~',16,(IData)(__Vtask_tb_mac_unit__DOT__check_result__38__exp)
                     , '#',1,(IData)(__Vtask_tb_mac_unit__DOT__check_result__38__exp_valid)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        VL_STOP_MT("tb\\tb_mac_unit.sv", 102, "");
        vlSelfRef.tb_mac_unit__DOT__tests_failed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_failed);
    }
    __Vtask_tb_mac_unit__DOT__run_single_test__39__acc = 0x0000000000014001ULL;
    __Vtask_tb_mac_unit__DOT__run_single_test__39__b_in = 0U;
    __Vtask_tb_mac_unit__DOT__run_single_test__39__a_in = 0U;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__39__name = "11.3 RNE: G=1, St=1, L=0 (Round up)"s;
    __Vtask_tb_mac_unit__DOT__run_single_test__39__exp = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__acc = __Vtask_tb_mac_unit__DOT__run_single_test__39__acc;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__b = __Vtask_tb_mac_unit__DOT__run_single_test__39__b_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__a = __Vtask_tb_mac_unit__DOT__run_single_test__39__a_in;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__Vfuncout = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__prod = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__sum41 = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__G = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__L = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__St = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__inc = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__shifted_sum = 0;
    __Vfunc_tb_mac_unit__DOT__ref_model__40__prod = 
        VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__40__a)), 
                    VL_EXTENDS_II(32,16, (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__40__b)));
    __Vfunc_tb_mac_unit__DOT__ref_model__40__sum41 
        = (0x000001ffffffffffULL & (VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__40__acc) 
                                    + VL_EXTENDS_QI(41,32, __Vfunc_tb_mac_unit__DOT__ref_model__40__prod)));
    __Vfunc_tb_mac_unit__DOT__ref_model__40__sum = 
        ((1U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__40__sum41 
                               >> 0x27U)))) ? 0x0000007fffffffffULL
          : ((2U == (3U & (IData)((__Vfunc_tb_mac_unit__DOT__ref_model__40__sum41 
                                   >> 0x27U)))) ? 0x0000008000000000ULL
              : (0x000000ffffffffffULL & __Vfunc_tb_mac_unit__DOT__ref_model__40__sum41)));
    __Vfunc_tb_mac_unit__DOT__ref_model__40__G = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__40__sum 
                                                             >> 0x0eU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__40__St = (0U 
                                                   != 
                                                   (0x00003fffU 
                                                    & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__40__sum)));
    __Vfunc_tb_mac_unit__DOT__ref_model__40__L = (1U 
                                                  & (IData)(
                                                            (__Vfunc_tb_mac_unit__DOT__ref_model__40__sum 
                                                             >> 0x0fU)));
    __Vfunc_tb_mac_unit__DOT__ref_model__40__inc = 
        ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__40__G) 
         & ((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__40__St) 
            | (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__40__L)));
    __Vfunc_tb_mac_unit__DOT__ref_model__40__shifted_sum 
        = (0x000001ffffffffffULL & (VL_SHIFTRS_QQI(41,41,32, 
                                                   (0x000001ffffffffffULL 
                                                    & VL_EXTENDS_QQ(41,40, __Vfunc_tb_mac_unit__DOT__ref_model__40__sum)), 0x0000000fU) 
                                    + (QData)((IData)(__Vfunc_tb_mac_unit__DOT__ref_model__40__inc))));
    __Vfunc_tb_mac_unit__DOT__ref_model__40__Vfuncout 
        = (VL_LTS_IQQ(41, 0x0000000000007fffULL, __Vfunc_tb_mac_unit__DOT__ref_model__40__shifted_sum)
            ? 0x00007fffU : (VL_GTS_IQQ(41, 0x000001ffffff8000ULL, __Vfunc_tb_mac_unit__DOT__ref_model__40__shifted_sum)
                              ? 0x00008000U : (0x0000ffffU 
                                               & (IData)(__Vfunc_tb_mac_unit__DOT__ref_model__40__shifted_sum))));
    __Vtask_tb_mac_unit__DOT__run_single_test__39__exp 
        = __Vfunc_tb_mac_unit__DOT__ref_model__40__Vfuncout;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         130);
    vlSelfRef.tb_mac_unit__DOT__A = __Vtask_tb_mac_unit__DOT__run_single_test__39__a_in;
    vlSelfRef.tb_mac_unit__DOT__B = __Vtask_tb_mac_unit__DOT__run_single_test__39__b_in;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = __Vtask_tb_mac_unit__DOT__run_single_test__39__acc;
    vlSelfRef.tb_mac_unit__DOT__valid_in = 1U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         136);
    vlSelfRef.tb_mac_unit__DOT__valid_in = 0U;
    vlSelfRef.tb_mac_unit__DOT__A = 0U;
    vlSelfRef.tb_mac_unit__DOT__B = 0U;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = 0ULL;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         140);
    __Vtask_tb_mac_unit__DOT__check_result__41__exp_valid = 1U;
    __Vtask_tb_mac_unit__DOT__check_result__41__exp 
        = __Vtask_tb_mac_unit__DOT__run_single_test__39__exp;
    vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__41__name 
        = vlSelfRef.__Vtask_tb_mac_unit__DOT__run_single_test__39__name;
    vlSelfRef.tb_mac_unit__DOT__total_tests = ((IData)(1U) 
                                               + vlSelfRef.tb_mac_unit__DOT__total_tests);
    if (VL_LIKELY(((((IData)(vlSelfRef.tb_mac_unit__DOT__Out) 
                     == (IData)(__Vtask_tb_mac_unit__DOT__check_result__41__exp)) 
                    & ((IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out) 
                       == (IData)(__Vtask_tb_mac_unit__DOT__check_result__41__exp_valid)))))) {
        VL_WRITEF_NX("[PASS] %s | Out=%h Valid=%b\n",3
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__41__name)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        vlSelfRef.tb_mac_unit__DOT__tests_passed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_passed);
    } else {
        VL_WRITEF_NX("[%0t] %%Error: tb_mac_unit.sv:102: Assertion failed in %m: [FAIL] %s | Expected: Out=%h Valid=%b | Got: Out=%h Valid=%b\n",8, 'M',vlSymsp->name(),"tb_mac_unit.check_result", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__41__name)
                     , '~',16,(IData)(__Vtask_tb_mac_unit__DOT__check_result__41__exp)
                     , '#',1,(IData)(__Vtask_tb_mac_unit__DOT__check_result__41__exp_valid)
                     , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                     , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
        VL_STOP_MT("tb\\tb_mac_unit.sv", 102, "");
        vlSelfRef.tb_mac_unit__DOT__tests_failed = 
            ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_failed);
    }
    __Vtask_tb_mac_unit__DOT__apply_reset__42__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         109);
    vlSelfRef.tb_mac_unit__DOT__rst_n = 0U;
    vlSelfRef.tb_mac_unit__DOT__valid_in = 0U;
    vlSelfRef.tb_mac_unit__DOT__A = 0U;
    vlSelfRef.tb_mac_unit__DOT__B = 0U;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = 0ULL;
    __Vtask_tb_mac_unit__DOT__apply_reset__42__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 = 2U;
    while (VL_LTS_III(32, 0U, __Vtask_tb_mac_unit__DOT__apply_reset__42__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                           "@(negedge tb_mac_unit.clk)");
        co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge tb_mac_unit.clk)", 
                                                             "tb\\tb_mac_unit.sv", 
                                                             115);
        __Vtask_tb_mac_unit__DOT__apply_reset__42__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_tb_mac_unit__DOT__apply_reset__42__tb_mac_unit__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    vlSelfRef.tb_mac_unit__DOT__rst_n = 1U;
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         117);
    for (int __Vi0 = 0; __Vi0 < 15; ++__Vi0) {
        VL_ZERO_W(88, __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__vec_mem[__Vi0]);
    }
    __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__exp = 0;
    VL_READMEM_N(true, 88, 15, 0, "test_vectors.hex"s
                 ,  &(__Vtask_tb_mac_unit__DOT__test_golden_vectors__43__vec_mem)
                 , 0, ~0ULL);
    __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n = 0U;
    __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n = 0U;
    while (VL_GTS_III(32, 0x0000000fU, __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n)) {
        if ((0x0eU >= (0x0000000fU & __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n))) {
            __Vtemp_1[0U] = __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__vec_mem
                [(0x0000000fU & __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n)][0U];
            __Vtemp_1[1U] = __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__vec_mem
                [(0x0000000fU & __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n)][1U];
            __Vtemp_1[2U] = __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__vec_mem
                [(0x0000000fU & __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n)][2U];
        } else {
            __Vtemp_1[0U] = 0U;
        }
        __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__exp 
            = (0x0000ffffU & __Vtemp_1[0U]);
        Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                           "@(negedge tb_mac_unit.clk)");
        co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge tb_mac_unit.clk)", 
                                                             "tb\\tb_mac_unit.sv", 
                                                             211);
        if ((0x0eU >= (0x0000000fU & __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n))) {
            __Vtemp_2[0U] = __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__vec_mem
                [(0x0000000fU & __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n)][0U];
            __Vtemp_2[1U] = __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__vec_mem
                [(0x0000000fU & __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n)][1U];
            __Vtemp_2[2U] = __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__vec_mem
                [(0x0000000fU & __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n)][2U];
            __Vtemp_3[0U] = __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__vec_mem
                [(0x0000000fU & __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n)][0U];
            __Vtemp_3[1U] = __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__vec_mem
                [(0x0000000fU & __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n)][1U];
            __Vtemp_3[2U] = __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__vec_mem
                [(0x0000000fU & __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n)][2U];
            __Vtemp_4[0U] = __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__vec_mem
                [(0x0000000fU & __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n)][0U];
            __Vtemp_4[1U] = __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__vec_mem
                [(0x0000000fU & __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n)][1U];
            __Vtemp_4[2U] = __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__vec_mem
                [(0x0000000fU & __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n)][2U];
        } else {
            __Vtemp_2[2U] = 0U;
            __Vtemp_3[1U] = 0U;
            __Vtemp_3[2U] = 0U;
            __Vtemp_4[0U] = 0U;
            __Vtemp_4[1U] = 0U;
        }
        vlSelfRef.tb_mac_unit__DOT__A = (0x0000ffffU 
                                         & (__Vtemp_2[2U] 
                                            >> 8U));
        vlSelfRef.tb_mac_unit__DOT__B = (0x0000ffffU 
                                         & ((__Vtemp_3[2U] 
                                             << 8U) 
                                            | (__Vtemp_3[1U] 
                                               >> 0x00000018U)));
        vlSelfRef.tb_mac_unit__DOT__Acc_in = (0x000000ffffffffffULL 
                                              & (((QData)((IData)(__Vtemp_4[1U])) 
                                                  << 0x00000010U) 
                                                 | ((QData)((IData)(__Vtemp_4[0U])) 
                                                    >> 0x00000010U)));
        vlSelfRef.tb_mac_unit__DOT__valid_in = 1U;
        Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                           "@(negedge tb_mac_unit.clk)");
        co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge tb_mac_unit.clk)", 
                                                             "tb\\tb_mac_unit.sv", 
                                                             217);
        vlSelfRef.tb_mac_unit__DOT__valid_in = 0U;
        vlSelfRef.tb_mac_unit__DOT__A = 0U;
        vlSelfRef.tb_mac_unit__DOT__B = 0U;
        vlSelfRef.tb_mac_unit__DOT__Acc_in = 0ULL;
        Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                           "@(negedge tb_mac_unit.clk)");
        co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge tb_mac_unit.clk)", 
                                                             "tb\\tb_mac_unit.sv", 
                                                             221);
        __Vtask_tb_mac_unit__DOT__check_result__44__exp_valid = 1U;
        __Vtask_tb_mac_unit__DOT__check_result__44__exp 
            = __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__exp;
        VL_SFORMAT_NX(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__44__name
                      ,"20.%0d Golden Vector",1, '~',32,
                      ((IData)(1U) + __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n));
        vlSelfRef.tb_mac_unit__DOT__total_tests = ((IData)(1U) 
                                                   + vlSelfRef.tb_mac_unit__DOT__total_tests);
        if (VL_LIKELY(((((IData)(vlSelfRef.tb_mac_unit__DOT__Out) 
                         == (IData)(__Vtask_tb_mac_unit__DOT__check_result__44__exp)) 
                        & ((IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out) 
                           == (IData)(__Vtask_tb_mac_unit__DOT__check_result__44__exp_valid)))))) {
            VL_WRITEF_NX("[PASS] %s | Out=%h Valid=%b\n",3
                         , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__44__name)
                         , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                         , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
            vlSelfRef.tb_mac_unit__DOT__tests_passed 
                = ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_passed);
        } else {
            VL_WRITEF_NX("[%0t] %%Error: tb_mac_unit.sv:102: Assertion failed in %m: [FAIL] %s | Expected: Out=%h Valid=%b | Got: Out=%h Valid=%b\n",8, 'M',vlSymsp->name(),"tb_mac_unit.check_result", 'T',-9
                         , '#',64,VL_TIME_UNITED_Q(1000)
                         , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__44__name)
                         , '~',16,(IData)(__Vtask_tb_mac_unit__DOT__check_result__44__exp)
                         , '#',1,(IData)(__Vtask_tb_mac_unit__DOT__check_result__44__exp_valid)
                         , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                         , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
            VL_STOP_MT("tb\\tb_mac_unit.sv", 102, "");
            vlSelfRef.tb_mac_unit__DOT__tests_failed 
                = ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_failed);
        }
        __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n 
            = ((IData)(1U) + __Vtask_tb_mac_unit__DOT__test_golden_vectors__43__unnamedblk4__DOT__n);
    }
    VL_WRITEF_NX("\n========================================\nTest Summary:\nTotal Tests : %0d\nPassed      : %0d\nFailed      : %0d\n========================================\n",3
                 , '~',32,vlSelfRef.tb_mac_unit__DOT__total_tests
                 , '~',32,vlSelfRef.tb_mac_unit__DOT__tests_passed
                 , '~',32,vlSelfRef.tb_mac_unit__DOT__tests_failed);
    if ((0U == vlSelfRef.tb_mac_unit__DOT__tests_failed)) {
        VL_WRITEF_NX(">>> ALL TESTS PASSED <<<\n\n",0);
    } else {
        VL_WRITEF_NX(">>> SOME TESTS FAILED <<<\n\n",0);
    }
    VL_FINISH_MT("tb\\tb_mac_unit.sv", 294, "");
    co_return;
}

VlCoroutine Vtb_mac_unit___024root___eval_initial__TOP__Vtiming__1____Vfork_1__1(Vtb_mac_unit___024root* vlSelf, VlForkSync __Vfork_1__sync) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_initial__TOP__Vtiming__1____Vfork_1__1\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __Vtask_tb_mac_unit__DOT__test_pipeline__24__chk__DOT__unnamedblk3__DOT__i;
    __Vtask_tb_mac_unit__DOT__test_pipeline__24__chk__DOT__unnamedblk3__DOT__i = 0;
    SData/*15:0*/ __Vtask_tb_mac_unit__DOT__check_result__26__exp;
    __Vtask_tb_mac_unit__DOT__check_result__26__exp = 0;
    CData/*0:0*/ __Vtask_tb_mac_unit__DOT__check_result__26__exp_valid;
    __Vtask_tb_mac_unit__DOT__check_result__26__exp_valid = 0;
    // Body
    vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__tb_mac_unit__DOT__chk__DOT__unnamedblk1_2__DOT____Vrepeat1 = 2U;
    while (VL_LTS_III(32, 0U, vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__tb_mac_unit__DOT__chk__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                           "@(negedge tb_mac_unit.clk)");
        co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge tb_mac_unit.clk)", 
                                                             "tb\\tb_mac_unit.sv", 
                                                             168);
        vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__tb_mac_unit__DOT__chk__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__tb_mac_unit__DOT__chk__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_tb_mac_unit__DOT__test_pipeline__24__chk__DOT__unnamedblk3__DOT__i = 0U;
    __Vtask_tb_mac_unit__DOT__test_pipeline__24__chk__DOT__unnamedblk3__DOT__i = 0U;
    while (VL_GTS_III(32, 4U, __Vtask_tb_mac_unit__DOT__test_pipeline__24__chk__DOT__unnamedblk3__DOT__i)) {
        Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                           "@(negedge tb_mac_unit.clk)");
        co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge tb_mac_unit.clk)", 
                                                             "tb\\tb_mac_unit.sv", 
                                                             170);
        __Vtask_tb_mac_unit__DOT__check_result__26__exp_valid = 1U;
        __Vtask_tb_mac_unit__DOT__check_result__26__exp 
            = vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__exp_arr
            [(3U & __Vtask_tb_mac_unit__DOT__test_pipeline__24__chk__DOT__unnamedblk3__DOT__i)];
        VL_SFORMAT_NX(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__26__name
                      ,"9. Pipeline %0d",1, '~',32,__Vtask_tb_mac_unit__DOT__test_pipeline__24__chk__DOT__unnamedblk3__DOT__i);
        vlSelfRef.tb_mac_unit__DOT__total_tests = ((IData)(1U) 
                                                   + vlSelfRef.tb_mac_unit__DOT__total_tests);
        if (VL_LIKELY(((((IData)(vlSelfRef.tb_mac_unit__DOT__Out) 
                         == (IData)(__Vtask_tb_mac_unit__DOT__check_result__26__exp)) 
                        & ((IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out) 
                           == (IData)(__Vtask_tb_mac_unit__DOT__check_result__26__exp_valid)))))) {
            VL_WRITEF_NX("[PASS] %s | Out=%h Valid=%b\n",3
                         , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__26__name)
                         , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                         , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
            vlSelfRef.tb_mac_unit__DOT__tests_passed 
                = ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_passed);
        } else {
            VL_WRITEF_NX("[%0t] %%Error: tb_mac_unit.sv:102: Assertion failed in %m: [FAIL] %s | Expected: Out=%h Valid=%b | Got: Out=%h Valid=%b\n",8, 'M',vlSymsp->name(),"tb_mac_unit.check_result", 'T',-9
                         , '#',64,VL_TIME_UNITED_Q(1000)
                         , 'S',&(vlSelfRef.__Vtask_tb_mac_unit__DOT__check_result__26__name)
                         , '~',16,(IData)(__Vtask_tb_mac_unit__DOT__check_result__26__exp)
                         , '#',1,(IData)(__Vtask_tb_mac_unit__DOT__check_result__26__exp_valid)
                         , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                         , '#',1,(IData)(vlSelfRef.tb_mac_unit__DOT__Valid_out));
            VL_STOP_MT("tb\\tb_mac_unit.sv", 102, "");
            vlSelfRef.tb_mac_unit__DOT__tests_failed 
                = ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__tests_failed);
        }
        __Vtask_tb_mac_unit__DOT__test_pipeline__24__chk__DOT__unnamedblk3__DOT__i 
            = ((IData)(1U) + __Vtask_tb_mac_unit__DOT__test_pipeline__24__chk__DOT__unnamedblk3__DOT__i);
    }
    __Vfork_1__sync.done("tb\\tb_mac_unit.sv", 167);
    co_return;
}

VlCoroutine Vtb_mac_unit___024root___eval_initial__TOP__Vtiming__1____Vfork_1__0(Vtb_mac_unit___024root* vlSelf, VlForkSync __Vfork_1__sync) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_initial__TOP__Vtiming__1____Vfork_1__0\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __Vtask_tb_mac_unit__DOT__test_pipeline__24__drv__DOT__unnamedblk2__DOT__i;
    __Vtask_tb_mac_unit__DOT__test_pipeline__24__drv__DOT__unnamedblk2__DOT__i = 0;
    // Body
    __Vtask_tb_mac_unit__DOT__test_pipeline__24__drv__DOT__unnamedblk2__DOT__i = 0U;
    __Vtask_tb_mac_unit__DOT__test_pipeline__24__drv__DOT__unnamedblk2__DOT__i = 0U;
    while (VL_GTS_III(32, 4U, __Vtask_tb_mac_unit__DOT__test_pipeline__24__drv__DOT__unnamedblk2__DOT__i)) {
        Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                           "@(negedge tb_mac_unit.clk)");
        co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge tb_mac_unit.clk)", 
                                                             "tb\\tb_mac_unit.sv", 
                                                             157);
        vlSelfRef.tb_mac_unit__DOT__A = vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__a_arr
            [(3U & __Vtask_tb_mac_unit__DOT__test_pipeline__24__drv__DOT__unnamedblk2__DOT__i)];
        vlSelfRef.tb_mac_unit__DOT__B = vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__b_arr
            [(3U & __Vtask_tb_mac_unit__DOT__test_pipeline__24__drv__DOT__unnamedblk2__DOT__i)];
        vlSelfRef.tb_mac_unit__DOT__Acc_in = vlSelfRef.__Vtask_tb_mac_unit__DOT__test_pipeline__24__acc_arr
            [(3U & __Vtask_tb_mac_unit__DOT__test_pipeline__24__drv__DOT__unnamedblk2__DOT__i)];
        vlSelfRef.tb_mac_unit__DOT__valid_in = 1U;
        __Vtask_tb_mac_unit__DOT__test_pipeline__24__drv__DOT__unnamedblk2__DOT__i 
            = ((IData)(1U) + __Vtask_tb_mac_unit__DOT__test_pipeline__24__drv__DOT__unnamedblk2__DOT__i);
    }
    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(vlSelf, 
                                                       "@(negedge tb_mac_unit.clk)");
    co_await vlSelfRef.__VtrigSched_hd325cf94__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge tb_mac_unit.clk)", 
                                                         "tb\\tb_mac_unit.sv", 
                                                         163);
    vlSelfRef.tb_mac_unit__DOT__valid_in = 0U;
    vlSelfRef.tb_mac_unit__DOT__A = 0U;
    vlSelfRef.tb_mac_unit__DOT__B = 0U;
    vlSelfRef.tb_mac_unit__DOT__Acc_in = 0ULL;
    __Vfork_1__sync.done("tb\\tb_mac_unit.sv", 155);
    co_return;
}

bool Vtb_mac_unit___024root___trigger_anySet__ico(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___trigger_anySet__ico\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((1U > n));
    return (0U);
}

bool Vtb_mac_unit___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___trigger_anySet__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((1U > n));
    return (0U);
}

void Vtb_mac_unit___024root___nba_sequent__TOP__0(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___nba_sequent__TOP__0\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_wrapped;
    __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_wrapped = 0;
    IData/*31:0*/ __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_idx;
    __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_idx = 0;
    CData/*0:0*/ __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_wrapped;
    __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_wrapped = 0;
    IData/*31:0*/ __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_idx;
    __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_idx = 0;
    // Body
    if (vlSymsp->_vm_contextp__->assertCtlGet(VerilatedAssertCtlQuery::ASSERT_CTL_ON, 1, 1)) {
        if (vlSymsp->_vm_contextp__->assertCtlGet(VerilatedAssertCtlQuery::ASSERT_CTL_FAIL_ON, 1, 1)) {
            if (VL_UNLIKELY(((1U & (~ ((~ ((~ (IData)(vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__rst_n)) 
                                           | (~ (IData)(vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_2_1)))) 
                                       | (~ (IData)(vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__Valid_out)))))))) {
                VL_WRITEF_NX("[%0t] %%Error: mac_unit_sva.sv:55: Assertion failed in %m: [SVA-A3] \345\244\215\344\275\215\346\234\237\351\227\264/\351\207\212\346\224\276\351\246\226\346\213\215\345\207\272\347\216\260\351\235\236\346\263\225 Valid_out (t=%0t)\n",4, 'M',vlSymsp->name(),"tb_mac_unit.uut.u_mac_unit_sva.a_reset_no_valid", 'T',-9
                             , '#',64,VL_TIME_UNITED_Q(1000)
                             , '#',64,VL_TIME_UNITED_Q(1000));
                VL_STOP_MT("rtl\\mac_unit_sva.sv", 55, "");
            }
            if (VL_UNLIKELY(((1U & (~ ((~ (((IData)(vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__rst_n) 
                                            & (IData)(vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_2_1)) 
                                           & (IData)(vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_2_2))) 
                                       | ((IData)(vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__Valid_out) 
                                          == (IData)(vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_3_2)))))))) {
                VL_WRITEF_NX("[%0t] %%Error: mac_unit_sva.sv:45: Assertion failed in %m: [SVA-A2] Valid_out \344\270\216 2 \346\213\215\345\211\215 valid_in \344\270\215\344\270\200\350\207\264 (\344\274\252\350\204\211\345\206\262\346\210\226\344\270\242\345\214\205, t=%0t)\n",4, 'M',vlSymsp->name(),"tb_mac_unit.uut.u_mac_unit_sva.a_latency_exact", 'T',-9
                             , '#',64,VL_TIME_UNITED_Q(1000)
                             , '#',64,VL_TIME_UNITED_Q(1000));
                VL_STOP_MT("rtl\\mac_unit_sva.sv", 45, "");
            }
            if (vlSelfRef.tb_mac_unit__DOT__rst_n) {
                if (VL_UNLIKELY(((1U & (~ ((~ (IData)(vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__Valid_out)) 
                                           | (VL_LTES_III(16, 0x8000U, (IData)(vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__Out)) 
                                              & VL_GTES_III(16, 0x7fffU, (IData)(vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__Out))))))))) {
                    VL_WRITEF_NX("[%0t] %%Error: mac_unit_sva.sv:77: Assertion failed in %m: [SVA-A5] Valid_out \346\234\211\346\225\210\346\227\266 Out \350\266\212\347\225\214\346\210\226\345\220\253 X \346\200\201: %h (t=%0t)\n",5, 'M',vlSymsp->name(),"tb_mac_unit.uut.u_mac_unit_sva.a_out_range", 'T',-9
                                 , '#',64,VL_TIME_UNITED_Q(1000)
                                 , '~',16,(IData)(vlSelfRef.tb_mac_unit__DOT__Out)
                                 , '#',64,VL_TIME_UNITED_Q(1000));
                    VL_STOP_MT("rtl\\mac_unit_sva.sv", 77, "");
                }
                if (VL_UNLIKELY((((((IData)(vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___1__s2) 
                                    & (vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___1__kill 
                                       == vlSymsp->_vm_contextp__->assertCtlGet(VerilatedAssertCtlQuery::ASSERT_CTL_KILL, 1, 1))) 
                                   & (IData)(vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__rst_n)) 
                                  & ((IData)(vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_6_1) 
                                     != (IData)(vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__Out)))))) {
                    VL_WRITEF_NX("[%0t] %%Error: mac_unit_sva.sv:66: Assertion failed in %m: [SVA-A4] \346\227\240\346\225\210\345\221\250\346\234\237 Out \346\234\252\344\277\235\346\214\201\344\270\212\344\270\200\346\213\215\346\234\211\346\225\210\345\200\274 (t=%0t)\n",4, 'M',vlSymsp->name(),"tb_mac_unit.uut.u_mac_unit_sva.a_out_hold", 'T',-9
                                 , '#',64,VL_TIME_UNITED_Q(1000)
                                 , '#',64,VL_TIME_UNITED_Q(1000));
                    VL_STOP_MT("rtl\\mac_unit_sva.sv", 66, "");
                }
                if (VL_UNLIKELY((((((0U != ((IData)(vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_wrapped)
                                             ? (IData)(vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_head)
                                             : 0U)) 
                                    & (vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__kill 
                                       == vlSymsp->_vm_contextp__->assertCtlGet(VerilatedAssertCtlQuery::ASSERT_CTL_KILL, 1, 1))) 
                                   & (IData)(vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__rst_n)) 
                                  & (~ (IData)(vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__Valid_out)))))) {
                    VL_WRITEF_NX("[%0t] %%Error: mac_unit_sva.sv:34: Assertion failed in %m: [SVA-A1] valid_in \345\220\216 2 \346\213\215\346\234\252\345\207\272\347\216\260 Valid_out (t=%0t)\n",4, 'M',vlSymsp->name(),"tb_mac_unit.uut.u_mac_unit_sva.a_latency_fwd", 'T',-9
                                 , '#',64,VL_TIME_UNITED_Q(1000)
                                 , '#',64,VL_TIME_UNITED_Q(1000));
                    VL_STOP_MT("rtl\\mac_unit_sva.sv", 34, "");
                }
            }
        }
    }
    __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_idx 
        = vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_idx;
    __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_wrapped 
        = vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_wrapped;
    __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_idx 
        = vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_idx;
    __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_wrapped 
        = vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_wrapped;
    if ((1U & ((vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__kill 
                != vlSymsp->_vm_contextp__->assertCtlGet(VerilatedAssertCtlQuery::ASSERT_CTL_KILL, 1, 1)) 
               | (~ (IData)(vlSelfRef.tb_mac_unit__DOT__rst_n))))) {
        __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_idx = 0U;
        vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_liveCount = 0U;
    } else {
        __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_idx 
            = ((1U == vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_idx)
                ? 0U : ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_idx));
        vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_liveCount 
            = ((vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_liveCount 
                - ((IData)(vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_wrapped)
                    ? (IData)(vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_head)
                    : 0U)) + (0U != ((IData)(vlSelfRef.tb_mac_unit__DOT__rst_n)
                                      ? (vlSymsp->_vm_contextp__->assertCtlGet(VerilatedAssertCtlQuery::ASSERT_CTL_ON, 1, 1) 
                                         & (IData)(vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__valid_in))
                                      : 0U)));
    }
    __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_wrapped 
        = ((1U & (~ ((vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__kill 
                      != vlSymsp->_vm_contextp__->assertCtlGet(VerilatedAssertCtlQuery::ASSERT_CTL_KILL, 1, 1)) 
                     | (~ (IData)(vlSelfRef.tb_mac_unit__DOT__rst_n))))) 
           && ((IData)(vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_wrapped) 
               | (1U == vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_idx)));
    if ((1U & (((~ (IData)(vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__valid_in)) 
                | (~ (IData)(vlSelfRef.tb_mac_unit__DOT__rst_n))) 
               | (vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__kill 
                  != vlSymsp->_vm_contextp__->assertCtlGet(VerilatedAssertCtlQuery::ASSERT_CTL_KILL, 1, 2))))) {
        __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_idx = 0U;
        vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_liveCount = 0U;
    } else {
        __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_idx 
            = ((2U == vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_idx)
                ? 0U : ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_idx));
        vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_liveCount 
            = ((vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_liveCount 
                - ((IData)(vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_wrapped)
                    ? (IData)(vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_head)
                    : 0U)) + (0U != ((IData)(vlSelfRef.tb_mac_unit__DOT__rst_n)
                                      ? (vlSymsp->_vm_contextp__->assertCtlGet(VerilatedAssertCtlQuery::ASSERT_CTL_ON, 1, 2) 
                                         & (IData)(vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__valid_in))
                                      : 0U)));
    }
    __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_wrapped 
        = ((1U & (~ (1U & (((~ (IData)(vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__valid_in)) 
                            | (~ (IData)(vlSelfRef.tb_mac_unit__DOT__rst_n))) 
                           | (vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__kill 
                              != vlSymsp->_vm_contextp__->assertCtlGet(VerilatedAssertCtlQuery::ASSERT_CTL_KILL, 1, 2)))))) 
           && ((IData)(vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_wrapped) 
               | (2U == vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_idx)));
    vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_2_2 
        = vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_2_1;
    vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_3_2 
        = vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_3_1;
    vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_6_1 
        = vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__Out;
    vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___1__s2 
        = (((vlSymsp->_vm_contextp__->assertCtlGet(VerilatedAssertCtlQuery::ASSERT_CTL_ON, 1, 1) 
             & (~ (IData)(vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__uut__DOT__valid_s1_reg))) 
            & (IData)(vlSelfRef.tb_mac_unit__DOT__rst_n)) 
           & (vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___1__kill 
              == vlSymsp->_vm_contextp__->assertCtlGet(VerilatedAssertCtlQuery::ASSERT_CTL_KILL, 1, 1)));
    vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_wrapped 
        = __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_wrapped;
    vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_wrapped 
        = __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_wrapped;
    if ((1U & (~ (1U & (((~ (IData)(vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__valid_in)) 
                         | (~ (IData)(vlSelfRef.tb_mac_unit__DOT__rst_n))) 
                        | (vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__kill 
                           != vlSymsp->_vm_contextp__->assertCtlGet(VerilatedAssertCtlQuery::ASSERT_CTL_KILL, 1, 2))))))) {
        if ((2U >= vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_idx)) {
            vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_ring 
                = (((~ ((IData)(1U) << vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_idx)) 
                    & (IData)(vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_ring)) 
                   | (7U & ((0U != ((IData)(vlSelfRef.tb_mac_unit__DOT__rst_n)
                                     ? (vlSymsp->_vm_contextp__->assertCtlGet(VerilatedAssertCtlQuery::ASSERT_CTL_ON, 1, 2) 
                                        & (IData)(vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__valid_in))
                                     : 0U)) << vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_idx)));
        }
        vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_head 
            = ((2U >= ((2U == vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_idx)
                        ? 0U : ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_idx))) 
               && (1U & ((IData)(vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_ring) 
                         >> ((2U == vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_idx)
                              ? 0U : ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_idx)))));
    }
    if ((1U & (~ ((vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__kill 
                   != vlSymsp->_vm_contextp__->assertCtlGet(VerilatedAssertCtlQuery::ASSERT_CTL_KILL, 1, 1)) 
                  | (~ (IData)(vlSelfRef.tb_mac_unit__DOT__rst_n)))))) {
        if ((1U >= vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_idx)) {
            vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_ring 
                = (((~ ((IData)(1U) << vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_idx)) 
                    & (IData)(vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_ring)) 
                   | (3U & ((0U != ((IData)(vlSelfRef.tb_mac_unit__DOT__rst_n)
                                     ? (vlSymsp->_vm_contextp__->assertCtlGet(VerilatedAssertCtlQuery::ASSERT_CTL_ON, 1, 1) 
                                        & (IData)(vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__valid_in))
                                     : 0U)) << vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_idx)));
        }
        vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_head 
            = ((1U >= ((1U == vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_idx)
                        ? 0U : ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_idx))) 
               && (1U & ((IData)(vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_ring) 
                         >> ((1U == vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_idx)
                              ? 0U : ((IData)(1U) + vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_idx)))));
    }
    vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_2_1 
        = vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__rst_n;
    vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_3_1 
        = vlSelfRef.__Vsampled_TOP__tb_mac_unit__DOT__valid_in;
    vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___1__kill 
        = vlSymsp->_vm_contextp__->assertCtlGet(VerilatedAssertCtlQuery::ASSERT_CTL_KILL, 1, 1);
    vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_idx 
        = __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_idx;
    vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_idx 
        = __Vdly__tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_idx;
    vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__kill 
        = vlSymsp->_vm_contextp__->assertCtlGet(VerilatedAssertCtlQuery::ASSERT_CTL_KILL, 1, 2);
    vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__kill 
        = vlSymsp->_vm_contextp__->assertCtlGet(VerilatedAssertCtlQuery::ASSERT_CTL_KILL, 1, 1);
}

void Vtb_mac_unit___024root___timing_ready(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___timing_ready\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((8ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_hd325cf94__0.ready("@(negedge tb_mac_unit.clk)");
    }
}

void Vtb_mac_unit___024root___timing_resume(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___timing_resume\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VtrigSched_hd325cf94__0.moveToResumeQueue(
                                                          "@(negedge tb_mac_unit.clk)");
    vlSelfRef.__VtrigSched_hd325cf94__0.resume("@(negedge tb_mac_unit.clk)");
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vtb_mac_unit___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___trigger_orInto__act_vec_vec\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((0U >= n));
}

void Vtb_mac_unit___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

void Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0(Vtb_mac_unit___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root____VbeforeTrig_hd325cf94__0\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)((((~ (IData)(vlSelfRef.tb_mac_unit__DOT__clk)) 
                                   & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tb_mac_unit__DOT__clk__0)) 
                                  << 3U)));
    vlSelfRef.__Vtrigprevexpr___TOP__tb_mac_unit__DOT__clk__0 
        = vlSelfRef.tb_mac_unit__DOT__clk;
    if ((8ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_hd325cf94__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

#ifdef VL_DEBUG
void Vtb_mac_unit___024root___eval_debug_assertions(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_debug_assertions\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
