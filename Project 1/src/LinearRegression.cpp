#include "LinearRegression.h"
#include "LinearSystem.h"
#include "Matrix.h"
#include "Vector.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <iostream>
#include <stdexcept>

bool LinearRegression::LoadData(const std::string& filename,
                                 std::vector<CPURecord>& records) const {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filename << "\n";
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;
        while (std::getline(ss, token, ','))
            tokens.push_back(token);

        // Expecting at least 10 columns
        if (tokens.size() < 10) continue;

        CPURecord r;
        // Col 0: vendor name (skip), Col 1: model name (skip)
        // Cols 2-7: MYCT, MMIN, MMAX, CACH, CHMIN, CHMAX
        // Col 8: PRP (target), Col 9: ERP (skip)
        try {
            r.MYCT  = std::stod(tokens[2]);
            r.MMIN  = std::stod(tokens[3]);
            r.MMAX  = std::stod(tokens[4]);
            r.CACH  = std::stod(tokens[5]);
            r.CHMIN = std::stod(tokens[6]);
            r.CHMAX = std::stod(tokens[7]);
            r.PRP   = std::stod(tokens[8]);
        } catch (...) {
            continue;
        }
        records.push_back(r);
    }
    file.close();
    return !records.empty();
}

// Build design matrix A and target vector b, then solve via normal equations
Vector LinearRegression::Train(const std::vector<CPURecord>& trainSet) const {
    int m = (int)trainSet.size();
    int n = 6; // MYCT, MMIN, MMAX, CACH, CHMIN, CHMAX

    Matrix A(m, n);
    Vector b(m);

    for (int i = 0; i < m; ++i) {
        A(i + 1, 1) = trainSet[i].MYCT;
        A(i + 1, 2) = trainSet[i].MMIN;
        A(i + 1, 3) = trainSet[i].MMAX;
        A(i + 1, 4) = trainSet[i].CACH;
        A(i + 1, 5) = trainSet[i].CHMIN;
        A(i + 1, 6) = trainSet[i].CHMAX;
        b(i + 1) = trainSet[i].PRP;
    }

    // Solve overdetermined system via normal equations: (A^T A) x = A^T b
    Matrix At  = A.Transpose();
    Matrix AtA = At * A;
    Vector Atb = At * b;

    // small value to avoid singular matrix problem (Tikhonov regularization)
    double lambda = 1e-6;
    for (int i = 1; i <= n; ++i)
        AtA(i, i) += lambda;

    // use LinearSystem from Part A to solve the 6x6 system
    LinearSystem sys(AtA, Atb);
    return sys.Solve();
}

double LinearRegression::Predict(const Vector& params,
                                  const CPURecord& r) const {
    return params(1) * r.MYCT
         + params(2) * r.MMIN
         + params(3) * r.MMAX
         + params(4) * r.CACH
         + params(5) * r.CHMIN
         + params(6) * r.CHMAX;
}

double LinearRegression::RMSE(const Vector& params,
                               const std::vector<CPURecord>& testSet) const {
    double sumSq = 0.0;
    for (const auto& r : testSet) {
        double err = Predict(params, r) - r.PRP;
        sumSq += err * err;
    }
    return std::sqrt(sumSq / (double)testSet.size());
}
