#ifndef INTERPOLATIONMETHODS_H
#define INTERPOLATIONMETHODS_H

#include <ginac/ginac.h>
#include <vector>

using namespace std;
using namespace GiNaC;

struct InterpolationResult{
    vector<pair<ex, double>> L; // Lagranch L experions, value
    vector<vector<double>> D; // For Newton Methods
    pair<ex, double> P;
};

class InterpolationMethods
{
public:
    InterpolationResult lagrangeInterpolation(const vector<double> &x, const std::vector<double> &y, const double &x_, const symbol &sym);

    InterpolationResult newtonForwardInterpolation(const vector<double> &x, const std::vector<double> &y, const double &x_, const symbol &sym);

    InterpolationResult newtonBackwardInterpolation(const vector<double> &x, const std::vector<double> &y, const double &x_, const symbol &sym);
private:

    InterpolationResult NewtonTable(const vector<double> &x, const std::vector<double> &y);
};

#endif // INTERPOLATIONMETHODS_H
