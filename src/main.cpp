#include <iostream>

#include "parser.hpp"

int main() {
    Parser::Grammar rules = {
        Rule("Formula") << Rule::Ref("Constante") | (Rule::Text("(") & Rule::Ref("Formula") & Rule::Text(")")),
        Rule("Constante") << Rule::Text("T") | Rule::Text("F"),
        Rule("AbreParen") << Rule::Text("("),
        Rule("FechaParen") << Rule::Text(")"),
    };

    std::cout << "Rules:" << '\n';
    for (const Rule& rule : rules) std::cout << rule << '\n';
    std::cout << '\n';

    Parser parser{ &rules };
    
    bool is_running = true;
    std::string expr;
    while (is_running) {
        std::cout << "> ";
        std::getline(std::cin, expr);
        if (expr.empty()) {is_running = false; continue; }
        std::cout << expr << ": " << (parser.valid(expr, "Formula") ? "valido" : "invalido") << '\n';
    }
}