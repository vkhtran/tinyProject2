#include "Matrix.h"
#include <cmath>
#include <stdexcept>
#include <iostream>

void Matrix::Allocate() {
    mData = new double*[mNumRows];
    for (int i = 0; i < mNumRows; ++i)
        mData[i] = new double[mNumCols]();
}

void Matrix::Deallocate() {
    for (int i = 0; i < mNumRows; ++i)
        delete[] mData[i];
    delete[] mData;
}

Matrix::Matrix(int numRows, int numCols) : mNumRows(numRows), mNumCols(numCols) {
    assert(numRows > 0 && numCols > 0);
    Allocate();
}

Matrix::Matrix(const Matrix& other) : mNumRows(other.mNumRows), mNumCols(other.mNumCols) {
    Allocate();
    for (int i = 0; i < mNumRows; ++i)
        for (int j = 0; j < mNumCols; ++j)
            mData[i][j] = other.mData[i][j];
}

Matrix::~Matrix() {
    Deallocate();
}

Matrix& Matrix::operator=(const Matrix& other) {
    if (this == &other) return *this;
    Deallocate();
    mNumRows = other.mNumRows;
    mNumCols = other.mNumCols;
    Allocate();
    for (int i = 0; i < mNumRows; ++i)
        for (int j = 0; j < mNumCols; ++j)
            mData[i][j] = other.mData[i][j];
    return *this;
}

// 1-based indexing
double& Matrix::operator()(int i, int j) {
    assert(i >= 1 && i <= mNumRows && j >= 1 && j <= mNumCols);
    return mData[i - 1][j - 1];
}

double Matrix::operator()(int i, int j) const {
    assert(i >= 1 && i <= mNumRows && j >= 1 && j <= mNumCols);
    return mData[i - 1][j - 1];
}

Matrix Matrix::operator+() const { return *this; }

Matrix Matrix::operator-() const {
    Matrix result(mNumRows, mNumCols);
    for (int i = 0; i < mNumRows; ++i)
        for (int j = 0; j < mNumCols; ++j)
            result.mData[i][j] = -mData[i][j];
    return result;
}

Matrix Matrix::operator+(const Matrix& other) const {
    assert(mNumRows == other.mNumRows && mNumCols == other.mNumCols);
    Matrix result(mNumRows, mNumCols);
    for (int i = 0; i < mNumRows; ++i)
        for (int j = 0; j < mNumCols; ++j)
            result.mData[i][j] = mData[i][j] + other.mData[i][j];
    return result;
}

Matrix Matrix::operator-(const Matrix& other) const {
    assert(mNumRows == other.mNumRows && mNumCols == other.mNumCols);
    Matrix result(mNumRows, mNumCols);
    for (int i = 0; i < mNumRows; ++i)
        for (int j = 0; j < mNumCols; ++j)
            result.mData[i][j] = mData[i][j] - other.mData[i][j];
    return result;
}

Matrix Matrix::operator*(const Matrix& other) const {
    assert(mNumCols == other.mNumRows);
    Matrix result(mNumRows, other.mNumCols);
    for (int i = 0; i < mNumRows; ++i)
        for (int j = 0; j < other.mNumCols; ++j)
            for (int k = 0; k < mNumCols; ++k)
                result.mData[i][j] += mData[i][k] * other.mData[k][j];
    return result;
}

Matrix Matrix::operator*(double scalar) const {
    Matrix result(mNumRows, mNumCols);
    for (int i = 0; i < mNumRows; ++i)
        for (int j = 0; j < mNumCols; ++j)
            result.mData[i][j] = mData[i][j] * scalar;
    return result;
}

Matrix operator*(double scalar, const Matrix& m) {
    return m * scalar;
}

Vector Matrix::operator*(const Vector& v) const {
    assert(mNumCols == v.GetSize());
    Vector result(mNumRows);
    for (int i = 0; i < mNumRows; ++i) {
        double sum = 0.0;
        for (int j = 0; j < mNumCols; ++j)
            sum += mData[i][j] * v(j + 1);
        result(i + 1) = sum;
    }
    return result;
}

