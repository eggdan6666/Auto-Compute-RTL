// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtb_mac_unit.h for the primary calling header

#include "Vtb_mac_unit__pch.h"

void Vtb_mac_unit___024root___timing_ready(Vtb_mac_unit___024root* vlSelf);

VL_ATTR_COLD void Vtb_mac_unit___024root___eval_static(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_static\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    {
        // Inlined CFunc: _eval_static__TOP
        vlSelfRef.tb_mac_unit__DOT__total_tests = 0U;
        vlSelfRef.tb_mac_unit__DOT__tests_passed = 0U;
        vlSelfRef.tb_mac_unit__DOT__tests_failed = 0U;
        vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_2_1 = 0U;
        vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_2_2 = 0U;
        vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_3_1 = 0U;
        vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_3_2 = 0U;
        vlSelfRef.tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_6_1 = 0U;
    }
    vlSelfRef.__Vtrigprevexpr___TOP__tb_mac_unit__DOT__clk__0 
        = vlSelfRef.tb_mac_unit__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__tb_mac_unit__DOT__rst_n__0 
        = vlSelfRef.tb_mac_unit__DOT__rst_n;
    Vtb_mac_unit___024root___timing_ready(vlSelf);
    do {
        vlSelfRef.__VactTriggeredAcc[vlSelfRef.__Vi] 
            = vlSelfRef.__VactTriggered[vlSelfRef.__Vi];
        vlSelfRef.__Vi = ((IData)(1U) + vlSelfRef.__Vi);
    } while ((0U >= vlSelfRef.__Vi));
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_mac_unit___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtb_mac_unit___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

VL_ATTR_COLD bool Vtb_mac_unit___024root___eval_stl(Vtb_mac_unit___024root* vlSelf, CData/*0:0*/ firstIteration) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_stl\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[0U]) 
                                     | (IData)((IData)(firstIteration)));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtb_mac_unit___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vtb_mac_unit___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        {
            // Inlined CFunc: _eval_body__stl
            if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
                {
                    // Inlined CFunc: _stl_sequent__TOP__0
                    QData/*39:0*/ __Vinline_0__eval_body__stl___Vinline_0__stl_sequent__TOP__0___VdfgRegularize_h6e95ff9d_0_0;
                    __Vinline_0__eval_body__stl___Vinline_0__stl_sequent__TOP__0___VdfgRegularize_h6e95ff9d_0_0 = 0;
                    QData/*40:0*/ __Vinline_0__eval_body__stl___Vinline_0__stl_sequent__TOP__0___VdfgRegularize_h6e95ff9d_0_3;
                    __Vinline_0__eval_body__stl___Vinline_0__stl_sequent__TOP__0___VdfgRegularize_h6e95ff9d_0_3 = 0;
                    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2 
                        = VL_MULS_III(32, VL_EXTENDS_II(32,16, (IData)(vlSelfRef.tb_mac_unit__DOT__A)), 
                                      VL_EXTENDS_II(32,16, (IData)(vlSelfRef.tb_mac_unit__DOT__B)));
                    __Vinline_0__eval_body__stl___Vinline_0__stl_sequent__TOP__0___VdfgRegularize_h6e95ff9d_0_3 
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
                    __Vinline_0__eval_body__stl___Vinline_0__stl_sequent__TOP__0___VdfgRegularize_h6e95ff9d_0_0 
                        = ((1U == (3U & (IData)((__Vinline_0__eval_body__stl___Vinline_0__stl_sequent__TOP__0___VdfgRegularize_h6e95ff9d_0_3 
                                                 >> 0x00000027U))))
                            ? 0x0000007fffffffffULL
                            : ((2U == (3U & (IData)(
                                                    (__Vinline_0__eval_body__stl___Vinline_0__stl_sequent__TOP__0___VdfgRegularize_h6e95ff9d_0_3 
                                                     >> 0x00000027U))))
                                ? 0x0000008000000000ULL
                                : (0x000000ffffffffffULL 
                                   & __Vinline_0__eval_body__stl___Vinline_0__stl_sequent__TOP__0___VdfgRegularize_h6e95ff9d_0_3)));
                    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 
                        = (0x000001ffffffffffULL & 
                           (VL_SHIFTRS_QQI(41,41,32, 
                                           (0x000001ffffffffffULL 
                                            & VL_EXTENDS_QQ(41,40, __Vinline_0__eval_body__stl___Vinline_0__stl_sequent__TOP__0___VdfgRegularize_h6e95ff9d_0_0)), 0x0000000fU) 
                            + (QData)((IData)(((__Vinline_0__eval_body__stl___Vinline_0__stl_sequent__TOP__0___VdfgRegularize_h6e95ff9d_0_0 
                                                >> 0x0000000eU) 
                                               & (0ULL 
                                                  != 
                                                  (0x000000000000bfffULL 
                                                   & __Vinline_0__eval_body__stl___Vinline_0__stl_sequent__TOP__0___VdfgRegularize_h6e95ff9d_0_0)))))));
                }
            }
        }
    }
    return (__VstlExecute);
}

