#include "interpolationmethods.h"

InterpolationResult InterpolationMethods::lagrangeInterpolation(const vector<double> &x, const std::vector<double> &y, const double &x_, const symbol &sym)
{
    InterpolationResult History;
    for (int n = 0; n < x.size(); ++n) {
        ex l_n = 1;
        for (int m = 0; m < x.size(); ++m) {
            if(n == m) continue;
            l_n *= (sym - x[m]) / (x[n] - x[m]);
        }
        double l_nx = ex_to<numeric>(l_n.subs(sym == x_)).to_double();
        History.L.push_back(make_pair(l_n, l_nx));
    }

    ex P_ex;
    for (int n = 0; n < y.size(); ++n) {
        P_ex += History.L[n].first * y[n];
    }

    double P_v = ex_to<numeric>(P_ex.subs(sym == x_)).to_double();

    History.P = make_pair(P_ex, P_v);

    History.D = {};

    return History;
}

InterpolationResult InterpolationMethods::newtonForwardInterpolation(const vector<double> &x, const std::vector<double> &y, const double &x_, const symbol &sym)
{
    InterpolationResult History = NewtonTable(x,y);

    ex P_ex = 0;
    for (int term = 0; term < History.D.size(); ++term) {
        ex temp = 1;

        for (int i = 0; i < term; ++i) {
            temp *= (sym - x[i]);
        }
        P_ex += numeric(History.D[term].front()) * temp;
    }

    double P_v = ex_to<numeric>(P_ex.subs(sym == x_)).to_double();

    History.P = make_pair(P_ex, P_v);
    History.L = {};

    return History;
}

InterpolationResult InterpolationMethods::newtonBackwardInterpolation(const vector<double> &x, const std::vector<double> &y, const double &x_, const symbol &sym)
{
    InterpolationResult History = NewtonTable(x,y);
    ex P_ex;
    for (int term = 0; term < History.D.size(); ++term) {
        ex temp = 1;

        for (int i = 0; i < term; ++i) {
            temp *= (sym - x[x.size()-1-i]);
        }
        P_ex += numeric(History.D[term].back()) * temp;
    }

    double P_v = ex_to<numeric>(P_ex.subs(sym == x_)).to_double();

    History.P = make_pair(P_ex, P_v);

    return History;
}

InterpolationResult InterpolationMethods::NewtonTable(const vector<double> &x, const std::vector<double> &y)
{
    InterpolationResult History;
    History.D.push_back(y);
    for (int n = 1; n < x.size(); ++n) {
        vector<double> dn(x.size() - n);
        for (int m = 0; m < x.size() - n; ++m) {
            dn[m] = (History.D[n-1][m+1] - History.D[n-1][m])/(x[n+m] - x[m]);
        }
        History.D.push_back(dn);
    }
    return History;
}
