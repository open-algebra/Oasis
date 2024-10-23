//
// Created by Matthew McCall on 10/22/24.
//

#ifndef OASIS_MATCHCAST_HPP
#define OASIS_MATCHCAST_HPP

namespace Oasis {

template <typename ArgumentT>
class MatchCast {
public:
    using CaseFuncT = std::function<std::unique_ptr<ArgumentT>(const ArgumentT&)>;

    template<typename T>
    MatchCast& Case(std::function<std::unique_ptr<ArgumentT>(const T&)> caseTrueCallback)
    {
        cases_.emplace_back([this, caseTrueCallback](const ArgumentT& arg_) -> std::unique_ptr<ArgumentT> {
            if (std::unique_ptr<T> castResult = RecursiveCast<T>(arg_)) return caseTrueCallback(*castResult);
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
