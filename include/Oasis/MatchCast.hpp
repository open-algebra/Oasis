//
// Created by Matthew McCall on 10/22/24.
//

#ifndef OASIS_MATCHCAST_HPP
#define OASIS_MATCHCAST_HPP

#include <type_traits>
#include <functional>

namespace Oasis {

template <typename T>
struct lambda_traits;

template <typename Ret, typename ClassType, typename Arg>
struct lambda_traits<Ret(ClassType::*)(Arg) const> {
    using argument_type = std::remove_cvref_t<Arg>;
};

template <typename Lambda>
using lambda_argument_type = typename lambda_traits<decltype(&Lambda::operator())>::argument_type;

template <typename ArgumentT>
class MatchCast {
public:
    using CaseFuncT = std::function<std::unique_ptr<ArgumentT>(const ArgumentT&)>;

    template<typename Lambda>
    MatchCast& Case(Lambda caseTrueCallback)
    {
        using ArgType = lambda_argument_type<Lambda>;
        cases_.emplace_back([caseTrueCallback](const ArgumentT& arg_) -> std::unique_ptr<ArgumentT> {
            if (std::unique_ptr<ArgType> castResult = RecursiveCast<ArgType>(arg_)) return caseTrueCallback(*castResult);
            return {};
        });

        return *this;
    }

    std::unique_ptr<ArgumentT> Execute(const ArgumentT& arg, std::unique_ptr<ArgumentT>&& fallback) const
    {
        for (const auto& c : cases_) if (auto result = c(arg)) return result;
        return fallback;
    }

private:
    std::vector<CaseFuncT> cases_;
};

}

#endif //OASIS_MATCHCAST_HPP
