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

void MakeBinaryExpression(ExpressionType op, std::unique_ptr<Expression>& self, const Expression& exp)
{
    switch(op)
    {
        case ExpressionType::Add:
            self = Add<Expression>(self, exp);
        case ExpressionType::Subtract:
            self = Subtract<Expression>(self, exp);
        case ExpressionType::Multiply:
            self = Multiply<Expression>(self, exp);
        case ExpressionType::Divide:
            self = Divide<Expression>(self, exp);
        case ExpressionType::Log:
            self = Log<Expression>(self, exp);
        case ExpressionType::Exponent:
            self = Exponent<Expression>(self, exp);
        default:
            throw std::logic_error("ERROR op value not valid");
    }
}


auto Expression::Insert(Oasis::ExpressionType op, const Oasis::Expression& exp)
{
    if(CheckIfInsertShouldGoAboveOrBelow(op, *this)) {
            MakeBinaryExpression(op, *this, exp);
            return this;
    }
    std::unique_ptr<Expression> insert_node;
    //std::unique_ptr<BinaryExpression> parent;
//    if(this->GetType() == ExpressionType::Add)
//    {
//            insert_node = this->Specialize(Add<Expression>());
//            parent = this->Specialize(Add<Expression>());
//    }
//    else
//    {
//            insert_node = this->Specialize(Multiply<Expression>());
//            parent = this->Specialize(Multiply<Expression>());
//    }

    while(true)
    {
            if (insert_node->GetType() != ExpressionType::Add and insert_node->GetType() != ExpressionType::Multiply) //Possibly a check that can be a function
            {
//                if (parent->insertDirection)
//                {
                    MakeBinaryExpression(op, *insert_node, exp);
                    break;
//                }
//                else
//                {
//                    MakeBinaryExpression(op, *exp, insert_node);
//                    break;
//                }

            }

            insert_node = this->Specialize(BinaryExpression);
            insert_node->insertDirection = not(insert_node->insertDirection);
            if (not(insert_node->insertDirection))
            {
                    insert_node = insert_node.GetLeastSigOp();
//                    if(insert_node->GetType() != ExpressionType::Add)
//                    {
//                        auto addNode = dynamic_cast<Add<Expression>*>(insert_node.get());
//                        insert_node = addNode->GetMostSigOp();
//                    }
//                    else
//                    {
//                        auto mulNode = dynamic_cast<Multiply<Expression>*>(insert_node.get());
//                        insert_node = mulNode->GetMostSigOp();
//                    }
//            }
//            else
//            {
//                    if(insert_node->GetType() != ExpressionType::Add)
//                    {
//                        auto addNode = dynamic_cast<Add<Expression>*>(insert_node.get());
//                        insert_node = addNode->GetLeastSigOp();
//                    }
//                    else
//                    {
//                        auto mulNode = dynamic_cast<Multiply<Expression>*>(insert_node.get());
//                        insert_node = mulNode->GetLeastSigOp();
//                    }
            }
            else {
                    insert_node = insert_node.GetMostSigOp();
            }

            //parent = insert_node;
            //parent->insertDirection = not(parent->insertDirection);

    }

}
// RECENTLY ADDED

} // namespace Oasis