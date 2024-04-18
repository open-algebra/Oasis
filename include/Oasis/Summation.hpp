//
// Created by Andrew Choi on 3/15/24
//
#ifndef OASIS_SUMMATION_HPP
#define OASIS_SUMMATION_HPP

#include "Expression.hpp"

namespace Oasis {

class Summation : public Expression {
public:
    Summation(const Expression& lowBound, const Expression& upperBound, const Expression& exp);

    auto Simplify() const;
    auto ToString() const;

    auto Evaluate() const;



private:
    Expression lowBound;  // Lower bound for the summation
    Expression upperBound; // Upper bound for the summation
    Expression exp; // Expression being summed
};

} // namespace Oasis