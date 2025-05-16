#include "curvefitting.h"


CurveResult CurveFitting::linear(const ex &c_x,const ex &c_y,const vector<double> &x,const vector<double> &y,symbol xs,symbol ys)
{
    CurveResult CR;
    CR.sum_X = CR.sum_Y = CR.sum_XY = CR.sum_X2 = 0;
    int n = x.size();

    for (int i = 0; i < n; ++i) {
        double cx, cy;

        try {
            cx = ex_to<numeric>(c_x.subs(xs == x[i])).to_double();
        } catch (...) {
            cerr << "could not resolve cx\n";
            return CR;
        }

        try {
            cy = ex_to<numeric>(c_y.subs(ys == y[i])).to_double();
        } catch (...) {
            cerr << "could not resolve cy\n";
            return CR;
        }

        CR.X.push_back(cx);
        CR.Y.push_back(cy);

        CR.XY.push_back(CR.X[i] * CR.Y[i]);
        CR.X2.push_back(CR.X[i] * CR.X[i]);

        CR.sum_X += CR.X[i];
        CR.sum_Y += CR.Y[i];
        CR.sum_XY += CR.XY[i];
        CR.sum_X2 += CR.X2[i];
    }

    // CR.a = (CR.sum_Y * CR.sum_X - n * CR.sum_XY) / CR.sum_X * CR.sum_X - n * CR.sum_X;
    // CR.b = (CR.sum_XY * CR.sum_X - CR.sum_Y * CR.sum_X2) / CR.sum_X * CR.sum_X - n * CR.sum_X;
    CR.a = (n * CR.sum_XY - CR.sum_X * CR.sum_Y)/(n * CR.sum_X2 - (CR.sum_X * CR.sum_X));
    CR.b = (CR.sum_Y - CR.a * CR.sum_X)/(n);

    return CR;
}

CurveResult CurveFitting::quadric(const ex &c_x, const ex &c_y, const vector<double> &x, const vector<double> &y, symbol xs, symbol ys)
{
    CurveResult CR;
    CR.sum_X = CR.sum_Y = CR.sum_XY = CR.sum_X2 = CR.sum_X2Y= CR.sum_X3 = CR.sum_X4= 0;
    int n = x.size();

    for (int i = 0; i < n; ++i) {
        CR.X.push_back(ex_to<numeric>(c_x.subs(xs == x[i])).to_double());
        CR.Y.push_back(ex_to<numeric>(c_y.subs(ys == y[i])).to_double());

        CR.XY.push_back(CR.X[i] * CR.Y[i]);
        CR.X2.push_back(CR.X[i] * CR.X[i]);

        CR.X2Y.push_back(CR.X2[i] * CR.Y[i]);
        CR.X3.push_back(CR.X2[i] * CR.X[i]);
        CR.X4.push_back(CR.X2[i] * CR.X2[i]);

        CR.sum_X += CR.X[i];
        CR.sum_Y += CR.Y[i];
        CR.sum_XY += CR.XY[i];
        CR.sum_X2 += CR.X2[i];
        CR.sum_X2Y += CR.X2Y[i];
        CR.sum_X3 += CR.X3[i];
        CR.sum_X4 += CR.X4[i];
    }

    double S0   = n;             // n
    double S1   = CR.sum_X;      // Σx
    double S2   = CR.sum_X2;     // Σx²
    double S3   = CR.sum_X3;     // Σx³
    double S4   = CR.sum_X4;     // Σx⁴
    double Sy   = CR.sum_Y;      // Σy
    double Sxy  = CR.sum_XY;     // Σx y
    double SX2y = CR.sum_X2Y;    // Σx² y

    double D  = S2*(S2*S2 - S1*S3)
               - S1*(S3*S2 - S1*S4)
               + S0*(S3*S3 - S2*S4);

    double Na = Sy*(S2*S2 - S1*S3)
                - S1*(Sxy*S2 - S1*SX2y)
                + S0*(Sxy*S3 - S2*SX2y);

    double Nb = S2*(Sxy*S2 - Sy*S3)
                - Sy*(S3*S2 - S1*S4)
                + S0*(S3*SX2y - Sxy*S4);

    double Nc = S2*(S2*SX2y - Sxy*S3)
                - S1*(S3*SX2y - Sxy*S4)
                + Sy*(S3*S3 - S2*S4);

    CR.a = Na / D;
    CR.b = Nb / D;
    CR.c = Nc / D;

    return CR;
}

CurveResult CurveFitting::power1(const ex &c_x, const ex &c_y, const vector<double> &x, const vector<double> &y, symbol xs, symbol ys)
{
    CurveResult CR = linear(GiNaC::log(c_x), GiNaC::log(c_y), x, y, xs, ys);
    std::swap(CR.a, CR.b);

    CR.A = CR.a;
    CR.a = std::exp(CR.A);

    return CR;
}

CurveResult CurveFitting::power2(const ex &c_x, const ex &c_y, const vector<double> &x, const vector<double> &y, symbol xs, symbol ys)
{
    CurveResult CR = linear(c_x, GiNaC::log(c_y), x, y, xs, ys);


    CR.A = CR.a;
    CR.a = std::exp(CR.A);

    CR.B = CR.b;
    CR.b = std::exp(CR.B);

    return CR;
}

CurveResult CurveFitting::exponential(const ex &c_x, const ex &c_y, const vector<double> &x, const vector<double> &y, symbol xs, symbol ys)
{
    CurveResult CR = linear(c_x, GiNaC::log(c_y), x, y, xs, ys);

    std::swap(CR.a, CR.b);

    CR.A = CR.a;
    CR.a = std::exp(CR.A);

    return CR;
}
