//
// Created by Matthew McCall on 4/28/24.
//

#ifndef INFIXSERIALIZER_HPP
#define INFIXSERIALIZER_HPP

#include <string>

#include "Oasis/Visit.hpp"

namespace Oasis {

class InFixSerializer final : public Visitor {
public:
    void Visit(const Real& real) override;
    void Visit(const Imaginary& imaginary) override;
    void Visit(const Variable& variable) override;
    void Visit(const Undefined& undefined) override;
    void Visit(const Add<Expression, Expression>& add) override;
    void Visit(const Subtract<Expression, Expression>& subtract) override;
    void Visit(const Multiply<Expression, Expression>& multiply) override;
    void Visit(const Divide<Expression, Expression>& divide) override;
    void Visit(const Exponent<Expression, Expression>& exponent) override;
    void Visit(const Log<Expression, Expression>& log) override;
    void Visit(const Negate<Expression>& negate) override;
    void Visit(const Derivative<Expression, Expression>& derivative) override;
    void Visit(const Integral<Expression, Expression>& integral) override;

    [[nodiscard]] std::string getResult() const;

private:
    std::string result;
};

} // Oasis

#endif // INFIXSERIALIZER_HPP
