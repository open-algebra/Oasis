//
// Created by Matthew McCall on 4/21/24.
//

#ifndef FROMSTRING_HPP
#define FROMSTRING_HPP

namespace Oasis {

class Expression;

auto FromInFix(const std::string& str) -> std::unique_ptr<Expression>;

}

#endif // FROMSTRING_HPP
