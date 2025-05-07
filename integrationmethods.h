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
    IntegrationResult trapezoidal(const ex &fx, symbol x, double a, double b, int n = 100);
    IntegrationResult simpsonOneThird(const ex &f_expr, symbol x, double a, double b, int n = 100);
    IntegrationResult simpsonThreeEighth(const ex &f_expr, symbol x, double a, double b, int n = 99);
};

#endif // INTEGRATIONMETHODS_H
