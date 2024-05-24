//
// Created by Andrew Nazareth on 5/24/24.
//
#include "Oasis/Matrix.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Real.hpp"

namespace Oasis {

Matrix::Matrix(MatrixXXD other)
    : matrix(other)
{
}

Matrix::Matrix(int numRows, int numCols){
    matrix = MatrixXXD(numRows, numCols);
}

// TODO: Fix
auto Matrix::Differentiate(const Expression&) const -> std::unique_ptr<Expression>
{
    return std::make_unique<Real>(0);
}

auto Matrix::Equals(const Expression& other) const -> bool
{
    return other.Is<Matrix>() && matrix == dynamic_cast<const Matrix&>(other).matrix;
}

auto Matrix::GetMatrix() const -> MatrixXXD
{
    return matrix;
}

auto Matrix::Specialize(const Expression& other) -> std::unique_ptr<Matrix>
{
    return other.Is<Matrix>() ? std::make_unique<Matrix>(dynamic_cast<const Matrix&>(other)) : nullptr;
}

auto Matrix::Specialize(const Expression& other, tf::Subflow&) -> std::unique_ptr<Matrix>
{
    return other.Is<Matrix>() ? std::make_unique<Matrix>(dynamic_cast<const Matrix&>(other)) : nullptr;
}

// TODO: Fix
auto Matrix::Integrate(const Expression& integrationVariable) -> std::unique_ptr<Expression>
{
    Integral<Expression, Expression> integral { *(this->Copy()), *(integrationVariable.Copy()) };

    return integral.Copy();
}

} // namespace Oasis