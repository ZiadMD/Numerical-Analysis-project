#ifndef CURVEFITTING_H
#define CURVEFITTING_H

#include <ginac/ginac.h>
#include <cmath>
using namespace std;
using namespace GiNaC;

struct CurveResult{
    vector<double> x, X;
    vector<double> y, Y;

    // Table vectors
    vector<double> XY, X2Y, X2, X3, X4;

    // Sum of the table
    double sum_X, sum_Y,sum_XY, sum_X2Y, sum_X2, sum_X3, sum_X4;

    double a, A;
    double b, B;
    double c;
};

class CurveFitting
{
public:
    CurveResult linear(const ex &c_x, const ex &c_y, const vector<double> &x, const vector<double> &y);
    CurveResult quadric(const ex &c_x, const ex &c_y, const vector<double> &x, const vector<double> &y);
    CurveResult power1(const ex &c_x, const ex &c_y, const vector<double> &x, const vector<double> &y);
    CurveResult power2(const ex &c_x, const ex &c_y, const vector<double> &x, const vector<double> &y);
    CurveResult exponential(const ex &c_x, const ex &c_y, const vector<double> &x, const vector<double> &y);



};

#endif // CURVEFITTING_H
