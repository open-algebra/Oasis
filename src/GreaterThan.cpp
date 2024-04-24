#include "GreaterThan.hpp"

namespace Oasis {

GreaterThan::GreaterThan(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
    : left(std::move(left)), right(std::move(right)) {}

auto GreaterThan::Copy() const -> std::unique_ptr<Expression> {
    return std::make_unique<GreaterThan>(left->Copy(), right->Copy());
}

auto GreaterThan::Equals(const Expression& other) const -> bool {
    if (auto o = dynamic_cast<const GreaterThan*>(&other)) {
        return left->Equals(*o->left) && right->Equals(*o->right);
    }
    return false;
}

auto GreaterThan::StructurallyEquivalent(const Expression& other) const -> bool {
    if (auto o = dynamic_cast<const GreaterThan*>(&other)) {
        return left->StructurallyEquivalent(*o->left) && right->StructurallyEquivalent(*o->right);
    }
    return false;
}

std::unique_ptr<Expression> GreaterThan::Simplify() const {
    auto simplified_left = left->Simplify();
    auto simplified_right = right->Simplify();
    // might consider additional simplification rules based on the operands
    return std::make_unique<GreaterThan>(std::move(simplified_left), std::move(simplified_right));
}

auto GreaterThan::ToString() const -> std::string {
    return "(" + left->ToString() + " > " + right->ToString() + ")";
}

const Expression* GreaterThan::GetLeftPtr() const {
    return left.get();
}

const Expression* GreaterThan::GetRightPtr() const {
    return right.get();
}

} // namespace Oasis