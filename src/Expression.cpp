#include "taskflow/taskflow.hpp"

#include "Oasis/Expression.hpp"

#include <Oasis/Add.hpp>
#include <Oasis/Divide.hpp>
#include <Oasis/Multiply.hpp>
#include <Oasis/Subtract.hpp>
#include <Oasis/Variable.hpp>
#include <Oasis/Exponent.hpp>

std::vector<long> getAllFactors(long n){
    std::vector<long> answer;
    for(long i = 1; i*i <= n; i++){
        if(n%i==0){
            answer.push_back(i);
        }
    }
    if(abs(n)!=1){
        answer.push_back(abs(n));
    }
    return answer;
}

long gcf(long a, long b){
    if(b>a){
        std::swap(a,b);
    }
    while(1){
        a%=b;
        if(a==0){
            return b;
        }
        b%=a;
        if(b==0){
            return a;
        }
    }
}


namespace Oasis {

//currently only supports polynomials of one variable.
/**
 * The FindZeros function finds all rational zeros of a polynomial. Currently assumes an expression of the form a+bx+cx^2+dx^3+... where a, b, c, d are a integers.
 *
 * @tparam origonalExpresion The expression for which all the factors will be found.
 */
auto Expression::FindZeros() const -> std::vector<std::unique_ptr<Expression>>{
    std::vector<std::unique_ptr<Expression>> results;
    std::vector<std::unique_ptr<Expression>> termsE;
    if(auto addCase = Add<Expression>::Specialize(*this); addCase !=nullptr){
        addCase->Flatten(termsE);
    } else {
        termsE.push_back(Copy());
    }
    std::string varName = "";
    std::vector<long> termsC;
    for(const auto &i : termsE){
        double coefficent;
        std::string variableName;
        double exponent;
        if (auto variableCase = Variable::Specialize(*i); variableCase!=nullptr){
            coefficent = 1;
            variableName = variableCase->GetName();
            exponent = 1;
        } else if (auto expCase = Exponent<Variable,Real>::Specialize(*i); expCase!=nullptr){
            coefficent = 1;
            variableName = expCase->GetMostSigOp().GetName();
            exponent = expCase->GetLeastSigOp().GetValue();
        } else if (auto prodCase = Multiply<Real,Variable>::Specialize(*i); prodCase!=nullptr){
            coefficent = prodCase->GetMostSigOp().GetValue();
            variableName = prodCase->GetLeastSigOp().GetName();
            exponent = 1;
        } else if (auto prodExpCase = Multiply<Real,Exponent<Variable,Real>>::Specialize(*i); prodExpCase!=nullptr){
            coefficent = prodExpCase->GetMostSigOp().GetValue();
            variableName = prodExpCase->GetLeastSigOp().GetMostSigOp().GetName();
            exponent = prodExpCase->GetLeastSigOp().GetLeastSigOp().GetValue();
        } else if (auto realCase = Real::Specialize(*i); realCase!=nullptr){
            coefficent = realCase->GetValue();
            variableName = varName;
            exponent = 0;
        } else {
            std::cerr << "no match\n";
            return {};
        }
        if(varName==""){
            varName = variableName;
        }
        if(coefficent!=round(coefficent)||exponent!=round(exponent)||varName!=variableName||exponent<0){
            std::cerr << (coefficent!=round(coefficent)) << (exponent!=round(exponent)) << (varName!=variableName) << (exponent<0);
            std::cerr << "rule Break\n";
            return {};
        }
        if(termsC.size()<=exponent){
            termsC.resize(exponent+1,0);
        }
        std::cout << "(" << exponent << "," << coefficent << "," << lround(coefficent) <<',' << termsC[exponent] << ")\n";
        termsC[exponent]+=lround(coefficent);
    }
    while(termsC.back()==0){
        termsC.pop_back();
    }
    for(auto i : termsC){
        std::cout << i << ' ';
    }
    std::cout << '\n';
    std::reverse(termsC.begin(),termsC.end());
        std::vector<long> q = getAllFactors(termsC.front());
        std::vector<long> p = getAllFactors(termsC.back());
    for(auto i : p){
        std::cout << i << ' ';
    }
    std::cout << '\n';
    for(auto i : q){
        std::cout << i << ' ';
    }
        for(auto pv : p){
            for(auto qv : q){
                if(gcf(pv,qv)==1){    
                    for(long sign : {-1,1}){
                        long mpv = pv*sign;
                        std::vector<long> newTermsC;
                        long h = 0;
                        for(long i : termsC){
                            h*=mpv;
                            if(h%qv!=0){
                                break;
                            }
                            h/=qv;
                            h += i;
                            newTermsC.push_back(h);
                        }
                        if(newTermsC.size()==termsC.size()&&newTermsC.back()==0){
                            termsC = newTermsC;
                            results.push_back(std::make_unique<Divide<Real>>(Real(mpv),Real(qv)));
                            do{
                                termsC.pop_back();
                            } while(termsC.back()==0);  
                            if(termsC.size()==0){
                                return results;
                            }
                        }
                    }
                }
            }
        }
    return results;
}

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

} // namespace Oasis
std::unique_ptr<Oasis::Expression> operator+(const std::unique_ptr<Oasis::Expression>& lhs, const std::unique_ptr<Oasis::Expression>& rhs)
{
    return Oasis::Add { *lhs, *rhs }.Simplify();
}
std::unique_ptr<Oasis::Expression> operator-(const std::unique_ptr<Oasis::Expression>& lhs, const std::unique_ptr<Oasis::Expression>& rhs)
{
    return Oasis::Subtract { *lhs, *rhs }.Simplify();
}
std::unique_ptr<Oasis::Expression> operator*(const std::unique_ptr<Oasis::Expression>& lhs, const std::unique_ptr<Oasis::Expression>& rhs)
{
    return Oasis::Multiply { *lhs, *rhs }.Simplify();
}
std::unique_ptr<Oasis::Expression> operator/(const std::unique_ptr<Oasis::Expression>& lhs, const std::unique_ptr<Oasis::Expression>& rhs)
{
    return Oasis::Divide { *lhs, *rhs }.Simplify();
}