VL_ATTR_COLD void Vtb_mac_unit___024root___eval_dump_triggers__stl(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_dump_triggers__stl\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
#ifdef VL_DEBUG
    Vtb_mac_unit___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_mac_unit___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtb_mac_unit___024root___eval_dump_triggers__ico(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_dump_triggers__ico\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
#ifdef VL_DEBUG
    Vtb_mac_unit___024root___dump_triggers__ico(vlSelfRef.__VicoTriggered, "ico"s);
#endif
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_mac_unit___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtb_mac_unit___024root___eval_dump_triggers__act(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_dump_triggers__act\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
#ifdef VL_DEBUG
    Vtb_mac_unit___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
}

VL_ATTR_COLD void Vtb_mac_unit___024root___eval_dump_triggers__nba(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_dump_triggers__nba\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
#ifdef VL_DEBUG
    Vtb_mac_unit___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
}

VL_ATTR_COLD void Vtb_mac_unit___024root___eval_dump_triggers__obs(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_dump_triggers__obs\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vtb_mac_unit___024root___eval_dump_triggers__react(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_dump_triggers__react\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vtb_mac_unit___024root___eval_final(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___eval_final\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_mac_unit___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtb_mac_unit___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtb_mac_unit___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___trigger_anySet__stl\n"); );
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

bool Vtb_mac_unit___024root___trigger_anySet__ico(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_mac_unit___024root___dump_triggers__ico(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___dump_triggers__ico\n"); );
    // Body
    if ((1U & (~ (IData)(Vtb_mac_unit___024root___trigger_anySet__ico(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'ico' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

bool Vtb_mac_unit___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_mac_unit___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtb_mac_unit___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge tb_mac_unit.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge tb_mac_unit.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @(negedge tb_mac_unit.clk)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtb_mac_unit___024root___ctor_var_reset(Vtb_mac_unit___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_mac_unit___024root___ctor_var_reset\n"); );
    Vtb_mac_unit__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->tb_mac_unit__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17176617225639541106ull);
    vlSelf->tb_mac_unit__DOT__rst_n = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12444581862963597483ull);
    vlSelf->tb_mac_unit__DOT__valid_in = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2446735857337023169ull);
    vlSelf->tb_mac_unit__DOT__A = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 169232747855670634ull);
    vlSelf->tb_mac_unit__DOT__B = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 1521432768736741686ull);
    vlSelf->tb_mac_unit__DOT__Acc_in = VL_SCOPED_RAND_RESET_Q(40, __VscopeHash, 11995801132860930326ull);
    vlSelf->tb_mac_unit__DOT__Out = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 1759827781078532976ull);
    vlSelf->tb_mac_unit__DOT__Valid_out = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 775879854468624521ull);
    vlSelf->tb_mac_unit__DOT__uut__DOT__prod_ext_s1_reg = VL_SCOPED_RAND_RESET_Q(40, __VscopeHash, 10338836581175896483ull);
    vlSelf->tb_mac_unit__DOT__uut__DOT__acc_s1_reg = VL_SCOPED_RAND_RESET_Q(40, __VscopeHash, 6212818887627197126ull);
    vlSelf->tb_mac_unit__DOT__uut__DOT__valid_s1_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10049238460348494812ull);
    vlSelf->tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__kill = 0;
    vlSelf->tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_ring = 0;
    vlSelf->tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_head = 0;
    vlSelf->tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_idx = 0;
    vlSelf->tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_liveCount = 0;
    vlSelf->tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_wrapped = 0;
    vlSelf->tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___1__kill = 0;
    vlSelf->tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___1__s2 = 0;
    vlSelf->tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__kill = 0;
    vlSelf->tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_ring = 0;
    vlSelf->tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_head = 0;
    vlSelf->tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_idx = 0;
    vlSelf->tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_liveCount = 0;
    vlSelf->tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_wrapped = 0;
    vlSelf->tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_2_1 = 0;
    vlSelf->tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_2_2 = 0;
    vlSelf->tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_3_1 = 0;
    vlSelf->tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_3_2 = 0;
    vlSelf->tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_6_1 = 0;
    vlSelf->__Vtask_tb_mac_unit__DOT__test_pipeline__24__tb_mac_unit__DOT__chk__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->__Vtask_tb_mac_unit__DOT__test_pipeline__24__a_arr[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->__Vtask_tb_mac_unit__DOT__test_pipeline__24__b_arr[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->__Vtask_tb_mac_unit__DOT__test_pipeline__24__acc_arr[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->__Vtask_tb_mac_unit__DOT__test_pipeline__24__exp_arr[__Vi0] = 0;
    }
    vlSelf->__VdfgRegularize_h6e95ff9d_0_1 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_2 = 0;
    vlSelf->__Vsampled_TOP__tb_mac_unit__DOT__rst_n = 0;
    vlSelf->__Vsampled_TOP__tb_mac_unit__DOT__Valid_out = 0;
    vlSelf->__Vsampled_TOP__tb_mac_unit__DOT__Out = 0;
    vlSelf->__Vsampled_TOP__tb_mac_unit__DOT__valid_in = 0;
    vlSelf->__Vsampled_TOP__tb_mac_unit__DOT__uut__DOT__valid_s1_reg = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VicoTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggeredAcc[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__tb_mac_unit__DOT__clk__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__tb_mac_unit__DOT__rst_n__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    vlSelf->__Vi = 0;
}
