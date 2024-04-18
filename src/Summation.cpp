//
//Created by Andrew Choi on 4/2/24
//

#include "Oasis/Summation.hpp"

namespace Oasis {

Summation::Summation(const Expression& lowBound, const Expression& upperBound, const Expression& exp)
    : lowBound(lowBound), upperBound(upperBound), exp(exp) {}

auto Summation::Simplify() const {
    auto simplifiedLow = lowBound->Simplify();
    auto simplifiedUp = upperBound->Simplify();
    auto simplifiedExp = exp->Simplify();

    return std::make_unique<Summation>(*simplifiedLow, *simplfiiedUp, *simplifiedExp);
}

auto Summation::ToString() const {
    return "∑^{" + lowBound->ToString() + "}_{" + upperBound->ToString() + "}(" + exp->ToString() + ")";
}

auto Summation::Evaluate() const {
    std::unique_ptr<Summation> result = make_unique<Summation>();
    for(auto i = lowBound->Evaluate(); i < upperBound -> Evaluate(); ++i) {
        result = result + exp->Evaluate();
    }

    return result;
}

} // namespace Oasis