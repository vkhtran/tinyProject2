# Project 1 — Linear Algebra Toolkit and Linear Regression

## How to run

```bash
cd "Project 1"
make        # build
make run    # build and run with data/machine.data
make clean  # remove the binary
```

Requires a C++17 compiler and `make`.

## Files

**Source**

| File | Purpose |
| --- | --- |
| `src/main.cpp` | Entry point. Runs unit tests for all classes, then trains and evaluates the regression model. |
| `src/Vector.cpp` | Dynamic vector with arithmetic operators, dot product, and norm. |
| `src/Matrix.cpp` | Dynamic matrix with arithmetic, determinant, inverse, transpose, and pseudoinverse. |
| `src/LinearSystem.cpp` | Solves `Ax = b` for square systems using Gaussian elimination with partial pivoting. |
| `src/PosSymLinSystem.cpp` | Solves symmetric positive definite systems using the Conjugate Gradient method. |
| `src/LinearRegression.cpp` | Loads the dataset, builds the normal equations, solves via `LinearSystem`, and computes RMSE. |

**Headers** (`include/`) — declarations for each class above.

**Data**

| File | Purpose |
| --- | --- |
| `data/machine.data` | UCI Computer Hardware dataset (209 records) used to train and test the regression model. |

**Other**

| File | Purpose |
| --- | --- |
| `Makefile` | Build rules: `all`, `run`, `clean`. |
| `Report Project 1.pdf` | Written project report. |
