#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include <ginac/ginac.h>
#include <string>

using namespace GiNaC;

namespace detail {

class dynamic_symtab : public GiNaC::symtab {
public:
    GiNaC::ex& operator[](const std::string& name);
};

symtab get_function_table();

struct e_to_exp : public GiNaC::map_function {
    GiNaC::ex operator()(const GiNaC::ex& expr);
};

} // namespace detail

GiNaC::ex ParseExpression(const std::string& input);

#endif // EXPRESSION_PARSER_H