Matrix Matrix::Transpose() const {
    Matrix result(mNumCols, mNumRows);
    for (int i = 0; i < mNumRows; ++i)
        for (int j = 0; j < mNumCols; ++j)
            result.mData[j][i] = mData[i][j];
    return result;
}

// Recursive determinant via cofactor expansion (for general square matrices)
double Matrix::Determinant() const {
    assert(mNumRows == mNumCols);
    int n = mNumRows;
    if (n == 1) return mData[0][0];
    if (n == 2) return mData[0][0]*mData[1][1] - mData[0][1]*mData[1][0];

    // LU decomposition approach for larger matrices
    Matrix lu(*this);
    double det = 1.0;
    for (int col = 0; col < n; ++col) {
        // Find pivot
        int pivotRow = col;
        double maxVal = std::fabs(lu.mData[col][col]);
        for (int row = col + 1; row < n; ++row) {
            if (std::fabs(lu.mData[row][col]) > maxVal) {
                maxVal = std::fabs(lu.mData[row][col]);
                pivotRow = row;
            }
        }
        if (pivotRow != col) {
            std::swap(lu.mData[col], lu.mData[pivotRow]);
            det *= -1.0;
        }
        if (std::fabs(lu.mData[col][col]) < 1e-12) return 0.0;
        det *= lu.mData[col][col];
        for (int row = col + 1; row < n; ++row) {
            double factor = lu.mData[row][col] / lu.mData[col][col];
            for (int k = col; k < n; ++k)
                lu.mData[row][k] -= factor * lu.mData[col][k];
        }
    }
    return det;
}

// Inverse via Gauss-Jordan elimination
Matrix Matrix::Inverse() const {
    assert(mNumRows == mNumCols);
    int n = mNumRows;

    // Build augmented matrix [A | I]
    Matrix aug(n, 2 * n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j)
            aug.mData[i][j] = mData[i][j];
        aug.mData[i][n + i] = 1.0;
    }

    // Forward elimination with partial pivoting
    for (int col = 0; col < n; ++col) {
        int pivotRow = col;
        double maxVal = std::fabs(aug.mData[col][col]);
        for (int row = col + 1; row < n; ++row) {
            if (std::fabs(aug.mData[row][col]) > maxVal) {
                maxVal = std::fabs(aug.mData[row][col]);
                pivotRow = row;
            }
        }
        assert(std::fabs(aug.mData[pivotRow][col]) > 1e-12); // singular check
        if (pivotRow != col) std::swap(aug.mData[col], aug.mData[pivotRow]);

        double pivot = aug.mData[col][col];
        for (int k = 0; k < 2 * n; ++k)
            aug.mData[col][k] /= pivot;

        for (int row = 0; row < n; ++row) {
            if (row == col) continue;
            double factor = aug.mData[row][col];
            for (int k = 0; k < 2 * n; ++k)
                aug.mData[row][k] -= factor * aug.mData[col][k];
        }
    }

    Matrix inv(n, n);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            inv.mData[i][j] = aug.mData[i][n + j];
    return inv;
}

// Moore-Penrose pseudo-inverse using SVD-like approach via normal equations
// For overdetermined (m > n): pinv = (A^T A)^{-1} A^T
// For underdetermined (m < n): pinv = A^T (A A^T)^{-1}
// For square non-singular: same as inverse
Matrix Matrix::PseudoInverse() const {
    int m = mNumRows, n = mNumCols;
    Matrix At = Transpose();

    if (m >= n) {
        // Overdetermined or square: (A^T A)^{-1} A^T
        Matrix AtA = At * (*this);
        return AtA.Inverse() * At;
    } else {
        // Underdetermined: A^T (A A^T)^{-1}
        Matrix AAt = (*this) * At;
        return At * AAt.Inverse();
    }
}

void Matrix::Print() const {
    for (int i = 0; i < mNumRows; ++i) {
        std::cout << "[ ";
        for (int j = 0; j < mNumCols; ++j) {
            std::cout << mData[i][j];
            if (j < mNumCols - 1) std::cout << "\t";
        }
        std::cout << " ]\n";
    }
}
