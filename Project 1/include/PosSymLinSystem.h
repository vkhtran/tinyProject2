#pragma once
#include "LinearSystem.h"

// Positive definite symmetric linear system solver using Conjugate Gradient method
class PosSymLinSystem : public LinearSystem {
public:
    PosSymLinSystem(const Matrix& A, const Vector& b);
    virtual ~PosSymLinSystem() = default;

    virtual Vector Solve() const override;

private:
    bool IsSymmetric() const;
};
