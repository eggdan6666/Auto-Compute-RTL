# Contributing to Auto-Compute-RTL

Thank you for your interest in contributing to **Auto-Compute-RTL**! We welcome contributions to RTL architectures, formal verification assertions, Python co-simulation models, ASIC physical synthesis flows, and benchmark datasets.

---

## 🛠️ Development Workflow

1. **Fork & Branch**:
   - Fork the repository on GitHub.
   - Create a feature branch: `git checkout -b feature/your-feature-name`.

2. **Coding Standards**:
   - **SystemVerilog**: Follow IEEE 1800-2012 / 1800-2017 standards. Ensure fully synthesizable code without implicit latches (`No latch inferred`).
   - **Formal Assertions**: Pair critical RTL control logic and data handshakes with SystemVerilog Assertions (`*_sva.sv`).
   - **Fixed-Point Numerics**: Adhere strictly to 16-bit Q1.15 fixed-point conventions with RNE (Round-to-Nearest-Even) and saturation clipping.
   - **Python Models**: Ensure bit-exact alignment with hardware RTL or explicitly document intentional abstraction bounds.

3. **Verification & Regression**:
   Before submitting a Pull Request, run the complete regression test suite:
   ```bash
   # On Linux / SCNet:
   bash sim/run_sim_linux.sh

   # On Windows (Git Bash with OSS CAD Suite):
   bash sim/run_sim.sh
   ```
   Ensure all testbenches pass (`RUN_SIM_EXIT=0`).

4. **Pull Request Checklist**:
   - [ ] RTL compiles cleanly in both Icarus Verilog (`iverilog`) and Verilator (`verilator_bin`).
   - [ ] No inferred latches in Yosys synthesis check (`yosys`).
   - [ ] Testbenches updated and matching regression logs provided.
   - [ ] Clear commit messages following [Conventional Commits](https://www.conventionalcommits.org/) format (e.g., `feat:`, `fix:`, `docs:`, `test:`).

---

## 📜 Code of Conduct

We are committed to providing a friendly, safe, and welcoming environment for all contributors, regardless of background or level of experience. Please treat all community members with respect and kindness.

---

## 📄 License

By contributing to Auto-Compute-RTL, you agree that your contributions will be licensed under its **Apache License 2.0**.
