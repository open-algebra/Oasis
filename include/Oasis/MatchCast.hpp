//
// Created by Matthew McCall on 10/22/24.
//


// Hear ye, valiant coder! Within this hallowed script of Oasis' MatchCast,
// lies an unfathomable confluence of templated magic, scarcely understood only by even the
// most exalted compilers and the divine overseers. This arcane construct,
// when tampered with, may envelop thee in utter confusion, leaving thee solitary in thy pursuits.
// Of course, this sagely advice surely does not emanate from a mere language model. Rather, 
// it is the timeless counsel of battle-hardened developers who have faced such arcane complexity.
// Should regret cloud thy mind and thou desires to undo thine alterations, perform the following
// sacred rite to restore the code to its pristine state:
// 
// git checkout -- <path_to_this_file>
//

#ifndef OASIS_MATCHCAST_HPP
#define OASIS_MATCHCAST_HPP

#include <boost/mpl/vector.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/push_back.hpp>

#include <functional>

namespace Oasis {

template <typename T>
struct lambda_traits;

template <typename Ret, typename ClassType, typename Arg>
struct lambda_traits<Ret (ClassType::*)(Arg) const> {
    using argument_type = std::remove_cvref_t<Arg>;
};

template <typename Lambda>
using lambda_argument_type = typename lambda_traits<decltype(&Lambda::operator())>::argument_type;

template <typename ArgumentT, typename Cases>
class MatchCastImpl {
public:
    template <typename Lambda>
    MatchCastImpl<ArgumentT, typename boost::mpl::push_back<Cases, Lambda>::type>
    Case(Lambda) const
    {
        return MatchCastImpl<ArgumentT, typename boost::mpl::push_back<Cases, Lambda>::type>();
    }

    std::unique_ptr<ArgumentT> Execute(const ArgumentT& arg, std::unique_ptr<ArgumentT>&& fallback) const
    {
        std::unique_ptr<ArgumentT> result = nullptr;
        boost::mpl::for_each<Cases>([&]<typename CaseTrueCallbackT>(CaseTrueCallbackT caseTrueCallback) {
            using CaseType = lambda_argument_type<CaseTrueCallbackT>;
            if (result) return;
            if (std::unique_ptr<CaseType> castResult = RecursiveCast<CaseType>(arg))
                result = caseTrueCallback(*castResult);
        });
        return result ? std::move(result) : std::move(fallback);
    }
};

template <typename ArgumentT>
using MatchCast = MatchCastImpl<ArgumentT, boost::mpl::vector<>>;

}

#endif // OASIS_MATCHCAST_HPP
