#include "LessThan.hpp"

namespace Oasis {

LessThan::LessThan(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
    : left(std::move(left)), right(std::move(right)) {}

auto LessThan::Copy() const -> std::unique_ptr<Expression> {
    return std::make_unique<LessThan>(left->Copy(), right->Copy());
}

auto LessThan::Equals(const Expression& other) const -> bool {
    if (auto o = dynamic_cast<const LessThan*>(&other)) {
        return left->Equals(*o->left) && right->Equals(*o->right);
    }
    return false;
}

auto LessThan::StructurallyEquivalent(const Expression& other) const -> bool {
    if (auto o = dynamic_cast<const LessThan*>(&other)) {
        return left->StructurallyEquivalent(*o->left) && right->StructurallyEquivalent(*o->right);
    }
    return false;
}

std::unique_ptr<Expression> LessThan::Simplify() const {
    auto simplified_left = left->Simplify();
    auto simplified_right = right->Simplify();
    // might consider additional simplification rules based on the operands
    return std::make_unique<LessThan>(std::move(simplified_left), std::move(simplified_right));
}

auto LessThan::ToString() const -> std::string {
    return "(" + left->ToString() + " < " + right->ToString() + ")";
}

const Expression* LessThan::GetLeftPtr() const {
    return left.get();
}

const Expression* LessThan::GetRightPtr() const {
    return right.get();
}

} // namespace Oasis