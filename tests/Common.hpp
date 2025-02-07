//
// Created by Matthew McCall on 1/31/25.
//

#ifndef COMMON_HPP
#define COMMON_HPP

#ifdef OASIS_IO_ENABLED
#define OASIS_CAPTURE_WITH_SERIALIZER(serializer, __expr) \
__expr.Accept(serializer);                        \
auto __expr_str = serializer.getResult();          \
INFO(#__expr << " := " << __expr_str);
#else
#define OASIS_CAPTURE_WITH_SERIALIZER(serializer, __expr)
#endif

#endif //COMMON_HPP
