#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include "Vector.h"
#include "Matrix.h"
#include "LinearSystem.h"
#include "PosSymLinSystem.h"
#include "LinearRegression.h"

// ── Helper separator ──────────────────────────────────────────────────────────
static void sep(const std::string& title) {
    std::cout << "\n============================================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "============================================================\n";
}

// ── Part A Tests ──────────────────────────────────────────────────────────────

void testVector() {
    sep("TEST: Vector class");

    Vector v1(3), v2(3);
    v1(1) = 1.0; v1(2) = 2.0; v1(3) = 3.0;
    v2(1) = 4.0; v2(2) = 5.0; v2(3) = 6.0;

    std::cout << "v1 = "; v1.Print();
    std::cout << "v2 = "; v2.Print();
    std::cout << "v1 + v2 = "; (v1 + v2).Print();
    std::cout << "v1 - v2 = "; (v1 - v2).Print();
    std::cout << "2.0 * v1 = "; (2.0 * v1).Print();
    std::cout << "v1 dot v2 = " << v1.Dot(v2) << "\n";
    std::cout << "||v1|| = " << v1.Norm() << "\n";
    std::cout << "-v1 = "; (-v1).Print();
    // Square bracket (0-based)
    std::cout << "v1[0] = " << v1[0] << ", v1[2] = " << v1[2] << "\n";
}

void testMatrix() {
    sep("TEST: Matrix class");

    Matrix A(3, 3);
    A(1,1) = 2; A(1,2) = 1; A(1,3) = 0;
    A(2,1) = 1; A(2,2) = 3; A(2,3) = 1;
    A(3,1) = 0; A(3,2) = 1; A(3,3) = 2;

    std::cout << "Matrix A:\n"; A.Print();
    std::cout << "det(A) = " << A.Determinant() << "\n";
    std::cout << "A^{-1}:\n"; A.Inverse().Print();
    std::cout << "A * A^{-1} (should be I):\n"; (A * A.Inverse()).Print();
    std::cout << "A^T:\n"; A.Transpose().Print();

    // Pseudo-inverse on a non-square matrix
    Matrix B(4, 3);
    B(1,1) = 1; B(1,2) = 2; B(1,3) = 3;
    B(2,1) = 4; B(2,2) = 5; B(2,3) = 6;
    B(3,1) = 7; B(3,2) = 8; B(3,3) = 9;
    B(4,1) = 1; B(4,2) = 0; B(4,3) = 1;
    std::cout << "Matrix B (4x3):\n"; B.Print();
    std::cout << "Pseudo-inverse of B (3x4):\n"; B.PseudoInverse().Print();
}

void testLinearSystem() {
    sep("TEST: LinearSystem (Gaussian Elimination)");

    // Example: 3x3 system
    Matrix A(3, 3);
    A(1,1) = 2; A(1,2) =-1; A(1,3) = 0;
    A(2,1) =-1; A(2,2) = 2; A(2,3) =-1;
    A(3,1) = 0; A(3,2) =-1; A(3,3) = 2;

    Vector b(3);
    b(1) = 1; b(2) = 0; b(3) = 1;

    std::cout << "A:\n"; A.Print();
    std::cout << "b: "; b.Print();

    LinearSystem sys(A, b);
    Vector x = sys.Solve();
    std::cout << "Solution x: "; x.Print();

    // Verify A*x = b
    Vector Ax = A * x;
    std::cout << "A*x (should equal b): "; Ax.Print();
}

void testPosSymLinSystem() {
    sep("TEST: PosSymLinSystem (Conjugate Gradient)");

    // Symmetric positive definite matrix
    Matrix A(4, 4);
    A(1,1) = 4; A(1,2) = 1; A(1,3) = 0; A(1,4) = 0;
    A(2,1) = 1; A(2,2) = 3; A(2,3) = 1; A(2,4) = 0;
    A(3,1) = 0; A(3,2) = 1; A(3,3) = 4; A(3,4) = 1;
    A(4,1) = 0; A(4,2) = 0; A(4,3) = 1; A(4,4) = 3;

    Vector b(4);
    b(1) = 1; b(2) = 2; b(3) = 3; b(3) = 3; b(4) = 4;

    std::cout << "Symmetric positive definite A:\n"; A.Print();
    std::cout << "b: "; b.Print();

    PosSymLinSystem sys(A, b);
    Vector x = sys.Solve();
    std::cout << "CG Solution x: "; x.Print();

    // Verify
    Vector Ax = A * x;
    std::cout << "A*x (should equal b): "; Ax.Print();

    // Compare with Gaussian elimination
    LinearSystem sysGauss(A, b);
    Vector xGauss = sysGauss.Solve();
    std::cout << "Gauss solution x: "; xGauss.Print();
}

