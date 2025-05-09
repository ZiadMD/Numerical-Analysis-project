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

    /**
     *
     * Compute the Lagrange interpolating polynomial P(x) for given nodes and values,
     * and evaluate at point x0.
     *
     * @param xs   Abscissae x0...xN-1
     * @param ys   Ordinates y0...yN-1
     * @param x0   Evaluation point
     * @param xSym Symbol for x
     * @return     InterpolationResult with lagrangeBasis, polynomial, empty differenceTable
     */
    InterpolationResult lagrange(const vector<double> &x, const std::vector<double> &y, const double &x_, const symbol &sym);

    /**
     * Build Newton forward-difference table and polynomial, evaluate at x0.
     *
     * @param xs   Abscissae
     * @param ys   Ordinates
     * @param x0   Evaluation point
     * @param xSym Symbol for x
     * @return     InterpolationResult with differenceTable, polynomial, empty lagrangeBasis
     */
    InterpolationResult newtonForward(const vector<double> &x, const std::vector<double> &y, const double &x_, const symbol &sym);

    /**
     * Build Newton backward-difference table and polynomial, evaluate at x0.
     *
     * @param xs   Abscissae
     * @param ys   Ordinates
     * @param x0   Evaluation point
     * @param xSym Symbol for x
     * @return     InterpolationResult with differenceTable, polynomial, empty lagrangeBasis
     */
    InterpolationResult newtonBackward(const vector<double> &x, const std::vector<double> &y, const double &x_, const symbol &sym);
private:

    /**
     * Construct forward-difference table up to order N-1.
     *
     * Table[k][m] = (Table[k-1][m+1] - Table[k-1][m]) / (x[m+k] - x[m])
     *
     * @param xs Abscissae
     * @param ys Ordinates
     * @return   InterpolationResult with differenceTable filled
     */
    InterpolationResult NewtonTable(const vector<double> &x, const std::vector<double> &y);
};

#endif // INTERPOLATIONMETHODS_H
