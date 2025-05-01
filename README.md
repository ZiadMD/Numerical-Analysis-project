# Numerical Analysis Project

A Linux-optimized C++17 application showcasing fundamental numerical analysis algorithms, featuring both a Qt6 GUI and a CLI powered by GiNaC for symbolic math. Dive into root finding, polynomial interpolation, and numerical integration with interactive visuals or scriptable commands.

---

## 🚀 Features Overview

**Root Finding**  
• Bisection Method (bracketing)  
• Newton–Raphson Method (tangent-based)  
• Secant Method (finite-difference)  

**Polynomial Interpolation**  
• Newton’s Divided Differences  
• Lagrange Polynomial Construction  
• Newton Forward & Backward Interpolation  

**Numerical Integration**  
• Composite Trapezoidal Rule  
• Composite Simpson’s Rule  

**Interfaces**  
• **CLI:** Quick scriptable operations with customizable flags.  
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


---

# Root Methods

This section explains the algorithms and implementations of root-finding techniques:

### 1. Newton’s Method

Formula:
$$x_{n+1} = x_n - \frac{f(x_n)}{f'(x_n)}$$

```cpp
df_expr = diff(f_expr, x);
History.RootVariables['x'].push_back(findInitialGuess(bracket));

for (int i = 0; i < maxIterations; ++i) {
    fx  = ex_to<numeric>(f_expr.subs(x==x_i)).to_double();
    dfx = ex_to<numeric>(df_expr.subs(x==x_i)).to_double();
    History.RootVariables['x'].push_back(x_i - fx/dfx);

    if (matchDecimals(x_{i+1}, x_i, tol)) {
        History.Root = x_{i+1};
        break;
    }
}
```

---

### 2. Bisection Method

```cpp
History.RootVariables['a'] = {a};
History.RootVariables['b'] = {b};

for (int i = 0; i < maxIterations; ++i) {
    c = (a + b)/2;
    History.RootVariables['x'].push_back(c);
    fc = ex_to<numeric>(f_expr.subs(x==c)).to_double();

    if (matchDecimals(c, previous_c, tol)) {
        History.Root = c;
        return History;
    }

    if (fc < 0) a = c;
    else        b = c;
    History.RootVariables['a'].push_back(a);
    History.RootVariables['b'].push_back(b);
}
History.Root = History.RootVariables['x'].back();
```

---

### 3. Secant Method

```cpp
x0 = bracket.first;
x1 = bracket.second;
History.RootVariables['x'] = {x0, x1};

for (int i = 2; i < maxIterations; ++i) {
    f0 = f(x_{i-2});
    f1 = f(x_{i-1});
    x_new = x1 - f1*(x1 - x0)/(f1 - f0);
    History.RootVariables['x'].push_back(x_new);

    if (matchDecimals(x_new, x1, tol)) {
        History.Root = x_new;
        break;
    }
    x0 = x1;
    x1 = x_new;
}
```

---

# Interpolation Methods

### 1. Lagrange Interpolation

$$
P(x) = \sum_{k=0}^{n} y_k L_k(x),\quad
L_k(x) = \prod_{\substack{0 \le m \le n \\ m \neq k}} \frac{x - x_m}{x_k - x_m}
$$

```cpp
ex l_n = 1;
for (size_t m = 0; m < x.size(); ++m) {
    if (m == n) continue;
    l_n *= (sym - x[m]) / (x[n] - x[m]);
}

ex P_ex = 0;
for (size_t k = 0; k < y.size(); ++k) {
    P_ex += L_k(sym) * y[k];
}
double P_v = numeric(P_ex.subs(sym == x_));
```

---

### 2. Newton’s Divided-Difference Table

```cpp
History.D.push_back(y);
for (size_t k = 1; k < x.size(); ++k) {
    vector<double> d_k(x.size() - k);
    for (size_t i = 0; i < d_k.size(); ++i) {
        d_k[i] = (D[k-1][i+1] - D[k-1][i]) / (x[i+k] - x[i]);
    }
    History.D.push_back(d_k);
}
```

---

### 3. Newton Forward Interpolation

$$
P(x) = \sum_{k=0}^{n} D^{(k)}\_0 \prod\_{i=0}^{k-1} (x - x_i)
$$

```cpp
ex P_ex = 0;
for (size_t k = 0; k < D.size(); ++k) {
    ex coeff = History.D[k][0];
    ex term_expr = 1;
    for (size_t i = 0; i < k; ++i)
        term_expr *= (sym - x[i]);
    P_ex += coeff * term_expr;
}
```

---

### 4. Newton Backward Interpolation

$$
P(x) = \sum_{k=0}^{n} D^{(k)}\_{n-k} \prod_{i=0}^{k-1} (x - x\_{n-i})
$$

```cpp
ex P_ex = 0;
size_t n = x.size();
for (size_t k = 0; k < D.size(); ++k) {
    ex coeff = History.D[k].back();
    ex term_expr = 1;
    for (size_t i = 0; i < k; ++i)
        term_expr *= (sym - x[n-1-i]);
    P_ex += coeff * term_expr;
}
```

---

## 📝 TODO

- [ ] Add integration implementation documentation.
- [ ] Add Euler  implementation documentation.
- [ ] Add Curve fitting implementation documentation.
- [ ]  Add screenshots or example usage scenarios.

