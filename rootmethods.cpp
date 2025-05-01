#include "rootmethods.h"

parser RootMethods::make_full_parser(const symbol &x)
{
    parser p;
    p.get_syms()["x"] = x;
    p.get_syms()["pi"] = Pi;

    return p;
}

bool RootMethods::matchDecimals(double a, double b, double tol)
{
    double factor = std::pow(10.0, tol);
    auto ai = std::llround(a * factor);
    auto bi = std::llround(b * factor);
    return ai == bi;
}

pair<double, double> RootMethods::findBracket(
    const ex &f_expr, symbol x, double start, double end, double step)
{
    double prev_x = start;
    double prev_f;

    try {
        prev_f = ex_to<numeric>(f_expr.subs(x == prev_x)).to_double();
    } catch (const exception &e) {
        cerr << "Initial function evaluation failed at x = " << prev_x << ": " << e.what() << endl;
        prev_x += step;
        try {
            prev_f = ex_to<numeric>(f_expr.subs(x == prev_x)).to_double();
        } catch (const exception &e2) {
            cerr << "Failed again at x = " << prev_x << ": " << e2.what() << endl;
            return {NAN, NAN};
        }
    }

    for (double curr_x = prev_x + step; curr_x <= end; curr_x += step) {
        double curr_f;
        try {
            curr_f = ex_to<numeric>(f_expr.subs(x == curr_x)).to_double();
        } catch (const exception &e) {
            cerr << "Skipping x = " << curr_x << " due to error: " << e.what() << endl;
            continue;
        }

        if ((prev_f < 0 && curr_f > 0) || (prev_f > 0 && curr_f < 0)) {
            cout << "Sign change detected between x = " << prev_x << " and x = " << curr_x << endl;
            return {prev_x, curr_x};
        }

        prev_x = curr_x;
        prev_f = curr_f;
    }

    cerr << "No sign change found in the given range.\n";
    return {NAN, NAN};
}

double RootMethods::findInitialGuess(const pair<double, double> &bracket)
{
    if (isnan(bracket.first) || isnan(bracket.second)) {
        cerr << "Invalid bracket. Cannot compute initial guess.\n";
        return NAN;
    }

    return (bracket.first + bracket.second) / 2.0;
}
RootReturn RootMethods::newtonMethod(
    const ex &f_expr, symbol x, pair<double, double> &bracket, double tol, int maxIterations)
{
    RootReturn History;
    History.RootVariables['x'].push_back(findInitialGuess(bracket));
    if (isnan(History.RootVariables['x'][0])) {
        cerr << "Cannot proceed without a valid initial guess.\n";
        return {};
    }

    ex df_expr = diff(f_expr, x);

    for (int i = 0; i < maxIterations; i++) {
        double fx = ex_to<numeric>(f_expr.subs(x == History.RootVariables['x'][i])).to_double();
        double dfx = ex_to<numeric>(df_expr.subs(x == History.RootVariables['x'][i])).to_double();

        if (fabs(dfx) < 1e-10) {
            cerr << "Derivative too small. Newton's method failed.\n";
            return {};
        }

        History.RootVariables['x'].push_back(History.RootVariables['x'][i] - fx / dfx);
        double root;
        if (matchDecimals(History.RootVariables['x'][i + 1], History.RootVariables['x'][i], tol)) {
            History.Root = History.RootVariables['x'][i + 1];
            break;
        }
    }

    return History;
}

RootReturn RootMethods::bisectionMethod(
    const ex &f_expr, symbol x, pair<double, double> &bracket, double tol, int maxIterations)
{
    RootReturn History;

    // Safety: check initial values
    if (isnan(bracket.first) || isnan(bracket.second)) {
        cerr << "Invalid bracket values.\n";
        return {};
    }

    History.RootVariables['a'].push_back(bracket.first);
    History.RootVariables['b'].push_back(bracket.second);

    for (int i = 0; i < maxIterations; ++i) {
        History.RootVariables['x'].push_back(
            (History.RootVariables['a'].back() + History.RootVariables['b'].back()) / 2.0);
        if (isnan(History.RootVariables['x'].back()) || isinf(History.RootVariables['x'].back())) {
            cerr << "Midpoint calculation failed.\n";
            return {};
        }

        double fc;
        try {
            fc = ex_to<numeric>(f_expr.subs(x == History.RootVariables['x'].back())).to_double();
        } catch (const std::exception &e) {
            cerr << "Error evaluating f(c): " << e.what() << endl;
            return {};
        }

        // Check for convergence
        if (matchDecimals(History.RootVariables['x'][i], History.RootVariables['x'][i - 1], tol)) {
            if (fc < 0) {
                // Root is in [c, b]
                History.RootVariables['a'].push_back(History.RootVariables['x'].back());
                History.RootVariables['b'].push_back(History.RootVariables['b'].back());
            } else {
                // Root is in [a, c]
                History.RootVariables['a'].push_back(History.RootVariables['a'].back());
                History.RootVariables['b'].push_back(History.RootVariables['x'].back());
            }
            History.RootVariables['x'].push_back(
                (History.RootVariables['a'].back() + History.RootVariables['b'].back()) / 2.0);
            History.Root = History.RootVariables['x'].back();
            return History;
        }

        // Update a or b depending on sign
        if (fc < 0) {
            // Root is in [c, b]
            History.RootVariables['a'].push_back(History.RootVariables['x'].back());
            History.RootVariables['b'].push_back(History.RootVariables['b'].back());
        } else {
            // Root is in [a, c]
            History.RootVariables['a'].push_back(History.RootVariables['a'].back());
            History.RootVariables['b'].push_back(History.RootVariables['x'].back());
        }
    }

    // If loop ends without convergence, return last midpoint
    History.Root = History.RootVariables['x'].back();
    return History;
}

RootReturn RootMethods::secantMethod(
    const ex &f_expr, symbol x, pair<double, double> &bracket, double tol, int maxIterations)
{
    RootReturn History;
    History.RootVariables['x'].push_back(bracket.first);
    History.RootVariables['x'].push_back(bracket.second);

    for (int i = 2; i < maxIterations; i++) {
        double f_x0 = ex_to<numeric>(f_expr.subs(x == History.RootVariables['x'][i - 2])).to_double();
        double f_x1 = ex_to<numeric>(f_expr.subs(x == History.RootVariables['x'][i - 1])).to_double();
        cout << "F(Xm) = " << f_x1 << endl << "F(Xm-1) = " << f_x0 << endl;

        if (fabs(f_x1 - f_x0) < 1e-10) {
            if (i > 2)
                return History;
            else
                return {};
        }

        History.RootVariables['x'].push_back(
            History.RootVariables['x'][i - 1]
            - (f_x1 * (History.RootVariables['x'][i - 1] - History.RootVariables['x'][i - 2])
               / (f_x1 - f_x0)));

        if (matchDecimals(History.RootVariables['x'][i], History.RootVariables['x'][i - 1], tol)) {
            History.Root = History.RootVariables['x'][i];
            break;
        }
    }

    return History;
}
