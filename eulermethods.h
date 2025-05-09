#ifndef EULERMETHODS_H
#define EULERMETHODS_H

#include <ginac/ginac.h>

using namespace std;
using namespace GiNaC;

struct EulerResult{

    vector<double> X;
    vector<double> Y;
    vector<double> Y_P; // Modified Euler Only

    double h;

    vector<double> Fxy;
    vector<double> Fxy_P; // Modified Euler Only
};

class EulerMethods
{

public:
    EulerResult Euler(const ex &fxy, symbol x, symbol y, double x0, double y0, double x_, double h);
    EulerResult Euler(const ex &fxy, symbol x, symbol y, double x0, double y0, pair<double, double> x_, double h);

    EulerResult ModifiedEuler(const ex &fxy, symbol x, symbol y, double x0, double y0, double x_, double h);
    EulerResult ModifiedEuler(const ex &fxy, symbol x, symbol y, double x0, double y0, pair<double, double> x_, double h);

};

#endif // EULERMETHODS_H
