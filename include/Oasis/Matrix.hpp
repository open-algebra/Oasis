//
// Created by Andrew Nazareth on 5/24/24.
//

#ifndef OASIS_MATRIX_HPP
#define OASIS_MATRIX_HPP
#include "Eigen/Dense"
#include "LeafExpression.hpp"

namespace Oasis {
typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixXXD;
typedef Eigen::Matrix<double, Eigen::Dynamic, 1> Matrix1D;
/**
 * A matrix
 */
class Matrix : public LeafExpression<Matrix> {
public:
    Matrix() = default;
    Matrix(const Matrix& other) = default;
    Matrix(size_t numRows, size_t numCols);
    Matrix(size_t numRows, size_t numCols, std::vector<double>& vals);

    explicit Matrix(MatrixXXD other);

    [[nodiscard]] auto Equals(const Expression& other) const -> bool final;

    EXPRESSION_TYPE(Matrix)
    EXPRESSION_CATEGORY(UnExp)

    /**
     * Gets the matrix.
     * @return The matrix.
     */
    [[nodiscard]] auto GetMatrix() const -> MatrixXXD;

    /**
     * Gets the number of rows
     * @return number of rows
     */
    [[nodiscard]] auto GetRows() const -> size_t;

    /**
     * Gets the number of columns
     * @return number of columns
     */
    [[nodiscard]] auto GetCols() const -> size_t;

    /**
     * Gets the matrix's transpose
     * @return Unique pointer to a Matrix
     */
    [[nodiscard]] auto Transpose() const -> std::unique_ptr<Matrix>;

    /**
     * Gets the matrix's inverse
     * @return Unique pointer to a Matrix
     */
    [[nodiscard]] auto Inverse() const -> std::unique_ptr<Matrix>;

    [[nodiscard]] auto Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Identity() const -> std::unique_ptr<Expression>;

    static auto Specialize(const Expression& other) -> std::unique_ptr<Matrix>;
    [[nodiscard]] auto Differentiate(const Expression&) const -> std::unique_ptr<Expression> final;

    auto operator=(const Matrix& other) -> Matrix& = default;

private:
    MatrixXXD matrix {};
};

} // Oasis
#endif // OASIS_MATRIX_HPP
