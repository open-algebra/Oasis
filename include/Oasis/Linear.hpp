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

typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixXXD;
typedef Eigen::Matrix<double, Eigen::Dynamic, 1> Matrix1D;

/**
 * @param exprs A vector of expressions
 * @return map of variable to their values
 */
auto SolveLinearSystems(std::vector<std::unique_ptr<Expression>>& exprs) -> std::map<std::string, double>;

/**
 *
 * @param exprs A vector of expressions
 * @return Dynamic Float matrix with the provided expressions inserted
 */
auto ConstructMatrices(const std::vector<std::unique_ptr<Expression>>& exprs)
    -> std::pair<std::pair<MatrixXXD, Matrix1D>, std::map<std::string, size_t>>;

/**
 *
 * @tparam u key type
 * @tparam v value type
 * @param map A map that maps some key to a value
 * @param key The key you are searching for
 * @param nextValue What the next assigned value should be assuming that the key doesn't already have one. If no value
 * is found, this value will be assigned to this key.
 * @return returns the value that is associated with the key and a boolean to say whether the nextValue was used.
 */
template <class u, class v>
auto GetMapValue(std::map<u, v>& map, u key, v nextValue) -> std::pair<v, bool>
{
    v value;
    bool usedNextValue = false;
    if (auto p = map.find(key); p != map.end()) { // key is found
        value = p->second;
    } else { // key is not found
        map.insert(std::make_pair(key, nextValue));
        value = nextValue;
        usedNextValue = true;
    }
    return std::make_pair(value, usedNextValue);
}

}

#endif // OASIS_LINEAR_HPP
