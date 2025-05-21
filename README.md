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

### Concept

Newton’s method iteratively refines an initial guess to a root of $f(x)$ by:

$$x\_{n+1} = x\_n - \frac{f(x\_n)}{f'(x\_n)}$$

Convergence is quadratic near the root if $f'(x) \neq 0$.

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
### Concept

If $f(a)$ and $f(b)$ have opposite signs, a root lies in $[a, b]$. Repeatedly bisect the interval:

1. Compute midpoint $c = \frac{a + b}{2}$
    
2. Evaluate $f(c)$.
    
3. Replace `[a,b]` with `[a,c]` or `[c,b]` depending on sign.
    
4. Stop when midpoint converges to tolerance or max iterations.
    
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
### Concept

The secant method approximates the derivative by a finite difference using the two most recent iterates:

$$
x\_{n+1} = x\_n - \frac{f(x\_n)(x_n - x\_{n-1})}{f(x\_n)-f(x\_{n-1})}
$$
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

## 1. Lagrange Interpolation
### 1.1. Concept

Given $n+1$ data points $(x_0, y_0), \dots, (x_n, y_n)$, the Lagrange form of the interpolating polynomial is:

$$P(x) \;=\; \sum_{k=0}^{n} y\_k \, L\_k(x)$$

where each basis polynomial $L_k(x)$ is:

$$L\_k(x) \;=\; \prod_{\substack{0 \le m \le n \\ m \neq k}} \frac{x - x_m}{x_k - x_m}
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

## 2. Newton’s Divided‑Difference Table

### 2.1. Concept

The divided‑difference table stores coefficients for Newton’s form. The zeroth-order differences are the original $y-values$:

$$D^{(0)}_i = y_i,\quad i=0,\dots,n.$$

Higher orders are computed by:

$$D^{(k)}\_i = \frac{D^{(k-1)}\_{i+1} - D^{(k-1)}\_{i}}{x\_{i+k} - x\_i},$$

for $k=1,2,\dots,n$.

### 2.2. Implementation Highlights

```cpp
History.D.push_back(y);           // zeroth order
for (size_t k = 1; k < x.size(); ++k) {
    vector<double> d_k(x.size() - k);
    for (size_t i = 0; i < d_k.size(); ++i) {
        d_k[i] = (D[k-1][i+1] - D[k-1][i]) / (x[i+k] - x[i]);
    }
    History.D.push_back(d_k);
}
```

---

## 3. Newton Forward & Backward Interpolation

### 3.1. Forward Newton

The Newton forward polynomial is:

$$P(x)\;=\;\sum\_{k=0}^{n} D^{(k)}\_0 \;\prod\_{i=0}^{k-1} (x - x\_i)$$.

- **Coefficient**: `D[k][0]` (first element of each row)
    
- **Term**: product of $((x - x\_0)(x - x\_1)\dots(x - x\_{k-1})$`
    

**Code Snippet**:

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

### 3.2. Backward Newton

The backward form uses the last data point and backward differences:

$$P(x)\;=\;\sum\_{k=0}^{n} D^{(k)}\_{n-k} \;\prod_{i=0}^{k-1} (x - x\_{n-i})$$

- **Coefficient**: `D[k].back()` (last element of each row)
    
- **Term**: product of $((x - x\_n)(x - x\_{n-1})\dots(x - x\_{n-k+1})$`
    

**Code Snippet**:

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
# Integration Methods

This section explains the algorithms and implementations of numerical integration techniques:

### 1. Composite Trapezoidal Rule

#### Concept

Approximate the area under $f(x)$ on $[a,b]$ by dividing it into $n$ equal sub intervals of width $h=\frac{b-a}{n}$, then summing trapezoids:

$$I = \frac{h}{2}\Bigl[f(a) + f(b) + 2\sum\_{i=1}^{n-1} f\bigl(a + i*h\bigr)\Bigr].
$$
```cpp
IntegrationResult IntegrationMethods::trapezoidal(const ex &f_expr, symbol x, double a, double b, int n)
{
    IntegrationResult Result = StartingTable(f_expr, x, a, b, n);

    double Sum = 0;
    for(int i = 1; i < Result.FX.size()-1; ++i){
        Sum += Result.FX[i];
    }

    Result.I = (Result.h / 2) * (Result.FX.front() + Result.FX.back() + 2 * Sum);

    return Result;
}
```

---

### 2. Composite Simpson’s 1/3 Rule

#### Concept

Use parabolic arcs on pairs of sub intervals (requires even $n$):

$$I \approx \frac{h}{3} \left(f(x\_0) + 4 \sum\_{i=1, \text{odd}}^{n-1} f(x\_i) + 2 \sum\_{i=2, \text{even}}^{n-2} f(x\_i) + f(x\_n)\right)$$

```cpp
IntegrationResult IntegrationMethods::simpsonOneThird(const ex &f_expr, symbol x, double a, double b, int n)
{
    IntegrationResult Result = StartingTable(f_expr, x, a, b, n);
    double Sum_odd = 0,
           Sum_even = 0;
    for(int i = 1; i < Result.FX.size()-1; ++i){
        Sum_even += (i % 2 == 0 ? Result.FX[i] : 0);
        Sum_odd += (i % 2 == 1 ? Result.FX[i] : 0);
    }

    Result.I = (Result.h / 3) * (Result.FX.front() + Result.FX.back() + (2 * Sum_even) + (4 * Sum_odd));

    return Result;
}
``` 

---
### 3. Composite Simpson’s 3/8 Rule 
#### Concept
Divide interval into $n$ subintervals where $n$ is a multiple of 3, width $h=\frac{b-a}{n}$:  = 

$$I \approx \frac{3h}{8} \left(f(x\_0) + 3 \sum\_{\text{not multiple of 3}} f(x\_i) + 2 \sum\_{\text{multiple of 3}} f(x\_i) + f(x\_n)\right)
$$

```cpp
IntegrationResult IntegrationMethods::simpsonThreeEighth(const ex &f_expr, symbol x, double a, double b, int n)
{
    IntegrationResult Result = StartingTable(f_expr, x, a, b, n);
    double Sum_third = 0,
           Sum_norm = 0;
    for(int i = 1; i < Result.FX.size()-1; ++i){
        Sum_third += (i % 3 == 0 ? Result.FX[i] : 0);
        Sum_norm += (i % 3 != 0 ? Result.FX[i] : 0);
    }

    Result.I = ((3 *Result.h) / 8) * (Result.FX.front() + Result.FX.back() + (2 * Sum_third) + (3 * Sum_norm));

    return Result;
}
```

---

# Euler Methods

This section explains the algorithms and implementations of numerical methods for solving ordinary differential equations (ODEs):

## 1. Euler's Method

### Concept

For a first-order ODE of the form $\frac{dy}{dx} = f(x,y)$ with initial condition $y(x_0) = y_0$, Euler's method approximates the solution using:

$$y_{n+1} = y_n + h \cdot f(x_n, y_n)$$

where $h$ is the step size.

```cpp
EulerResult EulerMethods::Euler(const ex &fxy, symbol x, symbol y, double x0, double y0, double x_, double h)
{
    EulerResult R;
    R.Y.push_back(y0);
    int steps = (x_ - x0) / h + 1;
    for (int i = 0; i < steps; i++) {
        R.X.push_back(x0 + i*h);
    }

    for (int i = 1; i < size; ++i) {
        double Fxy_v = eval_xy(fxy, x, y, R.X[i-1], R.Y[i-1]);
        R.Fxy.push_back(Fxy_v);
        R.Y.push_back(R.Y[i-1] + (h * Fxy_v));
    }
    return R;
}
```

## 2. Modified Euler's Method (Heun's Method)

### Concept
 $$y_{n+1}^* = y_n + h \cdot f(x_n, y_n)$$ $$y_{n+1} = y_n + \frac{h}{2}[f(x_n, y_n) + f(x_{n+1}, y_{n+1}^*)]$$
```cpp
EulerResult EulerMethods::ModifiedEuler(const ex &fxy, symbol x, symbol y, double x0, double y0, double x_, double h)
{
    EulerResult R;
    R.Y.push_back(y0);

    for (double i = x0; i <= x_; i += h) {
        R.X.push_back(i);
    }

    for (int i = 1; i < size; ++i) {
        // Predictor
        double Fxy_v = eval_xy(fxy, x, y, R.X[i-1], R.Y.back());
        double yn_n1 = R.Y.back() + (h * Fxy_v);
        
        // Corrector
        double Fxyn1_v = eval_xy(fxy, x, y, R.X[i], yn_n1);
        double yn1_n1 = R.Y.back() + (h/2) * (Fxy_v + Fxyn1_v);

        R.Fxy.push_back(Fxy_v);
        R.Fxy_P.push_back(Fxyn1_v);
        R.Y_P.push_back(yn_n1);
        R.Y.push_back(yn1_n1);
    }
    return R;
}
```

---

# Curve Fitting Methods

This section explains the algorithms and implementations of curve fitting techniques:

## 1. Linear Regression

### Concept

Fits data to the model $y = ax + b$ using the method of least squares. The normal equations are:

$$\begin{align*}
\sum y &= a\sum x + nb \\
\sum xy &= a\sum x^2 + b\sum x
\end{align*}$$

```cpp
CurveResult CurveFitting::linear(const ex &c_x,const ex &c_y,const vector<double> &x,const vector<double> &y,symbol xs,symbol ys)
{
    Matrix A = Matrix({{CR.sum_X, double(n)},
                       {CR.sum_X2, CR.sum_X}});
    vector<double> Sy = {CR.sum_Y}, Sxy = {CR.sum_XY};
    Matrix B = Matrix({Sy,
                       Sxy});

    Matrix A_inv = A.inverse();
    Matrix X = A_inv * B;

    CR.a = X(0,0);
    CR.b = X(1,0);
    return CR;
}
```

## 2. Quadratic Regression

### Concept

Fits data to the model $y = ax^2 + bx + c$ using the method of least squares. The normal equations are:

$$\begin{align*}
\sum y &= a\sum x^2 + b\sum x + nc \\
\sum xy &= a\sum x^3 + b\sum x^2 + c\sum x \\
\sum x^2y &= a\sum x^4 + b\sum x^3 + c\sum x^2
\end{align*}$$

```cpp
CurveResult CurveFitting::quadric(const ex &c_x, const ex &c_y, const vector<double> &x, const vector<double> &y, symbol xs, symbol ys)
{
    Matrix A = Matrix({{CR.sum_X2, CR.sum_X, double(n)},
                      {CR.sum_X3, CR.sum_X2, CR.sum_X},
                      {CR.sum_X4, CR.sum_X3, CR.sum_X2}});

    Matrix B = Matrix({{CR.sum_Y},
                      {CR.sum_XY},
                      {CR.sum_X2Y}});

    Matrix X = A.inverse() * B;
    CR.a = X(0,0);
    CR.b = X(1,0);
    CR.c = X(2,0);
    return CR;
}
```

## 3. Exponential Regression

### Concept

Fits data to the model $y = ae^{bx}$ by linearizing to $\ln(y) = \ln(a) + bx$. Let $Y = \ln(y)$ and $A = \ln(a)$, then solve:

$$\begin{align*}
\sum Y &= nA + b\sum x \\
\sum xY &= A\sum x + b\sum x^2
\end{align*}$$

## 4. Power Regression

### Concept

Fits data to the model $y = ax^b$ by linearizing to $\ln(y) = \ln(a) + b\ln(x)$.
Let:
$Y = \ln(y)$, $X = \ln(x)$, and $A = \ln(a)$, then solve:

$$\begin{align*}
\sum Y &= nA + b\sum X \\
\sum XY &= A\sum X + b\sum X^2
\end{align*}$$


Fits data to the model $y = b a^x$ by linearizing to $\ln(y) = \ln(b) + x\ln(a)$. 

Let:
$Y = \ln(y)$, $X = x$, and $A = \ln(a)$,  $B = ln(b)$,  then solve:
$$\begin{align*}
\sum Y &= nB + A\sum X \\
\sum XY &= B\sum X + A\sum X^2
\end{align*}$$


