//
// Created by Matthew McCall on 4/9/24.
//

#ifndef SEXPRESSION_HPP
#define SEXPRESSION_HPP

#include <string>
#include <list>

/**
 * Text to Tree
 */
namespace Oasis::t3 {

struct Sexp {
    Sexp() = default;

    /**
     * Represents and s-expression tree
     * @param str The string to parse
     */
    explicit Sexp(const std::string& str);
    
    std::string label;
    std::list<Sexp> operands; // so that add ops don't invalidate refs

    bool BAD_SEXP = false;
};

}

#endif //SEXPRESSION_HPP
