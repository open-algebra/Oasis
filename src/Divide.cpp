//
// Created by Matthew McCall on 8/10/23.
//
#include <map>
#include <vector>
#include "Oasis/Divide.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/Exponent.hpp"

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
        std::unordered_map<std::string, double> variables;


        const Oasis::IExpression auto& holderLeft=likeTermsCase->GetMostSigOp().GetLeastSigOp(); 
        const Oasis::IExpression auto& holderRight=likeTermsCase->GetLeastSigOp().GetLeastSigOp();

        auto leftover=holderLeft.Generalize();

        std::list<std::pair<std::unique_ptr<Expression>, double>> topexpress;
        

        //Variables
        for(auto sortingLeft = Multiply<Variable, Expression>::Specialize(holderLeft); sortingLeft != nullptr;){
            if (auto it=variables.find(sortingLeft->GetMostSigOp().GetName()); it==variables.end())
                variables.insert(std::make_pair(sortingLeft->GetMostSigOp().GetName(),0));
            variables[sortingLeft->GetMostSigOp().GetName()]++;
            leftover = sortingLeft->GetLeastSigOp().Generalize();
            sortingLeft = Multiply<Variable, Expression>::Specialize(sortingLeft->GetLeastSigOp());
        }

        //Exponents
        for (auto sortingLeft=Multiply<Exponent<Variable, Real>, Expression>::Specialize(*leftover); sortingLeft != nullptr;){
            if (auto it=variables.find(sortingLeft->GetMostSigOp().GetMostSigOp().GetName()); it==variables.end())
                variables.insert(std::make_pair(sortingLeft->GetMostSigOp().GetMostSigOp().GetName(),0));
            variables[sortingLeft->GetMostSigOp().GetMostSigOp().GetName()]+=sortingLeft->GetMostSigOp().GetLeastSigOp().GetValue();
            leftover = sortingLeft->GetLeastSigOp().Generalize();
            sortingLeft = Multiply<Exponent<Variable, Real>, Expression>::Specialize(sortingLeft->GetLeastSigOp());
        }

        //Expressions
        for (auto sortingLeft=Multiply<Expression, Expression>::Specialize(*leftover); sortingLeft != nullptr;){
            if (sortingLeft->GetLeastSigOp().GetType()==ExpressionType::Exponent){
                const auto& sortingLeftLeastSigOp = dynamic_cast<const Exponent<Expression>&>(*sortingLeft->Generalize());
                double val = dynamic_cast<const Real&>(sortingLeftLeastSigOp.GetMostSigOp()).GetValue();
                topexpress.push_back(std::make_pair(sortingLeftLeastSigOp.GetLeastSigOp().Copy(), val));
            }
            else{
                topexpress.push_back(std::make_pair(sortingLeft->GetLeastSigOp().Copy(), 1));
            }
            leftover = sortingLeft->GetMostSigOp().Generalize();
            sortingLeft = Multiply<Expression, Expression>::Specialize(sortingLeft->GetLeastSigOp());
        }



        if (leftover->GetType()==ExpressionType::Variable){
            auto temp=Variable::Specialize(*leftover);
            if (auto it=variables.find(temp->GetName()); it==variables.end())
                variables.insert(std::make_pair(temp->GetName(),0));
            variables[temp->GetName()]++;
        }
        else if (auto temp=Exponent<Variable, Real>::Specialize(*leftover); temp!= nullptr){
            if (auto it=variables.find(temp->GetMostSigOp().GetName()); it==variables.end())
                variables.insert(std::make_pair(temp->GetMostSigOp().GetName(),0));
            variables[temp->GetMostSigOp().GetName()]+=temp->GetLeastSigOp().GetValue();
        }
        else{
            auto check=Exponent<Expression, Real>::Specialize(*leftover);
            if (check!=nullptr){
                const auto& sortingLeftLeastSigOp = dynamic_cast<const Exponent<Expression>&>(*check->Generalize());
                double val = dynamic_cast<const Real&>(sortingLeftLeastSigOp.GetMostSigOp()).GetValue();
                topexpress.push_back(std::make_pair(sortingLeftLeastSigOp.GetLeastSigOp().Copy(), val));
            }
            else{
                topexpress.push_back(std::make_pair(leftover->Generalize(), 1));
            }
        }

        leftover=holderRight.Generalize();
        for(auto sortingRight = Multiply<Variable, Expression>::Specialize(holderRight); sortingRight != nullptr;){
            if (auto it=variables.find(sortingRight->GetMostSigOp().GetName()); it==variables.end())
                variables.insert(std::make_pair(sortingRight->GetMostSigOp().GetName(),0));
            variables[sortingRight->GetMostSigOp().GetName()]--;
            leftover = Variable::Specialize(sortingRight->GetLeastSigOp());
            sortingRight = Multiply<Variable, Expression>::Specialize(sortingRight->GetLeastSigOp());
        }
        for (auto sortingRight=Multiply<Exponent<Variable, Real>, Expression>::Specialize(*leftover); sortingRight != nullptr;){
            if (auto it=variables.find(sortingRight->GetMostSigOp().GetMostSigOp().GetName()); it==variables.end())
                variables.insert(std::make_pair(sortingRight->GetMostSigOp().GetMostSigOp().GetName(),0));
            variables[sortingRight->GetMostSigOp().GetMostSigOp().GetName()]+=sortingRight->GetMostSigOp().GetLeastSigOp().GetValue();
            leftover = sortingRight->GetLeastSigOp().Generalize();
            sortingRight = Multiply<Exponent<Variable, Real>, Expression>::Specialize(sortingRight->GetLeastSigOp());
        }
        for (auto sortingRight=Multiply<Expression, Expression>::Specialize(*leftover); sortingRight != nullptr;){
            bool checked=true;
            if (sortingRight->GetLeastSigOp().GetType()==ExpressionType::Exponent){
                const auto& sortingRightLeastSigOp = dynamic_cast<const Exponent<Expression>&>(*sortingRight->Generalize());
                double val = dynamic_cast<const Real&>(sortingRightLeastSigOp.GetMostSigOp()).GetValue();
                std::list<std::pair<std::unique_ptr<Expression>, double>>::iterator it;
                for (it = topexpress.begin(); it != topexpress.end(); ++it){
                    if (it->first==sortingRightLeastSigOp.GetLeastSigOp().Copy()){
                        checked=false;
                        topexpress.push_back(std::make_pair(sortingRightLeastSigOp.GetLeastSigOp().Copy(), it->second-val));
                        topexpress.erase(it);
                    }
                }
                if (checked){
                    topexpress.push_back(std::make_pair(sortingRightLeastSigOp.GetLeastSigOp().Copy(), -val));
                }
            }
            else{
                std::list<std::pair<std::unique_ptr<Expression>, double>>::iterator it;
                for (it = topexpress.begin(); it != topexpress.end(); ++it){
                    if (it->first==sortingRight->GetLeastSigOp().Copy()){
                        bool checked=false;
                        topexpress.push_back(std::make_pair(sortingRight->GetLeastSigOp().Copy(), it->second-1));
                        topexpress.erase(it);
                    }
                }
                if (checked){
                    topexpress.push_back(std::make_pair(sortingRight->GetLeastSigOp().Copy(), -1));
                }
            }
            leftover = sortingRight->GetLeastSigOp().Generalize();
            sortingRight = Multiply<Expression, Expression>::Specialize(sortingRight->GetLeastSigOp());
        }
        if (leftover->GetType()==ExpressionType::Variable){
            auto temp=Variable::Specialize(*leftover);
            if (auto it=variables.find(temp->GetName()); it==variables.end())
                variables.insert(std::make_pair(temp->GetName(),0));
            variables[temp->GetName()]--;
        }
        else if (auto temp=Exponent<Variable, Real>::Specialize(*leftover); temp!= nullptr){
            if (auto it=variables.find(temp->GetMostSigOp().GetName()); it==variables.end())
                variables.insert(std::make_pair(temp->GetMostSigOp().GetName(),0));
            variables[temp->GetMostSigOp().GetName()]-=temp->GetLeastSigOp().GetValue();
        }
        else{
            auto check=Exponent<Expression, Real>::Specialize(*leftover);
            bool checked=true;
            if (check!=nullptr){
                const auto& sortingRightLeastSigOp = dynamic_cast<const Exponent<Expression>&>(*check->Generalize());
                double val = dynamic_cast<const Real&>(sortingRightLeastSigOp.GetMostSigOp()).GetValue();
                std::list<std::pair<std::unique_ptr<Expression>, double>>::iterator it;
                for (it = topexpress.begin(); it != topexpress.end(); ++it){
                    if (it->first==sortingRightLeastSigOp.GetLeastSigOp().Copy()){
                        checked=false;
                        topexpress.push_back(std::make_pair(sortingRightLeastSigOp.GetLeastSigOp().Copy(), it->second-val));
                        topexpress.erase(it);
                    }
                }
                if (checked){
                    topexpress.push_back(std::make_pair(sortingRightLeastSigOp.GetLeastSigOp().Copy(), -val));
                }
            }
            else{
                std::list<std::pair<std::unique_ptr<Expression>, double>>::iterator it;
                for (it = topexpress.begin(); it != topexpress.end(); ++it){
                    if ((*(it->first)).Equals(*leftover->Generalize())){
                        checked=false;
                        topexpress.push_back(std::make_pair(leftover->Generalize(), it->second-1));
                        topexpress.erase(it);
                        break;
                    }
                }
                if (checked){
                    topexpress.push_back(std::make_pair(leftover->Generalize(), -1));
                }
            }
        }
        std::vector<std::unique_ptr<Expression>> top;
        std::vector<std::unique_ptr<Expression>> bot;
        
        for (auto it=topexpress.begin(); it!=topexpress.end(); it++){
            if (it->second==0)
                continue;
            else if (it->second==1)
                top.push_back(std::move(it->first));
            else if (it->second==-1)
                bot.push_back(std::move(it->first));
            else if (it->second>0){
                std::unique_ptr<Expression> filler=std::move(it->first);
                top.push_back(std::make_unique<Exponent<Expression/*, Real*/>>(*filler, Real(it->second)));
            }
            else if (it->second<0)
                bot.push_back(std::make_unique<Exponent<Expression/*, Real*/>>(*(it->first), Real(-1*(it->second))));
        }

        for (auto it=variables.begin(); it!=variables.end(); it++){
            if (it->second==0)
                continue;
            else if (it->second==1)
                top.push_back(std::make_unique<Variable>(it->first));
            else if (it->second==-1)
                bot.push_back(std::make_unique<Variable>(it->first));
            else if (it->second>0)
                top.push_back(std::make_unique<Exponent<Variable,Real>>(Variable(it->first), Real(it->second)));
            else if (it->second<0)
                bot.push_back(std::make_unique<Exponent<Variable,Real>>(Variable(it->first), Real(-1*(it->second))));
        }
        if (bot.size()!=0 && top.size()!=0){
            return std::make_unique<Divide<Expression>>(Multiply<Expression>(Real(coefficient1.GetValue()/coefficient2.GetValue()), *(Recurse(top, 0, top.size()-1))), *(Recurse(bot, 0, bot.size()-1)));
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


auto Divide<Expression>::Recurse(std::vector<std::unique_ptr<Expression>> &varList, int front, int end) const -> std::unique_ptr<Expression>{
    if (front==end)
        return varList[front]->Copy();
    return std::make_unique<Multiply<Expression>>(*(Recurse(varList, front, (end+front)/2)), *(Recurse(varList,(end+front+1)/2, end)));
}




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