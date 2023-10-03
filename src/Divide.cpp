//
// Created by Matthew McCall on 8/10/23.
//
#include <map>
#include <vector>
#include "Oasis/Divide.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Variable.hpp"

namespace Oasis {

Divide<Expression>::Divide(const Expression& dividend, const Expression& divisor)
    : BinaryExpression(dividend, divisor)
{
}

auto Divide<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    auto simplifiedDividend = mostSigOp->Simplify();
    auto simplifiedDivider = leastSigOp->Simplify();

    Divide simplifiedDivide { *simplifiedDividend, *simplifiedDivider };


    //Factor the expression here
    //Rest is written assuming factor function is complete

    if (auto realCase = Divide<Real>::Specialize(simplifiedDivide); realCase != nullptr) {
        const Real& dividend = realCase->GetMostSigOp();
        const Real& divisor = realCase->GetLeastSigOp();

        return std::make_unique<Real>(dividend.GetValue() / divisor.GetValue());
    }

    if (auto likeTermsCase = Divide<Multiply<Real, Expression>>::Specialize(simplifiedDivide); likeTermsCase != nullptr) {
        const Oasis::IExpression auto& leftTerm = likeTermsCase->GetMostSigOp().GetLeastSigOp();
        const Oasis::IExpression auto& rightTerm = likeTermsCase->GetLeastSigOp().GetLeastSigOp();
        const Real& coefficient1 = likeTermsCase->GetMostSigOp().GetMostSigOp();
        const Real& coefficient2 = likeTermsCase->GetLeastSigOp().GetMostSigOp();
        
        if (leftTerm.Equals(rightTerm)) {
            return std::make_unique<Real>(coefficient1.GetValue() / coefficient2.GetValue());
        }
        std::unordered_map<std::string, int> variables;


        const Oasis::IExpression auto& holderLeft=likeTermsCase->GetMostSigOp().GetLeastSigOp(); 
        const Oasis::IExpression auto& holderRight=likeTermsCase->GetLeastSigOp().GetLeastSigOp();

        auto leftover=(Multiply<Variable, Expression>::Specialize(holderLeft))->GetMostSigOp();

        for(auto sortingLeft = Multiply<Variable, Expression>::Specialize(holderLeft); sortingLeft != nullptr;){
            if (auto it=variables.find(sortingLeft->GetMostSigOp().GetName()); it==variables.end())
                variables.insert(std::make_pair(sortingLeft->GetMostSigOp().GetName(),0));
            variables[sortingLeft->GetMostSigOp().GetName()]++;
            sortingLeft = Multiply<Variable, Expression>::Specialize(sortingLeft->GetLeastSigOp());
            if (sortingLeft==nullptr)
                leftover = *(Variable::Specialize(sortingLeft->GetLeastSigOp()));
        }
        if (auto it=variables.find(leftover.GetName()); it==variables.end())
            variables.insert(std::make_pair(leftover.GetName(),0));
        variables[leftover.GetName()]++;

        leftover=(Multiply<Variable, Expression>::Specialize(holderRight))->GetMostSigOp();

        for(auto sortingRight = Multiply<Variable, Expression>::Specialize(holderRight); sortingRight != nullptr;){
            if (auto it=variables.find(sortingRight->GetMostSigOp().GetName()); it==variables.end())
                variables.insert(std::make_pair(sortingRight->GetMostSigOp().GetName(),0));
            variables[sortingRight->GetMostSigOp().GetName()]--;
            sortingRight = Multiply<Variable, Expression>::Specialize(sortingRight->GetLeastSigOp());
            if (sortingRight==nullptr)
                leftover = *(Variable::Specialize(sortingRight->GetLeastSigOp()));
        }
        if (auto it=variables.find(leftover.GetName()); it==variables.end())
            variables.insert(std::make_pair(leftover.GetName(),0));
        variables[leftover.GetName()]--;


        std::vector<std::pair<std::string, int>> top;
        std::vector<std::pair<std::string, int>> bot;
        for (auto it=variables.begin(); it!=variables.end(); it++){
            if (it->second==0)
                continue;
            if (it->second>0)
                top.push_back(std::make_pair(it->first, it->second));
            if (it->second<0)
                bot.push_back(std::make_pair(it->first, -(it->second)));
        }
    
        if (bot.size()!=0 && top.size()!=0){
            return std::make_unique<Divide<Expression>>(Multiply<Expression>(Real(coefficient1.GetValue()/coefficient2.GetValue()), *(Recurse(top, 0, top.size()-1))),*(Recurse(bot, 0, bot.size()-1)));
        }
        if (top.size()!=0){
            return std::make_unique<Multiply<Expression>>(Real(coefficient1.GetValue()/coefficient2.GetValue()), *(Recurse(top, 0, top.size()-1)));
        }
        if (bot.size()!=0){
            return std::make_unique<Divide<Expression>>(Real(coefficient1.GetValue()/coefficient2.GetValue()),*(Recurse(bot, 0, bot.size()-1)));   
        }
        else{
            return std::make_unique<Real>(coefficient1.GetValue() / coefficient2.GetValue());
        }
    }
    return simplifiedDivide.Copy();
}




