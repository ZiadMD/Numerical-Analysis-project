# Numerical Analysis Project

A cross-platform C++17 application showcasing fundamental numerical analysis algorithms, featuring both a Qt6 GUI and a CLI powered by GiNaC for symbolic math. Dive into root finding, polynomial interpolation, and numerical integration with interactive visuals or scriptable commands.

---

## 🚀 Features Overview

**Root Finding**  
• Bisection Method (bracketing)  
• Newton–Raphson Method (tangent-based)  
• Secant Method (finite-difference)

**Polynomial Interpolation**  
• Newton’s Divided Differences  
• Lagrange Polynomial Construction

**Numerical Integration**  
• Composite Trapezoidal Rule  
• Composite Simpson’s Rule

**Interfaces**  
• **CLI:** Quick scriptable operations with customizable flags.  
• **Qt6 GUI:** Interactive forms, result tables, and real-time plotting.

---

## 🔧 Prerequisites

- **C++17** (GCC ≥ 9, Clang ≥ 10, or MSVC 2019+)  
- **CMake** ≥ 3.19  
- **Qt6** ≥ 6.5 (Core & Widgets modules)  
- **GiNaC** (symbolic math via pkg-config)  
- **pkg-config** (for locating GiNaC)

Ensure all dependencies are installed and accessible via your system’s package manager or build from source.

---

## 🏗️ Build Instructions

1. **Clone the repo**  
   ```bash
   git clone https://github.com/ZiadMD/Numerical-Analysis-project.git
   cd Numerical-Analysis-project
