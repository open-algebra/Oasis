//
// Created by Andrew Choi on 3/15/24
//
#ifndef OASIS_SUMMATION_HPP
#define OASIS_SUMMATION_HPP

namespace Oasis {
/**
*The concept of a Capital-sigma notation for summation
*@tparam lowBound is the starting iteration
*@tparam upperBound is the ending iteration 
*@tparam expression is the expression that is being summed
*/
template <IExpression lowBound, IExpression upperBound, IExpression expression>
class Summation; 

class Summation {

public: 
    Summation() = default;
    Summation(const Summation& other) { 
        if(other.)
    }

    [[nodiscard]] auto HasLowerBound() const -> bool
    {
        return lowBound != nullptr;
    }

    [[nodiscard]] auto HasUpperBound() const -> bool
    {
        return upperBound != nullptr;
    }




};







} //namespace Oasis