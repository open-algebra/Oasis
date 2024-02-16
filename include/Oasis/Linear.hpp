//
// Created by Andrew Nazareth on 2/16/24.
//

#ifndef OASIS_LINEAR_HPP
#define OASIS_LINEAR_HPP

#include "Expression.hpp"
#include <map>
#include <string>

namespace Oasis {
template <class Expression...>
auto SolveLinearSystems(const Expression& expressions...) -> std::map<std::string, float>;

}

#endif // OASIS_LINEAR_HPP
