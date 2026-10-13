// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vtb_mac_unit__pch.h"

//============================================================
// Constructors

Vtb_mac_unit::Vtb_mac_unit(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vtb_mac_unit__Syms(contextp(), _vcname__, this)}
    , m_evalLoop{*this, /*convergeLimit:*/ 10000}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vtb_mac_unit::Vtb_mac_unit(const char* _vcname__)
    : Vtb_mac_unit(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vtb_mac_unit::~Vtb_mac_unit() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vtb_mac_unit___024root___eval_debug_assertions(Vtb_mac_unit___024root* vlSelf);
#endif  // VL_DEBUG
VL_ATTR_COLD void Vtb_mac_unit___024root___eval_static(Vtb_mac_unit___024root* vlSelf);
void Vtb_mac_unit___024root___eval_initial(Vtb_mac_unit___024root* vlSelf);
VL_ATTR_COLD bool Vtb_mac_unit___024root___eval_stl(Vtb_mac_unit___024root* vlSelf, CData/*0:0*/ firstIteration);
void Vtb_mac_unit___024root___eval_sample(Vtb_mac_unit___024root* vlSelf);
bool Vtb_mac_unit___024root___eval_ico(Vtb_mac_unit___024root* vlSelf, CData/*0:0*/ firstIteration);
bool Vtb_mac_unit___024root___eval_act(Vtb_mac_unit___024root* vlSelf);
bool Vtb_mac_unit___024root___eval_inact(Vtb_mac_unit___024root* vlSelf);
bool Vtb_mac_unit___024root___eval_nba(Vtb_mac_unit___024root* vlSelf);
bool Vtb_mac_unit___024root___eval_obs(Vtb_mac_unit___024root* vlSelf);
bool Vtb_mac_unit___024root___eval_react(Vtb_mac_unit___024root* vlSelf);
void Vtb_mac_unit___024root___eval_postponed(Vtb_mac_unit___024root* vlSelf);
VL_ATTR_COLD void Vtb_mac_unit___024root___eval_final(Vtb_mac_unit___024root* vlSelf);
VL_ATTR_COLD void Vtb_mac_unit___024root___eval_dump_triggers__stl(Vtb_mac_unit___024root* vlSelf);
VL_ATTR_COLD void Vtb_mac_unit___024root___eval_dump_triggers__ico(Vtb_mac_unit___024root* vlSelf);
VL_ATTR_COLD void Vtb_mac_unit___024root___eval_dump_triggers__act(Vtb_mac_unit___024root* vlSelf);
VL_ATTR_COLD void Vtb_mac_unit___024root___eval_dump_triggers__nba(Vtb_mac_unit___024root* vlSelf);
VL_ATTR_COLD void Vtb_mac_unit___024root___eval_dump_triggers__obs(Vtb_mac_unit___024root* vlSelf);
VL_ATTR_COLD void Vtb_mac_unit___024root___eval_dump_triggers__react(Vtb_mac_unit___024root* vlSelf);

void Vtb_mac_unit::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vtb_mac_unit::eval_step\n"); );
    m_evalLoop.eval();
}

void Vtb_mac_unit::evalBegin() {
#ifdef VL_DEBUG
    // Debug assertions
    Vtb_mac_unit___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->clearTriggeredEvents();
    vlSymsp->__Vm_deleter.deleteAll();
}

void Vtb_mac_unit::evalEnd() {
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
    vlSymsp->TOP.__VdlySched.cleanupForevered();
}

void Vtb_mac_unit::evalStatic() {
    Vtb_mac_unit___024root___eval_static(&(vlSymsp->TOP));
}

void Vtb_mac_unit::evalInitial() {
    Vtb_mac_unit___024root___eval_initial(&(vlSymsp->TOP));
}

bool Vtb_mac_unit::evalStl(bool firstIteration) {
    return Vtb_mac_unit___024root___eval_stl(&(vlSymsp->TOP), firstIteration);
}

void Vtb_mac_unit::evalSample() {
    Vtb_mac_unit___024root___eval_sample(&(vlSymsp->TOP));
}

bool Vtb_mac_unit::evalIco(bool firstIteration) {
    return Vtb_mac_unit___024root___eval_ico(&(vlSymsp->TOP), firstIteration);
}

bool Vtb_mac_unit::evalAct() {
    return Vtb_mac_unit___024root___eval_act(&(vlSymsp->TOP));
}

bool Vtb_mac_unit::evalInact() {
    return Vtb_mac_unit___024root___eval_inact(&(vlSymsp->TOP));
}

bool Vtb_mac_unit::evalNba() {
    return Vtb_mac_unit___024root___eval_nba(&(vlSymsp->TOP));
}

bool Vtb_mac_unit::evalObs() {
    return Vtb_mac_unit___024root___eval_obs(&(vlSymsp->TOP));
}

bool Vtb_mac_unit::evalReact() {
    return Vtb_mac_unit___024root___eval_react(&(vlSymsp->TOP));
}

void Vtb_mac_unit::evalPostponed() {
    Vtb_mac_unit___024root___eval_postponed(&(vlSymsp->TOP));
}

void Vtb_mac_unit::evalFinal() {
    Vtb_mac_unit___024root___eval_final(&(vlSymsp->TOP));
}

VL_ATTR_COLD void Vtb_mac_unit::dumpTriggersStl() {
    Vtb_mac_unit___024root___eval_dump_triggers__stl(&(vlSymsp->TOP));
}

VL_ATTR_COLD void Vtb_mac_unit::dumpTriggersIco() {
    Vtb_mac_unit___024root___eval_dump_triggers__ico(&(vlSymsp->TOP));
}

VL_ATTR_COLD void Vtb_mac_unit::dumpTriggersAct() {
    Vtb_mac_unit___024root___eval_dump_triggers__act(&(vlSymsp->TOP));
}

VL_ATTR_COLD void Vtb_mac_unit::dumpTriggersNba() {
    Vtb_mac_unit___024root___eval_dump_triggers__nba(&(vlSymsp->TOP));
}

VL_ATTR_COLD void Vtb_mac_unit::dumpTriggersObs() {
    Vtb_mac_unit___024root___eval_dump_triggers__obs(&(vlSymsp->TOP));
}

VL_ATTR_COLD void Vtb_mac_unit::dumpTriggersReact() {
    Vtb_mac_unit___024root___eval_dump_triggers__react(&(vlSymsp->TOP));
}

//============================================================
// Events and timing
bool Vtb_mac_unit::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty() && !contextp()->gotFinish(); }

uint64_t Vtb_mac_unit::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vtb_mac_unit::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

VL_ATTR_COLD void Vtb_mac_unit::final() {
    contextp()->executingFinal(true);
    evalFinal();
    contextp()->executingFinal(false);
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vtb_mac_unit::hierName() const { return vlSymsp->name(); }
const char* Vtb_mac_unit::modelName() const { return "Vtb_mac_unit"; }
unsigned Vtb_mac_unit::threads() const { return 1; }
void Vtb_mac_unit::prepareClone() const { contextp()->prepareClone(); }
void Vtb_mac_unit::atClone() const {
    contextp()->threadPoolpOnClone();
}
