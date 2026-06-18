#pragma once
#include <cstdlib>
#include <cassert>
#include <iostream>

class Vector {
private:
    int mSize;
    double* mData;

public:
    // Constructors & Destructor
    Vector(int size);
    Vector(const Vector& other);
    ~Vector();

    // Assignment operator
    Vector& operator=(const Vector& other);

    // Accessors
    int GetSize() const { return mSize; }

    // Round bracket operator: 1-based indexing
    double& operator()(int i);
    double  operator()(int i) const;

    // Square bracket operator: 0-based indexing with bounds check
    double& operator[](int i);
    double  operator[](int i) const;

    // Unary operators
    Vector operator+() const;
    Vector operator-() const;

    // Binary operators
    Vector operator+(const Vector& other) const;
    Vector operator-(const Vector& other) const;
    Vector operator*(double scalar) const;
    friend Vector operator*(double scalar, const Vector& v);

    // Dot product
    double Dot(const Vector& other) const;

    // Norm
    double Norm() const;

    // Print
    void Print() const;
};
