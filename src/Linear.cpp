//
// Created by Andrew Nazareth on 2/16/24.
//

#include "Oasis/Linear.hpp"
#include "Eigen/Dense"

namespace Oasis {

typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> MatrixXE;

template <class... E>
auto SolveLinearSystems(const E(&... expressions)) -> std::map<std::string, float>
{
}
}