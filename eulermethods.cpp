#include "eulermethods.h"


double eval_xy(const ex &fxy, symbol x, symbol y, double x_, double y_){
    return ex_to<numeric>(fxy.subs(lst{x == x_, y == y_}.evalf())).to_double();
}

EulerResult EulerMethods::Euler(const ex &fxy, symbol x, symbol y, double x0, double y0, double x_, double h)
{
    EulerResult R;
    R.Y.push_back(y0);

    for (double i = x0; i <= x_; i += h) {
        R.X.push_back(i);
    }

    int size = R.X.size();

    for (int i = 1; i < size; ++i) {
        double Fxy_v = eval_xy(fxy, x, y, R.X[i-1], R.Y[i-1]);
        R.Fxy.push_back(Fxy_v);
        R.Y.push_back(R.Y[i-1] + (h * Fxy_v));
    }

    return R;
}

EulerResult EulerMethods::Euler(const ex &fxy, symbol x, symbol y, double x0, double y0, pair<double, double> x_, double h)
{
    EulerResult R;
    R.Y.push_back(y0);
    R.X.push_back(x0);

    for (double i = x_.first; i <= x_.second; i += h) {
        R.X.push_back(i);
    }

    int size = R.X.size();

    for (int i = 1; i < size; ++i) {
        double Fxy_v = eval_xy(fxy, x, y, R.X[i-1], R.Y[i-1]);
        R.Fxy.push_back(Fxy_v);
        R.Y.push_back(R.Y[i-1] + (h * Fxy_v));
    }

    return R;
}

EulerResult EulerMethods::ModifiedEuler(const ex &fxy, symbol x, symbol y, double x0, double y0, double x_, double h)
{
    EulerResult R;
    R.Y.push_back(y0);

    for (double i = x0; i <= x_; i += h) {
        R.X.push_back(i);
    }

    int size = R.X.size();

    for (int i = 1; i < size; ++i) {
        double Fxy_v = eval_xy(fxy, x, y, R.X[i-1], R.Y[i-1]);
        R.Fxy.push_back(Fxy_v);
        R.Y.push_back(R.Y[i-1] + (h * Fxy_v));
    }

    for (int i = 1; i < size; ++i) {
        double Fxyp_v = eval_xy(fxy, x, y, R.X[i], R.Fxy[i]);
        R.Fxy_P.push_back(Fxyp_v);
        R.Y_P.push_back(R.Y[i-1] + ((h/2) * (R.Fxy[i] + Fxyp_v)));
    }

    return R;
}

EulerResult EulerMethods::ModifiedEuler(const ex &fxy, symbol x, symbol y, double x0, double y0, pair<double, double> x_, double h)
{
    EulerResult R;
    R.Y.push_back(y0);

    for (double i = x0; i <= x_.second; i += h) {
        R.X.push_back(i);
    }

    int size = R.X.size();

    for (int i = 1; i < size; ++i) {
        double Fxy_v = eval_xy(fxy, x, y, R.X[i-1], R.Y[i-1]);
        R.Fxy.push_back(Fxy_v);
        R.Y.push_back(R.Y[i-1] + (h * Fxy_v));
    }

    for (int i = 1; i < size; ++i) {
        double Fxyp_v = eval_xy(fxy, x, y, R.X[i], R.Fxy[i]);
        R.Fxy_P.push_back(Fxyp_v);
        R.Y_P.push_back(R.Y[i-1] + ((h/2) * (R.Fxy[i] + Fxyp_v)));
    }

    return R;
}
