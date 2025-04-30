#include "interpolationmethods.h"
#include <iostream>
#include <cmath>

using namespace GiNaC;
using namespace std;

ex InterpolationMethods::lagrangeInterpolation(const vector<numeric> &x, const vector<numeric> &y, const symbol &sym)
{
    int n = x.size();
    ex result = 0;

    for (int i = 0; i < n; i++)
    {
        ex term = y[i];
        for (int j = 0; j < n; j++)
        {
            if (j != i)
                term *= (sym - x[j]) / (x[i] - x[j]);
        }
        result += term;
    }

    return result.expand();
}

ex InterpolationMethods::newtonForwardInterpolation(const vector<numeric> &x, const vector<numeric> &y, const symbol &sym)
{
    int n = x.size();
    vector<vector<ex>> diff(n, vector<ex>(n));

    for (int i = 0; i < n; i++)
        diff[i][0] = y[i];

    for (int j = 1; j < n; j++)
        for (int i = 0; i < n - j; i++)
            diff[i][j] = diff[i + 1][j - 1] - diff[i][j - 1];

    ex h = x[1] - x[0];
    ex u = (sym - x[0]) / h;
    ex result = diff[0][0];
    ex u_term = 1;

    for (int i = 1; i < n; i++)
    {
        u_term *= (u - (i - 1));
        result += (u_term * diff[0][i]) / factorial(i);
    }

    return result.expand();
}

ex InterpolationMethods::newtonBackwardInterpolation(const vector<numeric> &x, const vector<numeric> &y, const symbol &sym)
{
    int n = x.size();
    vector<vector<ex>> diff(n, vector<ex>(n));

    for (int i = 0; i < n; i++)
        diff[i][0] = y[i];

    for (int j = 1; j < n; j++)
        for (int i = n - 1; i >= j; i--)
            diff[i][j] = diff[i][j - 1] - diff[i - 1][j - 1];

    ex h = x[1] - x[0];
    ex u = (sym - x[n - 1]) / h;
    ex result = diff[n - 1][0];
    ex u_term = 1;

    for (int i = 1; i < n; i++)
    {
        u_term *= (u + (i - 1));
        result += (u_term * diff[n - 1][i]) / factorial(i);
    }

    return result.expand();
}
