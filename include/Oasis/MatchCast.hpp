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

#include <boost/callable_traits/args.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/vector.hpp>

#include <gsl/gsl-lite.hpp>

#include <concepts>
#include <functional>
#include <tuple>

namespace Oasis {
template <typename Lambda>
using lambda_argument_type = std::remove_cvref_t<std::tuple_element_t<0, boost::callable_traits::args_t<Lambda>>>;

template <typename CheckF, typename TransformerF, typename ArgumentT>
concept TransformerAcceptsCheckArg = requires(TransformerF f, const lambda_argument_type<CheckF>& t) {
    { f(t, nullptr) } -> std::same_as<std::expected<gsl::not_null<std::unique_ptr<ArgumentT>>, std::string_view>>;
} && std::predicate<CheckF, const lambda_argument_type<CheckF>&>;

template <typename ArgumentT, typename Cases>
class MatchCastImpl {
public:
    template <typename Check, typename Transformer>
        requires TransformerAcceptsCheckArg<Check, Transformer, ArgumentT>
    consteval auto Case(
        Check,
        Transformer) const -> MatchCastImpl<ArgumentT, typename boost::mpl::push_back<Cases, std::pair<Check, Transformer>>::type>
    {
        return {};
    }

    template <typename VisitorPtrT> requires IVisitor<std::remove_pointer_t<VisitorPtrT>> || std::same_as<VisitorPtrT, std::nullptr_t>
    auto Execute(const ArgumentT& arg, VisitorPtrT visitor) const -> std::expected<std::unique_ptr<ArgumentT>, std::string_view>
    {
        std::expected<std::unique_ptr<ArgumentT>, std::string_view> result = nullptr;
        boost::mpl::for_each<Cases>(
            [&]<typename Check, typename Transformer>(std::pair<Check, Transformer> checkAndTransformer) {
                using CaseType = lambda_argument_type<Check>;
                if (!result.has_value() || (result.has_value() && *result))
                    return;

                auto [check, transformer] = checkAndTransformer;
                if (std::unique_ptr<CaseType> castResult = RecursiveCast<CaseType>(arg); castResult && check(*castResult))
                    result = transformer(*castResult, visitor).transform([](gsl::not_null<std::unique_ptr<ArgumentT>>&& transformResult) { return std::move(transformResult); });
            });
        return result;
    }
};

template <typename ArgumentT>
using MatchCast = MatchCastImpl<ArgumentT, boost::mpl::vector<>>;
}

#endif // OASIS_MATCHCAST_HPP
