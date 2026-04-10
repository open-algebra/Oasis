//
// Created by Matthew McCall on 1/31/25.
//

#ifndef COMMON_HPP
#define COMMON_HPP


#ifdef OASIS_IO_ENABLED
#include "Oasis/InFixSerializer.hpp"
inline Oasis::InFixSerializer __serializer;
#define __OASIS_CONCAT(a, b) a##b
#define __OASIS_CAPTURE_WITH_SERIALIZER_INNER(__expr, __counter)             \
auto __OASIS_CONCAT(__expr_str_, __counter) = (__expr).Accept(__serializer).value();  \
INFO(#__expr << " := " << __OASIS_CONCAT(__expr_str_, __counter));
#define OASIS_CAPTURE_WITH_SERIALIZER(__expr) \
    __OASIS_CAPTURE_WITH_SERIALIZER_INNER(__expr, __COUNTER__)
#else
#define OASIS_CAPTURE_WITH_SERIALIZER(__expr)
#endif

#endif //COMMON_HPP
