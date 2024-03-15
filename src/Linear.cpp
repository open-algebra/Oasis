//
// Created by Andrew Nazareth on 2/16/24.
//

#include "Oasis/Linear.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Multiply.hpp"
#include "iostream"

namespace Oasis {

auto SolveLinearSystems(std::vector<std::unique_ptr<Expression>>& exprs) -> std::map<std::string, float>
{
    for (auto& expr : exprs) {
        expr = expr->Simplify();
    }
    auto matrices = ConstructMatrix(exprs);
    auto A = matrices.first;
    auto b = matrices.second;

    return {};
}

auto ConstructMatrix(const std::vector<std::unique_ptr<Expression>>& exprs) -> std::pair<MatrixXXF, Matrix1F>
{
    size_t numRows = exprs.size();
    size_t numCols = exprs.size();

    MatrixXXF A;
    Matrix1F b;

    std::map<std::string, size_t> vars; // variable name, column in matrix
    for (size_t row = 0; row < exprs.size(); row++) {
        std::vector<std::unique_ptr<Expression>> terms;
        auto expr = Add<Expression>::Specialize(*exprs[row]);
        expr->Flatten(terms);
        for (auto& term : terms) {
            if (auto t = Real::Specialize(*term); t != nullptr) {
            }
        }
    }

    return std::make_pair(A, b);
}

}