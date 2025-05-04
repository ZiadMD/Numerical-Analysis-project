#ifndef INTEGRATIONMETHODS_H
#define INTEGRATIONMETHODS_H

#include <ginac/ginac.h>

using namespace std;
using namespace GiNaC;
struct IntegrationReturn
{
    vector<double> X;
    vector<double> FX;
    ex fx;
    double h;
    double I;
};
class IntegrationMethods
{
    ex str2ex(const string &fx);
public:
    IntegrationReturn trapezoidal(const string &fx, symbol x, double a, double b, int n = 100);
    IntegrationReturn simpsonOneThird(const string &f_expr, symbol x, double a, double b, int n = 100);
    IntegrationReturn simpsonThreeEighth(const string &f_expr, symbol x, double a, double b, int n = 99);
};

#endif // INTEGRATIONMETHODS_H
