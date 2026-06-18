#include "PosSymLinSystem.h"
#include <cmath>
#include <cassert>
#include <stdexcept>
#include <iostream>

PosSymLinSystem::PosSymLinSystem(const Matrix& A, const Vector& b)
    : LinearSystem(A, b)
{
    if (!IsSymmetric())
        throw std::invalid_argument("Matrix must be symmetric for PosSymLinSystem.");
}

bool PosSymLinSystem::IsSymmetric() const {
    const double tol = 1e-10;
    for (int i = 1; i <= mSize; ++i)
        for (int j = i + 1; j <= mSize; ++j)
            if (std::fabs((*mpA)(i, j) - (*mpA)(j, i)) > tol)
                return false;
    return true;
}

// Conjugate Gradient method
Vector PosSymLinSystem::Solve() const {
    int n = mSize;
    const double tol = 1e-10;
    const int maxIter = 10000;

    Vector x(n);  // Initial guess: zero vector
    Vector r(*mpb);  // r = b - A*x = b (since x=0)
    Vector p(r);     // p = r

    double rDotR = r.Dot(r);

    for (int iter = 0; iter < maxIter; ++iter) {
        if (std::sqrt(rDotR) < tol) break;

        Vector Ap = (*mpA) * p;
        double alpha = rDotR / p.Dot(Ap);

        // x = x + alpha * p
        for (int i = 1; i <= n; ++i)
            x(i) += alpha * p(i);

        // r = r - alpha * Ap
        for (int i = 1; i <= n; ++i)
            r(i) -= alpha * Ap(i);

        double rDotRNew = r.Dot(r);
        double beta = rDotRNew / rDotR;

        // p = r + beta * p
        for (int i = 1; i <= n; ++i)
            p(i) = r(i) + beta * p(i);

        rDotR = rDotRNew;
    }

    return x;
}
