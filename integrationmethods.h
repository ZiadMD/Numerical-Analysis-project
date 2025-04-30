#ifndef INTEGRATIONMETHODS_H
#define INTEGRATIONMETHODS_H

#include <ginac/ginac.h>

using namespace std;
using namespace GiNaC;

class IntegrationMethods
{
public:
    numeric trapezoidal(const ex &f_expr, symbol x, double a, double b, int n = 100);
    numeric simpsonOneThird(const ex &f_expr, symbol x, double a, double b, int n = 100);
    numeric simpsonThreeEighth(const ex &f_expr, symbol x, double a, double b, int n = 99);
};

#endif // INTEGRATIONMETHODS_H
