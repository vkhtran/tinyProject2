#pragma once
#include "Vector.h"
#include <cassert>
#include <iostream>

class Matrix {
private:
    int mNumRows;
    int mNumCols;
    double** mData;

    void Allocate();
    void Deallocate();

public:
    // Constructors & Destructor
    Matrix(int numRows, int numCols);
    Matrix(const Matrix& other);
    ~Matrix();

    // Assignment operator
    Matrix& operator=(const Matrix& other);

    // Accessors
    int GetNumRows() const { return mNumRows; }
    int GetNumCols() const { return mNumCols; }

    // Round bracket operator: 1-based indexing
    double& operator()(int i, int j);
    double  operator()(int i, int j) const;

    // Unary operators
    Matrix operator+() const;
    Matrix operator-() const;

    // Binary operators: matrix arithmetic
    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;
    Matrix operator*(double scalar) const;
    Vector operator*(const Vector& v) const;
    friend Matrix operator*(double scalar, const Matrix& m);

    // Special methods
    double  Determinant() const;
    Matrix  Inverse() const;
    Matrix  PseudoInverse() const;
    Matrix  Transpose() const;

    // Print
    void Print() const;
};
