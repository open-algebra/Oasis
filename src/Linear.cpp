//
// Created by Andrew Nazareth on 2/16/24.
//

#include "Oasis/Linear.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Variable.hpp"
#include "iostream"

namespace Oasis {

auto SolveLinearSystems(std::vector<std::unique_ptr<Expression>>& exprs) -> std::map<std::string, double>
{
    for (auto& expr : exprs) {
        expr = expr->Simplify();
    }
    auto matrices = ConstructMatrices(exprs);
    auto A = matrices.first.first;
    auto b = matrices.first.second;
    auto varMap = matrices.second;

    // get result vector from A^-1*b=x
    Matrix1D x = A.inverse() * b;

    std::map<std::string, double> values;
    for (auto& kv : varMap) {
        auto key = kv.first;
        auto index = kv.second;

        values.insert(std::make_pair(key, x[index]));
    }

    return values;
}

auto ConstructMatrices(const std::vector<std::unique_ptr<Expression>>& exprs)
    -> std::pair<std::pair<MatrixXXD, Matrix1D>, std::map<std::string, size_t>>
{
    size_t numRows = exprs.size();
    size_t numCols = exprs.size();

    MatrixXXD A(numRows, numCols);
    Matrix1D b(numRows);
    size_t varCount = 0;
    std::map<std::string, size_t> vars; // variable name, column in matrix
    for (size_t row = 0; row < exprs.size(); row++) {
        std::vector<std::unique_ptr<Expression>> terms;
        auto expr = Add<Expression>::Specialize(*exprs[row]);
        expr->Flatten(terms);
        for (auto& term : terms) {
            if (auto r = Real::Specialize(*term); r != nullptr) { // real number
                b[row] = -1 * r->GetValue();
            } else if (auto v = Variable::Specialize(*term); v != nullptr) { // variable by itself (coefficient of 1)
                std::pair<size_t, bool> keyloc = GetMapValue<std::string, size_t>(vars, v->GetName(), varCount);
                if (keyloc.second)
                    varCount++;
                A(row, keyloc.first) = 1;
            } else if (auto exprV = Multiply<Real, Variable>::Specialize(*term); exprV != nullptr) {
                // any expression times a variable
                std::pair<size_t, bool> keyloc = GetMapValue<std::string, size_t>(vars, exprV->GetLeastSigOp().GetName(), varCount);
                if (keyloc.second)
                    varCount++;
                A(row, keyloc.first) = exprV->GetMostSigOp().GetValue();
            }
        }
    }

    return std::make_pair(std::make_pair(A, b), vars);
}

}