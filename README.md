# Numerical Analysis Project

A Linux-optimized C++17 application showcasing fundamental numerical analysis algorithms, featuring both a Qt6 GUI and a CLI powered by GiNaC for symbolic math. Dive into root finding, polynomial interpolation, and numerical integration with interactive visuals or scriptable commands.

---

## 🚀 Features Overview

**Root Finding**\
• Bisection Method (bracketing)\
• Newton–Raphson Method (tangent-based)\
• Secant Method (finite-difference)

**Polynomial Interpolation**\
• Newton’s Divided Differences\
• Lagrange Polynomial Construction

**Numerical Integration**\
• Composite Trapezoidal Rule\
• Composite Simpson’s Rule

**Interfaces**\
• **CLI:** Quick scriptable operations with customizable flags.\
• **Qt6 GUI:** Interactive forms, result tables, and real-time plotting.

---

## 🔧 System & Prerequisites

> **Platform:** Primarily tested on Linux (Ubuntu 20.04+ / Fedora). Other POSIX systems may work with minor tweaks.

- **C++17** (GCC ≥ 9, Clang ≥ 10)
- **CMake** ≥ 3.19
- **Qt6** ≥ 6.5 (Core & Widgets modules)
- **GiNaC** (symbolic math via pkg-config)
- **pkg-config** (for locating GiNaC)

### Installing GiNaC on Ubuntu/Debian

```bash
sudo apt update
sudo apt install libginac-dev pkg-config
```

### Installing GiNaC on Fedora/RHEL

```bash
sudo dnf install ginac-devel pkgconfig
```

Ensure all dependencies are installed and accessible via your package manager or built from source.

---

## 🏗️ Build Instructions

1. **Clone the repo**
   ```bash
   git clone https://github.com/ZiadMD/Numerical-Analysis-project.git
   cd Numerical-Analysis-project
   ```
2. **Create & enter build directory**
   ```bash
   mkdir build && cd build
   ```
3. **Configure project**
   ```bash
   cmake ..
   ```
4. **Compile & install**
   ```bash
   cmake --build . -- -j$(nproc)
   sudo cmake --install .
   ```

Binaries will be installed under `/usr/local/bin/` (by default).

---

## ⚙️ Usage Guide


### 1. Qt6 Graphical Interface (GUI)

1. **Launch the GUI**
   ```bash
   Numerical_AnalysisGUI
   ```
2. **Select Operation**\
   Choose **Root Finding**, **Interpolation**, or **Integration** from the sidebar.
3. **Input Parameters**
   - Enter the function or data points.
   - Set interval (`a`, `b`), subdivisions (`n`), or tolerance (`tol`).
4. **Compute & Visualize**\
   Click **Compute** to display:
   - Numeric results in a table.
   - Real-time plot of function behavior or interpolation polynomial.

#### Sample GUI Layout



---

## 📝 TODO

-

---

## 🤝 Contributing

1. **Fork** & **clone** the project.
2. Create a **feature branch**: `git checkout -b feature/XYZ`.
3. **Commit** changes & run `clang-format`.
4. Push & open a **Pull Request**.

Include unit tests and update documentation for new features.


