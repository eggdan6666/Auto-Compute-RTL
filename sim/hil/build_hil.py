#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""build_hil.py — 在纯净路径 E:\\work\\hil_vl 构建 Verilator「哑执行器」

沿用仓库 sim/run_sim.sh 已踩平的雷区：
  1) 工程路径含中文/空格 → GNU Make 拒绝 ⇒ 必须在纯净目录构建（源码自动同步）
  2) VERILATOR_ROOT 必须用正斜杠
  3) WinLibs GCC16 的 -Os 触发 declone-ctor-dtor ⇒ 加 -fno-declone-ctor-dtor
  4) 本机既无 make 也无 POSIX shell（Verilator 生成的 Makefile 依赖 rm/sh，在
     PowerShell 下跑不动）⇒ 不调用 --build，改为「Verilator 只生成 C++ + g++ 直接编译」

用法:  python sim/hil/build_hil.py
"""
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
HIL = ROOT / "sim" / "hil"
OSS = Path(r"E:\tools\oss-cad-suite")
MINGW = Path(r"E:\tools\mingw64\bin")
WORK = Path(r"E:\work\hil_vl")
SHIM = Path(r"E:\work\shim")
INC = OSS / "share" / "verilator" / "include"

RTL = ["systolic_pe.sv", "rne_sat16.sv", "systolic_array.sv", "misr16.sv",
       "bist_controller.sv", "self_healing_array.sv", "abft_checksum.sv",
       "zero_mask.sv", "fault_arbiter.sv"]
HEX = ["bist_weights.hex", "bist_rows.hex", "bist_golden.hex"]


def run(cmd, env, tag):
    p = subprocess.run([str(c) for c in cmd], cwd=str(WORK), env=env,
                       capture_output=True, text=True, encoding="utf-8", errors="replace")
    if p.returncode != 0:
        print(f"[FAIL:{tag}]")
        print(p.stdout[-1500:])
        print(p.stderr[-3000:])
        sys.exit(1)
    return p


def main():
    WORK.mkdir(parents=True, exist_ok=True)
    SHIM.mkdir(parents=True, exist_ok=True)
    shutil.copy2(MINGW / "mingw32-make.exe", SHIM / "make.exe")
    for f in RTL:
        shutil.copy2(ROOT / "rtl" / f, WORK / f)
    for f in HEX:
        shutil.copy2(ROOT / f, WORK / f)
    shutil.copy2(HIL / "hil_main.cpp", WORK / "hil_main.cpp")

    env = dict(os.environ)
    env["PATH"] = f"{SHIM};{OSS / 'bin'};{OSS / 'lib'};{MINGW};{env['PATH']}"
    env["VERILATOR_ROOT"] = "E:/tools/oss-cad-suite/share/verilator"

    # 1) 只生成 C++ 模型（--public-flat-rw 暴露内部 PE 信号）
    print("[1/2] Verilator 生成 C++ ...")
    run([OSS / "bin" / "verilator_bin.exe", "--cc", "--exe", "-Wno-fatal",
         "--public-flat-rw", "--top-module", "self_healing_array", *RTL,
         "hil_main.cpp", "-CFLAGS", "-fno-declone-ctor-dtor", "-o", "hil"], env, "verilator")

    # 2) g++ 直接编译（不依赖 make/sh）
    print("[2/2] g++ 编译 ...")
    srcs = [WORK / "obj_dir" / "Vself_healing_array__ALL.cpp", WORK / "hil_main.cpp",
            INC / "verilated.cpp", INC / "verilated_threads.cpp"]
    cmd = [MINGW / "g++.exe", "-std=c++17", "-O1", "-fno-declone-ctor-dtor",
           # 复刻 verilated.mk 的必需宏（不加会出现 sc_time_stamp 未定义）
           "-DVM_COVERAGE=0", "-DVM_SC=0", "-DVM_TIMING=0", "-DVM_TRACE=0",
           "-DVM_TRACE_FST=0", "-DVM_TRACE_VCD=0", "-DVM_TRACE_SAIF=0", "-DVM_VPI=0",
           f"-I{WORK / 'obj_dir'}", f"-I{INC}", f"-I{INC / 'vltstd'}",
           *srcs, "-o", WORK / "obj_dir" / "hil.exe"]
    run(cmd, env, "g++")

    exe = WORK / "obj_dir" / "hil.exe"
    print(f"[ok] 产物: {exe}" if exe.exists() else "[FAIL] exe 未生成")
    return 0 if exe.exists() else 1


if __name__ == "__main__":
    sys.exit(main())
