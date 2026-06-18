#include "Vector.h"
#include <cmath>
#include <stdexcept>
#include <iostream>

Vector::Vector(int size) : mSize(size) {
    assert(size > 0);
    mData = new double[mSize]();
}

Vector::Vector(const Vector& other) : mSize(other.mSize) {
    mData = new double[mSize];
    for (int i = 0; i < mSize; ++i)
        mData[i] = other.mData[i];
}

Vector::~Vector() {
    delete[] mData;
}

Vector& Vector::operator=(const Vector& other) {
    if (this == &other) return *this;
    if (mSize != other.mSize) {
        delete[] mData;
        mSize = other.mSize;
        mData = new double[mSize];
    }
    for (int i = 0; i < mSize; ++i)
        mData[i] = other.mData[i];
    return *this;
}

// 1-based indexing
double& Vector::operator()(int i) {
    assert(i >= 1 && i <= mSize);
    return mData[i - 1];
}

double Vector::operator()(int i) const {
    assert(i >= 1 && i <= mSize);
    return mData[i - 1];
}

// 0-based indexing with bounds check
double& Vector::operator[](int i) {
    assert(i >= 0 && i < mSize);
    return mData[i];
}

double Vector::operator[](int i) const {
    assert(i >= 0 && i < mSize);
    return mData[i];
}

Vector Vector::operator+() const {
    return *this;
}

Vector Vector::operator-() const {
    Vector result(mSize);
    for (int i = 0; i < mSize; ++i)
        result.mData[i] = -mData[i];
    return result;
}

Vector Vector::operator+(const Vector& other) const {
    assert(mSize == other.mSize);
    Vector result(mSize);
    for (int i = 0; i < mSize; ++i)
        result.mData[i] = mData[i] + other.mData[i];
    return result;
}

Vector Vector::operator-(const Vector& other) const {
    assert(mSize == other.mSize);
    Vector result(mSize);
    for (int i = 0; i < mSize; ++i)
        result.mData[i] = mData[i] - other.mData[i];
    return result;
}

Vector Vector::operator*(double scalar) const {
    Vector result(mSize);
    for (int i = 0; i < mSize; ++i)
        result.mData[i] = mData[i] * scalar;
    return result;
}

Vector operator*(double scalar, const Vector& v) {
    return v * scalar;
}

double Vector::Dot(const Vector& other) const {
    assert(mSize == other.mSize);
    double sum = 0.0;
    for (int i = 0; i < mSize; ++i)
        sum += mData[i] * other.mData[i];
    return sum;
}

double Vector::Norm() const {
    return std::sqrt(Dot(*this));
}

void Vector::Print() const {
    std::cout << "[";
    for (int i = 0; i < mSize; ++i) {
        std::cout << mData[i];
        if (i < mSize - 1) std::cout << ", ";
    }
    std::cout << "]\n";
}
