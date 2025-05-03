#include "integrationmethods.h"
#include <iostream>

using namespace GiNaC;
using namespace std;

numeric IntegrationMethods::trapezoidal(const ex &f_expr, symbol x, double a, double b, int n)
{
    if (n < 1)
        n = 1;
    double h = (b - a) / n;
    numeric result = 0.5
                     * (ex_to<numeric>(f_expr.subs(x == a)) + ex_to<numeric>(f_expr.subs(x == b)));

    for (int i = 1; i < n; i++) {
        double xi = a + i * h;
        result += ex_to<numeric>(f_expr.subs(x == xi));
    }

    return result * h;
}

numeric IntegrationMethods::simpsonOneThird(const ex &f_expr, symbol x, double a, double b, int n)
{
    if (n % 2 != 0)
        n++; // n must be even
    double h = (b - a) / n;
    numeric result = ex_to<numeric>(f_expr.subs(x == a)) + ex_to<numeric>(f_expr.subs(x == b));

    for (int i = 1; i < n; i++) {
        double xi = a + i * h;
        int coeff = (i % 2 == 0) ? 2 : 4;
        result += coeff * ex_to<numeric>(f_expr.subs(x == xi));
    }

    return result * (h / 3.0);
}

numeric IntegrationMethods::simpsonThreeEighth(const ex &f_expr, symbol x, double a, double b, int n)
{
    if (n % 3 != 0)
        n += 3 - (n % 3); // n must be multiple of 3
    double h = (b - a) / n;
    numeric result = ex_to<numeric>(f_expr.subs(x == a)) + ex_to<numeric>(f_expr.subs(x == b));

    for (int i = 1; i < n; i++) {
        double xi = a + i * h;
        int coeff = (i % 3 == 0) ? 2 : 3;
        result += coeff * ex_to<numeric>(f_expr.subs(x == xi));
    }

    return result * (3.0 * h / 8.0);
}
