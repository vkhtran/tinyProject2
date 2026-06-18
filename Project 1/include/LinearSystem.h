#pragma once
#include "Matrix.h"
#include "Vector.h"

class LinearSystem {
private:
    // Prevent default constructor and copy constructor
    LinearSystem();
    LinearSystem(const LinearSystem&);

protected:
    int     mSize;
    Matrix* mpA;
    Vector* mpb;

public:
    LinearSystem(const Matrix& A, const Vector& b);
    virtual ~LinearSystem();

    virtual Vector Solve() const;
};