auto Divide<Expression>::Recurse(std::vector<std::pair<std::string, int>> varList, int front, int end) const -> std::unique_ptr<Expression>{
    if (front=end){
        if (varList[front].second==1)
            return std::make_unique<Variable>(varList[front].first);
        else
            return std::make_unique<Variable>(varList[front].first);
    }
    return std::make_unique<Multiply<Expression>>(*(Recurse(varList, front, (end+front)/2)), *(Recurse(varList,(end+front+1)/2, end)));
}
/*
auto Recurse(std::vector<std::pair<std::string, int>> varList, int front, int end) const -> std::unique_ptr<Expression>{
    if (front-end==1){
        if (varList[front].second==1)
            return std::make_unique<Multiply<Expression>>(Variable(varList[front].first), Variable(varList[end].first));
        else
            return std::make_unique<Multiply<Expression>>(Variable(varList[front].first), Variable(varList[end].first));
    }
    if (front-end==2){
        if (varList[front].second==1)
            return std::make_unique<Multiply<Expression>>(Multiply<Expression>(Variable(varList[front].first), Variable(varList[front+1].first)), Variable(varList[end].first));
        else
            return std::make_unique<Multiply<Expression>>(Multiply<Expression>(Variable(varList[front].first), Variable(varList[front+1].first)), Variable(varList[end].first));
    }
    return std::make_unique<Multiply<Expression>>(Recurse(varList, front, (end+front)/2), Recurse(varList,(end+front+1)/2, end));
    return std::make_unique<Variable>(varList[front].first);
}*/




auto Divide<Expression>::ToString() const -> std::string
{
    return fmt::format("({} / {})", mostSigOp->ToString(), leastSigOp->ToString());
}

auto Divide<Expression>::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    std::unique_ptr<Expression> simplifiedDividend, simplifiedDivisor;

    tf::Task leftSimplifyTask = subflow.emplace([this, &simplifiedDividend](tf::Subflow& sbf) {
        if (!mostSigOp) {
            return;
        }

        simplifiedDividend = mostSigOp->Simplify(sbf);
    });

    tf::Task rightSimplifyTask = subflow.emplace([this, &simplifiedDivisor](tf::Subflow& sbf) {
        if (!leastSigOp) {
            return;
        }

        simplifiedDivisor = leastSigOp->Simplify(sbf);
    });

    Divide simplifiedDivide;

    // While this task isn't actually parallelized, it exists as a prerequisite for check possible cases in parallel
    tf::Task simplifyTask = subflow.emplace([&simplifiedDivide, &simplifiedDividend, &simplifiedDivisor](tf::Subflow& sbf) {
        if (simplifiedDividend) {
            simplifiedDivide.SetMostSigOp(*simplifiedDividend);
        }

        if (simplifiedDivisor) {
            simplifiedDivide.SetLeastSigOp(*simplifiedDivisor);
        }
    });

    simplifyTask.succeed(leftSimplifyTask, rightSimplifyTask);

    std::unique_ptr<Divide<Real>> realCase;

    tf::Task realCaseTask = subflow.emplace([&simplifiedDivide, &realCase](tf::Subflow& sbf) {
        realCase = Divide<Real>::Specialize(simplifiedDivide, sbf);
    });

    simplifyTask.precede(realCaseTask);

    subflow.join();

    if (realCase) {
        const Real& multiplicand = realCase->GetMostSigOp();
        const Real& multiplier = realCase->GetLeastSigOp();

        return std::make_unique<Real>(multiplicand.GetValue() / multiplier.GetValue());
    }

    return simplifiedDivide.Copy();
}

auto Divide<Expression>::Specialize(const Expression& other) -> std::unique_ptr<Divide<Expression, Expression>>
{
    if (!other.Is<Divide>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize();
    return std::make_unique<Divide>(dynamic_cast<const Divide&>(*otherGeneralized));
}

auto Divide<Expression>::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Divide>
{
    if (!other.Is<Divide>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize(subflow);
    return std::make_unique<Divide>(dynamic_cast<const Divide&>(*otherGeneralized));
}

} // Oasis