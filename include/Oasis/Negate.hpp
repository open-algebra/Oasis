//
// Created by Andrew Nazareth on 3/26/24.
//

#ifndef OASIS_NEGATE_HPP
#define OASIS_NEGATE_HPP

#include "Expression.hpp"
#include "Subtract.hpp"
#include "Add.hpp"

namespace Oasis {
    auto NegateSubtract(const std::unique_ptr<Subtract<Expression>> &expr) -> std::unique_ptr<Expression>;
}

#endif //OASIS_NEGATE_HPP
