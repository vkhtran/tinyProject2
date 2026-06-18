#pragma once
#include "Matrix.h"
#include "Vector.h"
#include <vector>
#include <string>

struct CPURecord {
    double MYCT, MMIN, MMAX, CACH, CHMIN, CHMAX;
    double PRP;  // target
};

class LinearRegression {
public:
    LinearRegression() = default;

    // Load dataset from file
    bool LoadData(const std::string& filename,
                  std::vector<CPURecord>& records) const;

    // Train: solve A^T A x = A^T b via pseudo-inverse or normal equations
    Vector Train(const std::vector<CPURecord>& trainSet) const;

    // Evaluate RMSE on a set given trained parameters
    double RMSE(const Vector& params,
                const std::vector<CPURecord>& testSet) const;

    // Predict single record
    double Predict(const Vector& params, const CPURecord& r) const;
};
