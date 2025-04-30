#ifndef INTERPOLATIONMETHODS_H
#define INTERPOLATIONMETHODS_H

#include <vector>
#include <ginac/ginac.h>

using namespace std;
using namespace GiNaC;

class InterpolationMethods
{
public:
    ex lagrangeInterpolation(const vector< numeric> &x, const std::vector< numeric> &y, const  symbol &sym);

    ex newtonForwardInterpolation(const vector< numeric> &x, const std::vector< numeric> &y, const  symbol &sym);

    ex newtonBackwardInterpolation(const vector< numeric> &x, const std::vector< numeric> &y, const  symbol &sym);
};

#endif // INTERPOLATIONMETHODS_H
