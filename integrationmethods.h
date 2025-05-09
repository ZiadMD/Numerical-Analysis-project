#ifndef INTEGRATIONMETHODS_H
#define INTEGRATIONMETHODS_H

#include <ginac/ginac.h>

using namespace std;
using namespace GiNaC;

struct IntegrationResult
{
    vector<double> X;
    vector<double> FX;
    double h;
    double I;
};

class IntegrationMethods
{
public:
    /**
     * Compute integral using the composite trapezoidal rule:
     * I = (h/2) * [y0 + yn + 2 * (y1 + y2 + ... + yn-1)]
     *
     * @param fExpr   Symbolic expression of f(x)
     * @param xSym    Symbol for x
     * @param a       Lower limit
     * @param b       Upper limit
     * @param n       Number of subintervals
     * @return        IntegrationResult with nodes, values, stepSize, integral
     */
    IntegrationResult trapezoidal(const ex &fx, symbol x, double a, double b, int n);

    /**
     * Compute integral using Simpson's 1/3 rule (n must be even):
     * I = h/3 * [y0 + yn + 2 * (y2 + y4 + ...) + 4 * (y1 + y3 + ...)]
     *
     * @param fExpr   Symbolic expression of f(x)
     * @param xSym    Symbol for x
     * @param a       Lower limit
     * @param b       Upper limit
     * @param n       Number of subintervals
     * @return        IntegrationResult with nodes, values, stepSize, integral
     */
    IntegrationResult simpsonOneThird(const ex &f_expr, symbol x, double a, double b, int n);

    /**
     * Compute integral using Simpson's 3/8 rule (n mod 3 == 0):
     * I = 3*h/8 * [y0 + yn + 3 * (y1 + y2 + y4 + ...) + 2 * (y3 + y6 + ...)]
     *
     * @param fExpr   Symbolic expression of f(x)
     * @param xSym    Symbol for x
     * @param a       Lower limit
     * @param b       Upper limit
     * @param n       Number of subintervals
     * @return        IntegrationResult with nodes, values, stepSize, integral
     */
    IntegrationResult simpsonThreeEighth(const ex &f_expr, symbol x, double a, double b, int n);
};

#endif // INTEGRATIONMETHODS_H