void testUnderdetermined() {
    sep("TEST: Under/Over-determined system (Pseudo-inverse)");

    // Overdetermined: 4 equations, 2 unknowns
    Matrix A(4, 2);
    A(1,1) = 1; A(1,2) = 1;
    A(2,1) = 1; A(2,2) = 2;
    A(3,1) = 1; A(3,2) = 3;
    A(4,1) = 1; A(4,2) = 4;

    Vector b(4);
    b(1) = 6; b(2) = 5; b(3) = 7; b(4) = 10;

    std::cout << "Overdetermined A (4x2):\n"; A.Print();
    std::cout << "b: "; b.Print();

    // Least squares solution via pseudo-inverse
    Matrix Apinv = A.PseudoInverse();
    Vector x = Apinv * b;
    std::cout << "Least-squares solution x: "; x.Print();
    std::cout << "A*x (approximation): "; (A * x).Print();
}

// ── Part B ────────────────────────────────────────────────────────────────────

void runLinearRegression(const std::string& dataPath) {
    sep("PART B: Linear Regression on CPU Performance Dataset");

    LinearRegression lr;
    std::vector<CPURecord> records;

    if (!lr.LoadData(dataPath, records)) {
        std::cout << "Dataset not found at: " << dataPath << "\n";
        std::cout << "Generating synthetic demonstration with 20 records...\n";

        // Synthetic data for demonstration
        std::mt19937 rng(42);
        std::uniform_real_distribution<> dist(0.5, 2.0);
        // True parameters: x1..x6 = {0.01, 0.02, 0.005, 0.5, 1.0, 0.8}
        std::vector<double> trueParams = {0.01, 0.02, 0.005, 0.5, 1.0, 0.8};
        std::normal_distribution<> noise(0, 5.0);

        for (int i = 0; i < 100; ++i) {
            CPURecord r;
            r.MYCT  = 20 + rng() % 400;
            r.MMIN  = 256 + rng() % 8000;
            r.MMAX  = 512 + rng() % 32000;
            r.CACH  = 8 + rng() % 256;
            r.CHMIN = 1 + rng() % 52;
            r.CHMAX = 4 + rng() % 176;
            r.PRP   = trueParams[0] * r.MYCT
                    + trueParams[1] * r.MMIN
                    + trueParams[2] * r.MMAX
                    + trueParams[3] * r.CACH
                    + trueParams[4] * r.CHMIN
                    + trueParams[5] * r.CHMAX
                    + noise(rng);
            records.push_back(r);
        }
        std::cout << "Synthetic dataset generated with " << records.size() << " records.\n";
    } else {
        std::cout << "Loaded " << records.size() << " records from " << dataPath << "\n";
    }

    // Shuffle and split 80/20
    std::mt19937 rng(123);
    std::shuffle(records.begin(), records.end(), rng);

    int trainSize = (int)(records.size() * 0.8);
    std::vector<CPURecord> trainSet(records.begin(), records.begin() + trainSize);
    std::vector<CPURecord> testSet(records.begin() + trainSize, records.end());

    std::cout << "Training set size: " << trainSet.size() << "\n";
    std::cout << "Testing set size:  " << testSet.size() << "\n";

    // Train
    Vector params = lr.Train(trainSet);
    std::cout << "\nLearned parameters:\n";
    std::cout << "  x1 (MYCT)  = " << params(1) << "\n";
    std::cout << "  x2 (MMIN)  = " << params(2) << "\n";
    std::cout << "  x3 (MMAX)  = " << params(3) << "\n";
    std::cout << "  x4 (CACH)  = " << params(4) << "\n";
    std::cout << "  x5 (CHMIN) = " << params(5) << "\n";
    std::cout << "  x6 (CHMAX) = " << params(6) << "\n";

    // Evaluate
    double trainRMSE = lr.RMSE(params, trainSet);
    double testRMSE  = lr.RMSE(params, testSet);
    std::cout << "\nTrain RMSE: " << trainRMSE << "\n";
    std::cout << "Test  RMSE: " << testRMSE  << "\n";

    // Show some predictions
    std::cout << "\nSample predictions (first 5 test records):\n";
    std::cout << "  PRP_actual  |  PRP_predicted\n";
    std::cout << "  ----------- | --------------\n";
    int n = std::min((int)testSet.size(), 5);
    for (int i = 0; i < n; ++i) {
        double pred = lr.Predict(params, testSet[i]);
        std::cout << "  " << testSet[i].PRP << "\t\t| " << pred << "\n";
    }
}

// ── Main ──────────────────────────────────────────────────────────────────────
int main(int argc, char* argv[]) {
    // Part A
    testVector();
    testMatrix();
    testLinearSystem();
    testPosSymLinSystem();
    testUnderdetermined();

    // Part B: pass path to machine.data as first argument, or use default
    std::string dataPath = (argc > 1) ? argv[1] : "data/machine.data";
    runLinearRegression(dataPath);

    return 0;
}
