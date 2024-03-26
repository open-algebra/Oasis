//
// Created by Andrew Nazareth on 3/26/24.
//

#include <memory>
#include "Oasis/Negate.hpp"
#include "Oasis/Multiply.hpp"

namespace Oasis {
    auto NegateSubtract(const std::unique_ptr<Subtract<Expression>> &expr) -> std::unique_ptr<Expression> {
        return Add{expr->GetMostSigOp(),
                   *Multiply<Expression>{expr->GetLeastSigOp(), Real{-1.0}}.Simplify()}.Generalize();
    }
}
