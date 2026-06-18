#include "LinearSystem.h"
#include <cmath>
#include <cassert>
#include <stdexcept>
#include <iostream>

LinearSystem::LinearSystem(const Matrix& A, const Vector& b) {
    assert(A.GetNumRows() == A.GetNumCols());
    assert(A.GetNumRows() == b.GetSize());
    mSize = A.GetNumRows();
    mpA   = new Matrix(A);
    mpb   = new Vector(b);
}

LinearSystem::~LinearSystem() {
    delete mpA;
    delete mpb;
}

// Gaussian elimination with partial pivoting
Vector LinearSystem::Solve() const {
    int n = mSize;
    // Work on copies so original data is preserved
    Matrix A(*mpA);
    Vector b(*mpb);

    // Forward elimination
    for (int col = 0; col < n; ++col) {
        // Find pivot row
        int pivotRow = col;
        double maxVal = std::fabs(A(col + 1, col + 1));
        for (int row = col + 1; row < n; ++row) {
            if (std::fabs(A(row + 1, col + 1)) > maxVal) {
                maxVal = std::fabs(A(row + 1, col + 1));
                pivotRow = row;
            }
        }
        if (maxVal < 1e-12)
            throw std::runtime_error("Matrix is singular or nearly singular.");

        // Swap rows if needed
        if (pivotRow != col) {
            for (int k = 1; k <= n; ++k) {
                double tmp = A(col + 1, k);
                A(col + 1, k) = A(pivotRow + 1, k);
                A(pivotRow + 1, k) = tmp;
            }
            double tmp = b(col + 1);
            b(col + 1) = b(pivotRow + 1);
            b(pivotRow + 1) = tmp;
        }

        // Eliminate below pivot
        for (int row = col + 1; row < n; ++row) {
            double factor = A(row + 1, col + 1) / A(col + 1, col + 1);
            for (int k = col + 1; k <= n; ++k)
                A(row + 1, k) -= factor * A(col + 1, k);
            b(row + 1) -= factor * b(col + 1);
        }
    }

    // Back substitution
    Vector x(n);
    for (int i = n - 1; i >= 0; --i) {
        double sum = b(i + 1);
        for (int j = i + 1; j < n; ++j)
            sum -= A(i + 1, j + 1) * x(j + 1);
        x(i + 1) = sum / A(i + 1, i + 1);
    }
    return x;
}
