#include "taskflow/taskflow.hpp"
#include "Oasis/BinaryExpression.hpp"
#include "Oasis/Expression.hpp"

namespace Oasis {

auto Expression::GetCategory() const -> uint32_t
{
    return 0;
}

auto Expression::GetType() const -> ExpressionType
{
    return ExpressionType::None;
}

auto Expression::Generalize() const -> std::unique_ptr<Expression>
{
    return Copy();
}

auto Expression::Generalize(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    return Copy(subflow);
}

auto Expression::Specialize(const Expression& other) -> std::unique_ptr<Expression>
{
    return other.Copy();
}

auto Expression::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Expression>
{
    return other.Copy(subflow);
}

auto Expression::Simplify() const -> std::unique_ptr<Expression>
{
    return Copy();
}

auto Expression::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    return Copy(subflow);
}

auto Expression::SimplifyAsync() const -> std::unique_ptr<Expression>
{
    static tf::Executor executor;
    tf::Taskflow taskflow;

    std::unique_ptr<Expression> simplifiedExpression;

    taskflow.emplace([this, &simplifiedExpression](tf::Subflow& subflow) {
        simplifiedExpression = Simplify(subflow);
    });

    executor.run(taskflow).wait();
    return simplifiedExpression;
}

auto Expression::StructurallyEquivalentAsync(const Expression& other) const -> bool
{
    static tf::Executor executor;
    tf::Taskflow taskflow;

    bool equivalent = false;

    taskflow.emplace([&equivalent, this, &other](tf::Subflow& subflow) {
        equivalent = StructurallyEquivalent(other, subflow);
    });

    executor.run(taskflow).wait();
    return equivalent;
}

// RECENTLY ADDED
auto CheckIfInsertShouldGoAboveOrBelow(Oasis::ExpressionType op, const Oasis::Expression& self) -> bool
{
    if (op == ExpressionType::Add or op == ExpressionType::Multiply)
        if (op == self.GetType())
            return false;
    return true;
}

template <IExpression T> auto Expression::Insert(Oasis::ExpressionType op, const Oasis::Expression& exp)
{
    if(CheckIfInsertShouldGoAboveOrBelow(op, *this))
    {
        this = Oasis::BinaryExpression<T>(this, exp);
    }

}
// RECENTLY ADDED

} // namespace Oasis