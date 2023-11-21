#include "Oasis/Expression.hpp"
#include "Oasis/BinaryExpression.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Exponent.hpp"
#include "taskflow/taskflow.hpp"

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


auto MakeBinaryExpression(Oasis::ExpressionType op, const Expression& expression1, const Expression& expression2)
{
    std::unique_ptr<Expression> result;

    switch (op)
    {
    case ExpressionType::Add:
        result = Oasis::Add(expression1, expression2).Generalize();
        break;
    case ExpressionType::Subtract:
        result = Oasis::Subtract(expression1, expression2).Generalize();
        break;
    case ExpressionType::Multiply:
        result = Oasis::Multiply(expression1, expression2).Generalize();
        break;
    case ExpressionType::Divide:
        result = Oasis::Divide(expression1, expression2).Generalize();
        break;
    case ExpressionType::Log:
        result = Oasis::Log(expression1, expression2).Generalize();
        break;
    case ExpressionType::Exponent:
        result = Oasis::Exponent(expression1, expression2).Generalize();
        break;
    default:
        throw std::logic_error("ERROR: ExpressionType not valid");
    }
    return result;
}


auto Expression::Insert(Oasis::ExpressionType op, const Oasis::Expression& expression2)
{
    if(CheckIfInsertShouldGoAboveOrBelow(op, *this)) {
        auto self = this->Generalize();
        return MakeBinaryExpression(op, *self, expression2);
    }

    std::unique_ptr<Expression> insert_node = this->Generalize();
    if(op == ExpressionType::Add)
    {
        auto parent = Oasis::Add<Oasis::Real, Oasis::Add<Oasis::Real>>::Specialize(*insert_node);
    }
    else (op == ExpressionType::Multiply)
    {
        auto parent = Oasis::Multiply<Oasis::Real, Oasis::Multiply<Oasis::Real>>::Specialize(*insert_node);
    }



    while(true)
    {
            if (insert_node->GetType() != ExpressionType::Add and insert_node->GetType() != ExpressionType::Multiply) //Possibly a check that can be a function
            {
                auto result = MakeBinaryExpression(op, *insert_node, expression2);

                //return this;
            }

            //insert_node = this->Specialize(insert_node);
            //insert_node->insertDirection = not(insert_node->insertDirection);
            //if (not(insert_node->insertDirection))
            //{
            //    insert_node = insert_node.GetLeastSigOp();

            //}
            //else {
            //    insert_node = insert_node.GetMostSigOp();
            //}

            //parent = insert_node;
            //parent->insertDirection = not(parent->insertDirection);

    }

}
// RECENTLY ADDED

} // namespace Oasis