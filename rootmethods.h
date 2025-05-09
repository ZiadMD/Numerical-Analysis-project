#ifndef ROOTMETHODS_H
#define ROOTMETHODS_H

#include <cmath>
#include <ginac/ginac.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <utility>
#include <vector>

// #include "method.h"

using namespace std;
using namespace GiNaC;

struct RootResult
{
    map<char, vector<double>> RootVariables;
    double Root;
};

class RootMethods
{
public:
    // —————— PARSER SETUP ——————
    static parser make_full_parser(const symbol &x);

    // —————— UTILITY ——————
    bool matchDecimals(double a, double b, double tol = 1e-6);

    pair<double, double> findBracket(const ex &f_expr, symbol x, double start, double end, double step = 1);

    double findInitialGuess(const pair<double, double> &bracket); // mid point

    // —————— ROOT-FINDERS ——————
    RootResult newton(const ex &f_expr,
                            symbol x,
                            pair<double, double> &bracket,
                            double tol,
                            int maxIterations = 100);

    RootResult bisection(const ex &f_expr,
                               symbol x,
                               pair<double, double> &bracket,
                               double tol,
                               int maxIterations = 100);

    RootResult secant(const ex &f_expr,
                            symbol x,
                            pair<double, double> &bracket,
                            double tol,
                            int maxIterations = 100);
};

#endif // ROOTMETHODS_H
