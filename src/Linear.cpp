//
// Created by Andrew Nazareth on 2/16/24.
//

#include "Oasis/Linear.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/RecursiveCast.hpp"
#include "Oasis/SimplifyVisitor.hpp"
#include "Oasis/Variable.hpp"

namespace Oasis {

auto SolveLinearSystems(std::vector<std::unique_ptr<Expression>>& exprs) -> std::map<std::string, double>
{
    Oasis::SimplifyVisitor simplifyVisitor {};
    for (auto& expr : exprs) {
        auto simplified = expr->Accept(simplifyVisitor);
        expr = simplified.value()->Generalize();
    }
    auto matrices = ConstructMatrices(exprs);
    auto A = matrices.first.first;
    auto b = matrices.first.second;
    auto varMap = matrices.second;

    // get result vector from A^{-1}*b=x
    Matrix1D x = SolveLinearSystems(A, b);

    std::map<std::string, double> values;
    for (auto& kv : varMap) {
        auto key = kv.first;
        auto index = kv.second;

        values.insert(std::make_pair(key, x[Eigen::Index(index)]));
    }

    return values;
}

auto ConstructMatrices(const std::vector<std::unique_ptr<Expression>>& exprs)
    -> std::pair<std::pair<MatrixXXD, Matrix1D>, std::map<std::string, Eigen::Index>>
{
    size_t numRows = exprs.size();
    size_t numCols = exprs.size();

    MatrixXXD A(numRows, numCols);
    Matrix1D b(numRows);

    // init to 0 (to prevent unwanted behavior)
    for (Eigen::Index r = 0; r < A.rows(); r++) {
        for (Eigen::Index c = 0; c < A.cols(); c++) {
            A(r, c) = 0.0;
        }
    }
    for (Eigen::Index c = 0; c < b.rows(); c++) {
        b(c) = 0.0;
    }

    size_t varCount = 0;
    std::map<std::string, Eigen::Index> vars; // variable name, column in matrix
    for (size_t row = 0; row < exprs.size(); row++) {
        std::vector<std::unique_ptr<Expression>> terms;
        auto expr = RecursiveCast<Add<Expression>>(*exprs[row]);
        expr->Flatten(terms);
        for (auto& term : terms) {
            if (auto r = RecursiveCast<Real>(*term); r != nullptr) { // real number
                b[Eigen::Index(row)] = -1 * r->GetValue();
            } else if (auto v = RecursiveCast<Variable>(*term); v != nullptr) { // variable by itself (coefficient of 1)
                std::pair<size_t, bool> keyloc = GetMapValue<std::string, Eigen::Index>(vars, v->GetName(), varCount);
                if (keyloc.second)
                    varCount++;
                A(Eigen::Index(row), Eigen::Index(keyloc.first)) = 1;
            } else if (auto exprV = RecursiveCast<Multiply<Real, Variable>>(*term); exprV != nullptr) {
                // any expression times a variable
                std::pair<size_t, bool> keyloc = GetMapValue<std::string, Eigen::Index>(vars,
                    exprV->GetLeastSigOp().GetName(),
                    varCount);
                if (keyloc.second)
                    varCount++;
                A(Eigen::Index(row), Eigen::Index(keyloc.first)) = exprV->GetMostSigOp().GetValue();
            }
        }
    }

    return std::make_pair(std::make_pair(A, b), vars);
}

auto SolveLinearSystems(MatrixXXD& matrix) -> Matrix1D
{ // row echelon form
    size_t rows = matrix.rows();
    size_t cols = matrix.cols() - 1;

    if (rows != cols)
        return Matrix1D {}; // unsolvable

    // create matrices A and b
    MatrixXXD A(rows, cols);
    Matrix1D b(rows);

    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            A(Eigen::Index(row), Eigen::Index(col)) = matrix(Eigen::Index(row), Eigen::Index(col));
        }
    }
    for (size_t row = 0; row < rows; row++) {
        b(Eigen::Index(row)) = matrix(Eigen::Index(row), Eigen::Index(cols));
    }

    auto x = SolveLinearSystems(A, b);

    return x;
}

auto SolveLinearSystems(MatrixXXD& matrixA, Matrix1D& matrixb) -> Matrix1D
{
    if (matrixA.rows() != matrixb.rows())
        return Matrix1D {};

    Matrix1D x = matrixA.inverse() * matrixb;

    return x;
}

}