#include "curvefitting.h"

class Matrix {
private:
    std::vector<std::vector<double>> data;
    int rows;
    int cols;

public:
    // Constructor
    Matrix(int r, int c) : rows(r), cols(c), data(r, std::vector<double>(c, 0.0)) {}

    // Constructor from a 2D vector
    Matrix(const std::vector<std::vector<double>>& initialData) : data(initialData) {
        rows = data.size();
        if (rows > 0) {
            cols = data[0].size();
            // Ensure all rows have the same number of columns
            for (const auto& row : data) {
                if (row.size() != cols) {
                    throw std::invalid_argument("Matrix rows must have the same number of columns.");
                }
            }
        } else {
            cols = 0;
        }
    }

    // Getters for rows and columns
    int getRows() const {
        return rows;
    }

    int getCols() const {
        return cols;
    }

    // Access element at row i, column j (0-indexed)
    double& operator()(int i, int j) {
        if (i < 0 || i >= rows || j < 0 || j >= cols) {
            throw std::out_of_range("Matrix index out of bounds.");
        }
        return data[i][j];
    }

    // Access element at row i, column j (0-indexed) - const version
    double operator()(int i, int j) const {
        if (i < 0 || i >= rows || j < 0 || j >= cols) {
            throw std::out_of_range("Matrix index out of bounds.");
        }
        return data[i][j];
    }

    // Matrix addition
    Matrix operator+(const Matrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw std::invalid_argument("Matrices must have the same dimensions for addition.");
        }
        Matrix result(rows, cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                result(i, j) = data[i][j] + other(i, j);
            }
        }
        return result;
    }

    // Matrix multiplication
    Matrix operator*(const Matrix& other) const {
        if (cols != other.rows) {
            throw std::invalid_argument("Number of columns in the first matrix must equal the number of rows in the second matrix for multiplication.");
        }
        Matrix result(rows, other.cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < other.cols; ++j) {
                for (int k = 0; k < cols; ++k) {
                    result(i, j) += data[i][k] * other(k, j);
                }
            }
        }
        return result;
    }

    // Helper function to get a submatrix (used for determinant and inverse)
    Matrix getSubmatrix(int excludeRow, int excludeCol) const {
        if (rows != cols) {
            throw std::invalid_argument("Matrix must be square to get a submatrix for determinant calculation.");
        }
        if (rows <= 1) {
            throw std::invalid_argument("Cannot get a submatrix of a 1x1 matrix.");
        }
        Matrix sub(rows - 1, cols - 1);
        int subRow = 0;
        for (int i = 0; i < rows; ++i) {
            if (i == excludeRow) continue;
            int subCol = 0;
            for (int j = 0; j < cols; ++j) {
                if (j == excludeCol) continue;
                sub(subRow, subCol) = data[i][j];
                subCol++;
            }
            subRow++;
        }
        return sub;
    }

    // Helper function to calculate the determinant (recursive)
    double determinant() const {
        if (rows != cols) {
            throw std::invalid_argument("Matrix must be square to calculate the determinant.");
        }
        if (rows == 1) {
            return data[0][0];
        }
        if (rows == 2) {
            return data[0][0] * data[1][1] - data[0][1] * data[1][0];
        }
        double det = 0.0;
        for (int j = 0; j < cols; ++j) {
            Matrix sub = getSubmatrix(0, j);
            det += data[0][j] * std::pow(-1, 0 + j) * sub.determinant();
        }
        return det;
    }

    // Inverse of the matrix
    Matrix inverse() const {
        if (rows != cols) {
            throw std::invalid_argument("Matrix must be square to calculate the inverse.");
        }
        double det = determinant();
        if (std::abs(det) < 1e-9) { // Check if determinant is close to zero
            throw std::runtime_error("Matrix is singular (determinant is zero), cannot calculate inverse.");
        }

        Matrix cofactorMatrix(rows, cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                Matrix sub = getSubmatrix(i, j);
                cofactorMatrix(i, j) = std::pow(-1, i + j) * sub.determinant();
            }
        }

        // Adjugate (transpose of cofactor matrix)
        Matrix adjugateMatrix(cols, rows);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                adjugateMatrix(j, i) = cofactorMatrix(i, j);
            }
        }

        // Multiply by 1/determinant
        Matrix inverseMatrix(rows, cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                inverseMatrix(i, j) = adjugateMatrix(i, j) / det;
            }
        }
        return inverseMatrix;
    }
};

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

    /*
     * double S0   = n;             // n
    // double S1   = CR.sum_X;      // Σx
    // double S2   = CR.sum_X2;     // Σx²
    // double S3   = CR.sum_X3;     // Σx³
    // double S4   = CR.sum_X4;     // Σx⁴
    // double Sy   = CR.sum_Y;      // Σy
    // double Sxy  = CR.sum_XY;     // Σx y
    // double SX2y = CR.sum_X2Y;    // Σx² y

    // double D  = S2*(S2*S2 - S1*S3)
    //            - S1*(S3*S2 - S1*S4)
    //            + S0*(S3*S3 - S2*S4);

    // double Na = Sy*(S2*S2 - S1*S3)
    //             - S1*(Sxy*S2 - S1*SX2y)
    //             + S0*(Sxy*S3 - S2*SX2y);

    // double Nb = S2*(Sxy*S2 - Sy*S3)
    //             - Sy*(S3*S2 - S1*S4)
    //             + S0*(S3*SX2y - Sxy*S4);

    // double Nc = S2*(S2*SX2y - Sxy*S3)
    //             - S1*(S3*SX2y - Sxy*S4)
    //             + Sy*(S3*S3 - S2*S4);

    // CR.a = Na / D;
    // CR.b = Nb / D;
     CR.c = Nc / D;*/
    Matrix A = Matrix({{CR.sum_X2, CR.sum_X, double(n)},
                       {CR.sum_X3, CR.sum_X2, CR.sum_X},
                       {CR.sum_X4, CR.sum_X3, CR.sum_X2}});

    Matrix B = Matrix({{CR.sum_Y},
                       {CR.sum_XY},
                       {CR.sum_X2Y}});

    Matrix A_inv = A.inverse();
    Matrix X = A_inv * B;
    cerr << "Cols: " << X.getCols() << "\nRows: " << X.getRows();
    CR.a = X(0,0);
    CR.b = X(1,0);
    CR.c = X(2,0);
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
