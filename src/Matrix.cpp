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

Matrix::Matrix(size_t numRows, size_t numCols)
{
    matrix = MatrixXXD(numRows, numCols);
}

Matrix::Matrix(size_t numRows, size_t numCols, std::vector<double>& vals)
{
    matrix = MatrixXXD(numRows, numCols);
    size_t count = numRows * numCols;
    size_t index = 0;
    for (; index < count; index++) {
        if (index < vals.size()) {
            matrix(static_cast<long>(index / numCols), static_cast<long>(index % numCols)) = vals[index];
        } else {
            matrix(static_cast<long>(index / numCols), static_cast<long>(index % numCols)) = 0;
        }
    }
}

// TO DO: Fix?
auto Matrix::Differentiate(const Expression&) const -> std::unique_ptr<Expression>
{
    return std::make_unique<Real>(0);
}

auto Matrix::Equals(const Expression& other) const -> bool
{
    return other.Is<Matrix>() && matrix.rows() == dynamic_cast<const Matrix&>(other).matrix.rows()
        && matrix.cols() == dynamic_cast<const Matrix&>(other).matrix.cols()
        && matrix == dynamic_cast<const Matrix&>(other).matrix;
}

auto Matrix::GetMatrix() const -> MatrixXXD
{
    return matrix;
}

auto Matrix::GetRows() const -> size_t
{
    return matrix.rows();
}

auto Matrix::GetCols() const -> size_t
{
    return matrix.cols();
}

auto Matrix::Transpose() const -> std::unique_ptr<Matrix>
{
    return std::make_unique<Matrix>(matrix.transpose());
}

auto Matrix::Inverse() const -> std::unique_ptr<Matrix>
{
    return std::make_unique<Matrix>(matrix.inverse());
}

auto Matrix::Specialize(const Expression& other) -> std::unique_ptr<Matrix>
{
    return other.Is<Matrix>() ? std::make_unique<Matrix>(dynamic_cast<const Matrix&>(other)) : nullptr;
}

// TO DO: Fix?
auto Matrix::Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    Integral<Expression, Expression> integral { *(this->Copy()), *(integrationVariable.Copy()) };

    return integral.Copy();
}

auto Matrix::Identity() const -> std::unique_ptr<Expression>
{
    MatrixXXD identityMatrix = MatrixXXD(GetRows(), GetCols());
    for (size_t x = 0; x < GetRows(); x++) {
        for (size_t y = 0; y < GetCols(); y++) {
            identityMatrix(static_cast<Eigen::Index>(x), static_cast<Eigen::Index>(y)) = (x == y ? 1.0 : 0.0);
        }
    }
    return std::make_unique<Matrix>(identityMatrix);
}

} // namespace Oasis