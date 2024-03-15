//
// Created by Andrew Nazareth on 2/16/24.
//

#ifndef OASIS_LINEAR_HPP
#define OASIS_LINEAR_HPP

#include "Eigen/Dense"
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Expression.hpp"

namespace Oasis {

typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixXXF;
typedef Eigen::Matrix<float, Eigen::Dynamic, 1> Matrix1F;

/**
 * @param exprs A vector of expressions
 * @return map of variable to their values
 */
auto SolveLinearSystems(std::vector<std::unique_ptr<Expression>>& exprs) -> std::map<std::string, float>;

/**
 *
 * @param exprs A vector of expressions
 * @return Dynamic Float matrix with the provided expressions inserted
 */
auto ConstructMatrix(const std::vector<std::unique_ptr<Expression>>& exprs) -> std::pair<MatrixXXF, Matrix1F>;

}

#endif // OASIS_LINEAR_HPP
