#include "integrationmethods.h"

IntegrationResult StartingTable(const ex &f_expr, symbol x, double a, double b, int n){

    IntegrationResult Result;
    Result.h = (b - a) / n;

    for (int i = 0; i < n+1; ++i) {
        Result.X.push_back(a+(i*Result.h));
        Result.FX.push_back(ex_to<numeric>(f_expr.subs(x == Result.X.back())).to_double());
    }

    return Result;
}


IntegrationResult IntegrationMethods::trapezoidal(const ex &f_expr, symbol x, double a, double b, int n)
{
    IntegrationResult Result = StartingTable(f_expr, x, a, b, n);

    double Sum = 0;
    for(int i = 1; i < Result.FX.size()-1; ++i){
        Sum += Result.FX[i];
    }

    Result.I = (Result.h / 2) * (Result.FX.front() + Result.FX.back() + 2 * Sum);

    return Result;
}

IntegrationResult IntegrationMethods::simpsonOneThird(const ex &f_expr, symbol x, double a, double b, int n)
{
    IntegrationResult Result = StartingTable(f_expr, x, a, b, n);
    double Sum_odd = 0,
           Sum_even = 0;
    for(int i = 1; i < Result.FX.size()-1; ++i){
        Sum_even += (i % 2 == 0 ? Result.FX[i] : 0);
        Sum_odd += (i % 2 == 1 ? Result.FX[i] : 0);
    }

    Result.I = (Result.h / 3) * (Result.FX.front() + Result.FX.back() + (2 * Sum_even) + (4 * Sum_odd));

    return Result;
}

IntegrationResult IntegrationMethods::simpsonThreeEighth(const ex &f_expr, symbol x, double a, double b, int n)
{
    IntegrationResult Result = StartingTable(f_expr, x, a, b, n);
    double Sum_third = 0,
           Sum_norm = 0;
    for(int i = 1; i < Result.FX.size()-1; ++i){
        Sum_third += (i % 3 == 0 ? Result.FX[i] : 0);
        Sum_norm += (i % 3 != 0 ? Result.FX[i] : 0);
    }

    Result.I = ((3 *Result.h) / 8) * (Result.FX.front() + Result.FX.back() + (2 * Sum_third) + (3 * Sum_norm));

    return Result;
}
