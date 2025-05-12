// expression_parser.cpp
#include "expression_parser.h"
#include <set>

using namespace GiNaC;

namespace detail {

GiNaC::ex& dynamic_symtab::operator[](const std::string& name) {
    auto it = this->find(name);
    if (it != this->end())
        return it->second;

    static const std::set<std::string> reserved_words = {
        "sin", "cos", "tan", "asin", "acos", "atan",
        "sinh", "cosh", "tanh", "asinh", "acosh", "atanh",
        "log", "exp", "sqrt", "pi", "e"
    };

    if (reserved_words.count(name))
        throw std::invalid_argument("Identifier '" + name + "' is a reserved keyword.");

    symbol new_sym(name);
    auto result = this->emplace(name, new_sym);
    return result.first->second;
}

symtab get_function_table() {
    symtab ft;
    ft["ln"] = GiNaC::log(GiNaC::symbol("x"));  // Maps "ln(x)" to log(x)
    return ft;
}

GiNaC::ex e_to_exp::operator()(const GiNaC::ex& expr) {
    GiNaC::ex processed = expr.map(*this);
    if (is_a<power>(processed)) {
        const power& p = ex_to<power>(processed);
        if (p.op(0).is_equal(exp(numeric(1))))
            return exp(p.op(1));
    }
    return processed;
}

} // namespace detail

GiNaC::ex ParseExpression(const std::string& input) {
    detail::dynamic_symtab var_table;
    symtab func_table = detail::get_function_table();

    for (const auto& [name, expr] : func_table) {
        var_table[name] = expr;
    }

    parser reader(var_table);

    try {
        GiNaC::ex expr = reader(input);
        detail::e_to_exp transformer;
        return transformer(expr);
    } catch (const std::exception& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
        return 0;
    }
}
