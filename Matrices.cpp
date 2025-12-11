#include "Matrices.h"

using namespace std;
using namespace Matrices;

// Constructor: rows x cols, initialize all to 0
Matrix::Matrix(int _rows, int _cols)
    : a(_rows, vector<double>(_cols, 0.0)), rows(_rows), cols(_cols)
{
}

// Matrix addition
Matrix Matrices::operator+(const Matrix& lhs, const Matrix& rhs)
{
    int rows = lhs.getRows();
    int cols = lhs.getCols();

    Matrix result(rows, cols);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            result(i, j) = lhs(i, j) + rhs(i, j);
        }
    }

    return result;
}

// Matrix multiplication: (rows x k) * (k x cols)
Matrix Matrices::operator*(const Matrix& lhs, const Matrix& rhs)
{
    int rows = lhs.getRows();
    int inner = lhs.getCols();
    int cols = rhs.getCols();

    Matrix result(rows, cols);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            double sum = 0.0;
            for (int k = 0; k < inner; k++)
            {
                sum += lhs(i, k) * rhs(k, j);
            }
            result(i, j) = sum;
        }
    }

    return result;
}

// Equality: same size and all elements equal
bool Matrices::operator==(const Matrix& lhs, const Matrix& rhs)
{
    if (lhs.getRows() != rhs.getRows() || lhs.getCols() != rhs.getCols())
    {
        return false;
    }

    for (int i = 0; i < lhs.getRows(); i++)
    {
        for (int j = 0; j < lhs.getCols(); j++)
        {
            if (lhs(i, j) != rhs(i, j))
            {
                return false;
            }
        }
    }

    return true;
}

bool Matrices::operator!=(const Matrix& lhs, const Matrix& rhs)
{
    return !(lhs == rhs);
}

// Output matrix: columns separated by ' ', rows by '\n'
ostream& Matrices::operator<<(ostream& os, const Matrix& m)
{
    int rows = m.getRows();
    int cols = m.getCols();

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            os << m(i, j);
            if (j < cols - 1)
            {
                os << ' ';
            }
        }
        if (i < rows - 1)
        {
            os << '\n';
        }
    }

    return os;
}

// RotationMatrix: 2x2
RotationMatrix::RotationMatrix(double theta)
    : Matrix(2, 2)
{
    (*this)(0, 0) = cos(theta);
    (*this)(0, 1) = -sin(theta);
    (*this)(1, 0) = sin(theta);
    (*this)(1, 1) = cos(theta);
}

// ScalingMatrix: 2x2
ScalingMatrix::ScalingMatrix(double scale)
    : Matrix(2, 2)
{
    (*this)(0, 0) = scale;
    (*this)(0, 1) = 0.0;
    (*this)(1, 0) = 0.0;
    (*this)(1, 1) = scale;
}

// TranslationMatrix: 2 x nCols
TranslationMatrix::TranslationMatrix(double xShift, double yShift, int nCols)
    : Matrix(2, nCols)
{
    for (int j = 0; j < nCols; j++)
    {
        (*this)(0, j) = xShift;
        (*this)(1, j) = yShift;
    }
}