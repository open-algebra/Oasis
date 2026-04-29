//
// Created by Gregory Lemonnier on 2/xx/26.
//

#include "Oasis/Cosine.hpp"
#include "Oasis/Expression.hpp"
#include "Oasis/UnaryExpression.hpp"

namespace Oasis {
// auto Cosine<Expression>::Simplify() const -> std::unique_ptr<Expression>
// {   
//     const auto simplifiedOperand = operand ? operand->Simplify() : nullptr;
//     if (!operand){
//         return nullptr;
//     }

//     // Cos(real) --> some number
//     if (const auto realCase = RecursiveCast<Cosine<Real>>(simplifiedOperand); realCase != nullptr) {
//         return std::make_unique<Real>(Real(std::cos(realCase->GetOperand().GetValue())));
//     }

//     // Cos(real*pi) --> some number
//     if (const auto piCase = RecursiveCast<Cosine<Multiply<Real,Pi>>>(simplifiedOperand); piCase != nullptr) {
//         return std::make_unique<Real>(Real(std::cos(piCase->GetOperand().GetMostSigOp().GetValue() * piCase->GetOperand().GetLeastSigOp().GetValue())));
//     }

//     // Cos(2npi + x) --> Cos(x)
//     if (const auto periodicCase = RecursiveCast<Cosine<Add<Multiply<Real,Pi>,Expression>>>(simplifiedOperand); periodicCase != nullptr) {
//         const Real& multreal = periodicCase->GetOperand().getMostSigOp().GetMostSigOp();
//         if ((multreal.GetValue() % 2) == 0) {
//             return std::make_unique<Cosine<Expression>>(periodicCase->GetOperand().GetLeastSigOp());
//         }
//     }


//     // Cos(Arccos(x)) --> x
//     // Arccos not implemented yet
//     // if (auto CosArccosineCase = RecursiveCast<Cosine<Arccosine<Expression>>>(simplifiedOperand); CosArccosineCase != nullptr) {
//     //     return std::make_unique<Expression>(CosArccosineCase->GetOperand().GetOperand());
//     // }

    
//     if (auto CosMultiplyOperand = RecursiveCast<Cosine<Multiply<Real,Expression>>>(simplifiedOperand); CosMultiplyOperand != nullptr) {
//         //Cos(multreal*multexp)
//         const Real& multreal = CosMultiplyOperand->GetOperand().GetMostSigOp();
//         const Oasis::IExpression auto& multexp = CosMultiplyOperand->GetOperand().GetLeastSigOp();
        
//         // Cos(-x) --> Cos(x)
//         if (multreal.GetValue() < 0){
//             return std::make_unique<Cosine<Expression>>(Multiply<Real,Expression>{Real (multreal.GetValue() * -1),multexp});
//         }
//         // Cos(2x) --> cos^2(x) - sin^2(x)
//         if ((multreal.GetValue() % 2) == 0) {
//             return std::make_unique<Add<Expression>>(Exponent<Expression,Real>{Cosine<Expression>{Multiply<Real,Expression>{Real(multreal.GetValue() / 2),multexp}},Real(2)}
//                                                     ,Multiply<Real,Expression>{Real(-1),Exponent<Expression,Real>{Sine<Expression>{Multiply<Real,Expression>{Real(multreal.GetValue / 2),multexp}},Real(2)}});
//         }
//         // Cos(3x) --> 4cos^3(x) - 3cos(x)
//         if ((multreal.GetValue() % 3) == 0) {
//             return std::make_unique<Add<Expression>>(Multiply<Real,Expression>{Real(4),Exponent<Expression,Real>{Cosine<Expression>{Multiply<Real,Expression>(Real(multreal.GetValue() / 3),multexp)},Real(3)}}
//                                                     ,Multiply<Real,Expression>{Real(-3),Cosine<Expression>{Multiply<Real,Expression>(Real(multreal.GetValue() / 3),multexp)}});
//         }
//     }
//     // Cos(A + B) = Cos(A)Cos(B) - Sin(A)Sin(B)
//     if (auto CosAddOperand = RecursiveCast<Cosine<Add<Expression,Expression>>>(simplifiedOperand); CosAddOperand != nullptr) {
//         const Oasis::IExpression auto& Aexp = CosAddOperand->GetOperand().GetMostSigOp();
//         const Oasis::IExpression auto& Bexp = CosAddOperand->GetOperand().GetLeastSigOp();
//         return std::make_unique<Add<Expression>>(Multiply<Expression,Expression>{Cosine<Expression>{Aexp},Cosine<Expression>{Bexp}}
//                                                 ,Multiply<Real,Expression>{Real(-1),Multiply<Expression,Expression>{Sine<Expression>{Aexp},Sine<Expression>{Bexp}}});
//     }

//     return nullptr;
// }

// auto Cosine<Expression>::Differentiate(const Expression&) const -> std::unique_ptr<Expression>
// {
//     return nullptr;
// }

// auto Cosine<Expression>::Integrate(const Expression&) const -> std::unique_ptr<Expression>
// {
//     return nullptr;
// }
} // Oasis