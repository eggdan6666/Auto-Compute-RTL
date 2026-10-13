// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtb_mac_unit.h for the primary calling header

#ifndef VERILATED_VTB_MAC_UNIT___024ROOT_H_
#define VERILATED_VTB_MAC_UNIT___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vtb_mac_unit__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtb_mac_unit___024root final {
  public:

    // DESIGN-SPECIFIC STATE
    CData/*0:0*/ tb_mac_unit__DOT__clk;
    CData/*0:0*/ tb_mac_unit__DOT__rst_n;
    CData/*0:0*/ tb_mac_unit__DOT__valid_in;
    CData/*0:0*/ tb_mac_unit__DOT__Valid_out;
    CData/*0:0*/ tb_mac_unit__DOT__uut__DOT__valid_s1_reg;
    CData/*1:0*/ tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_ring;
    CData/*0:0*/ tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_head;
    CData/*0:0*/ tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_wrapped;
    CData/*0:0*/ tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___1__s2;
    CData/*2:0*/ tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_ring;
    CData/*0:0*/ tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_head;
    CData/*0:0*/ tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_wrapped;
    CData/*0:0*/ tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_2_1;
    CData/*0:0*/ tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_2_2;
    CData/*0:0*/ tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_3_1;
    CData/*0:0*/ tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_3_2;
    CData/*0:0*/ __Vsampled_TOP__tb_mac_unit__DOT__rst_n;
    CData/*0:0*/ __Vsampled_TOP__tb_mac_unit__DOT__Valid_out;
    CData/*0:0*/ __Vsampled_TOP__tb_mac_unit__DOT__valid_in;
    CData/*0:0*/ __Vsampled_TOP__tb_mac_unit__DOT__uut__DOT__valid_s1_reg;
    CData/*0:0*/ __Vtrigprevexpr___TOP__tb_mac_unit__DOT__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__tb_mac_unit__DOT__rst_n__0;
    SData/*15:0*/ tb_mac_unit__DOT__A;
    SData/*15:0*/ tb_mac_unit__DOT__B;
    SData/*15:0*/ tb_mac_unit__DOT__Out;
    SData/*15:0*/ tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT___Vpast_6_1;
    SData/*15:0*/ __Vsampled_TOP__tb_mac_unit__DOT__Out;
    IData/*31:0*/ tb_mac_unit__DOT__total_tests;
    IData/*31:0*/ tb_mac_unit__DOT__tests_passed;
    IData/*31:0*/ tb_mac_unit__DOT__tests_failed;
    IData/*31:0*/ tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__kill;
    IData/*31:0*/ tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_idx;
    IData/*31:0*/ tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___0__d2_liveCount;
    IData/*31:0*/ tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___1__kill;
    IData/*31:0*/ tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__kill;
    IData/*31:0*/ tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_idx;
    IData/*31:0*/ tb_mac_unit__DOT__uut__DOT__u_mac_unit_sva__DOT____Vnfa___3__d2_liveCount;
    IData/*31:0*/ __Vtask_tb_mac_unit__DOT__test_pipeline__24__tb_mac_unit__DOT__chk__DOT__unnamedblk1_2__DOT____Vrepeat1;
    IData/*31:0*/ __VdfgRegularize_h6e95ff9d_0_2;
    IData/*31:0*/ __Vi;
    QData/*39:0*/ tb_mac_unit__DOT__Acc_in;
    QData/*39:0*/ tb_mac_unit__DOT__uut__DOT__prod_ext_s1_reg;
    QData/*39:0*/ tb_mac_unit__DOT__uut__DOT__acc_s1_reg;
    QData/*40:0*/ __VdfgRegularize_h6e95ff9d_0_1;
    VlUnpacked<SData/*15:0*/, 4> __Vtask_tb_mac_unit__DOT__test_pipeline__24__a_arr;
    VlUnpacked<SData/*15:0*/, 4> __Vtask_tb_mac_unit__DOT__test_pipeline__24__b_arr;
    VlUnpacked<QData/*39:0*/, 4> __Vtask_tb_mac_unit__DOT__test_pipeline__24__acc_arr;
    VlUnpacked<SData/*15:0*/, 4> __Vtask_tb_mac_unit__DOT__test_pipeline__24__exp_arr;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VicoTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggeredAcc;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    std::string __Vtask_tb_mac_unit__DOT__run_single_test__1__name;
    std::string __Vtask_tb_mac_unit__DOT__check_result__3__name;
    std::string __Vtask_tb_mac_unit__DOT__run_single_test__4__name;
    std::string __Vtask_tb_mac_unit__DOT__check_result__6__name;
    std::string __Vtask_tb_mac_unit__DOT__run_single_test__7__name;
    std::string __Vtask_tb_mac_unit__DOT__check_result__9__name;
    std::string __Vtask_tb_mac_unit__DOT__run_single_test__11__name;
    std::string __Vtask_tb_mac_unit__DOT__check_result__13__name;
    std::string __Vtask_tb_mac_unit__DOT__run_single_test__14__name;
    std::string __Vtask_tb_mac_unit__DOT__check_result__16__name;
    std::string __Vtask_tb_mac_unit__DOT__run_single_test__17__name;
    std::string __Vtask_tb_mac_unit__DOT__check_result__19__name;
    std::string __Vtask_tb_mac_unit__DOT__run_single_test__20__name;
    std::string __Vtask_tb_mac_unit__DOT__check_result__22__name;
    std::string __Vtask_tb_mac_unit__DOT__check_result__26__name;
    std::string __Vtask_tb_mac_unit__DOT__run_single_test__29__name;
    std::string __Vtask_tb_mac_unit__DOT__check_result__31__name;
    std::string __Vtask_tb_mac_unit__DOT__run_single_test__33__name;
    std::string __Vtask_tb_mac_unit__DOT__check_result__35__name;
    std::string __Vtask_tb_mac_unit__DOT__run_single_test__36__name;
    std::string __Vtask_tb_mac_unit__DOT__check_result__38__name;
    std::string __Vtask_tb_mac_unit__DOT__run_single_test__39__name;
    std::string __Vtask_tb_mac_unit__DOT__check_result__41__name;
    std::string __Vtask_tb_mac_unit__DOT__check_result__44__name;
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_hd325cf94__0;

    // INTERNAL VARIABLES
    Vtb_mac_unit__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtb_mac_unit___024root(Vtb_mac_unit__Syms* symsp, const char* namep);
    ~Vtb_mac_unit___024root();
    VL_UNCOPYABLE(Vtb_mac_unit___024root);

    // INTERNAL METHODS
};


#endif  // guard
