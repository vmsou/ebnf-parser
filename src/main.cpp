#include <iostream>

#include "parser.hpp"

int main() {
    /* Parser::Grammar rules = {
        Rule("Formula") << Rule::Ref("Constante") | (Rule::Text("(") & Rule::Ref("Formula") & Rule::Text(")")),
        Rule("Constante") << Rule::Text("T") | Rule::Text("F"),
        Rule("AbreParen") << Rule::Text("("),
        Rule("FechaParen") << Rule::Text(")"),
    }; */

    Parser::Grammar rules = {
        Rule("letter") << "A" | "B" | "C" | "D" | "E" | "F" | "G"
                        | "H" | "I" | "J" | "K" | "L" | "M" | "N"
                        | "O" | "P" | "Q" | "R" | "S" | "T" | "U"
                        | "V" | "W" | "X" | "Y" | "Z",
        Rule("digit") << "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9",
        Rule("symbol") << "[" | "]" | "{" | "}" | "(" | ")" | "<" | ">"  | "=" | "|" | "." | "," | ";" | "'" | (Rule::Text("\\") & "\""),
        Rule("character") << Rule::Ref("letter") | Rule::Ref("digit") | Rule::Ref("symbol") | "_",
        Rule("identifier") << Rule::Ref("letter") & (Rule::Ref("letter") | Rule::Ref("digit") | "_")(Rule::Sequence::REPETITION),
        Rule("terminal") << (Rule::Text("\"") & Rule::Ref("character") & Rule::Ref("character")(Rule::Sequence::REPETITION) & Rule::Text("\"")) ,
    };


    std::cout << "Rules:\n";
    for (const Rule& rule : rules) std::cout << rule << '\n';
    std::cout << '\n';

    Parser parser{ &rules };
    
    bool is_running = true;
    std::string chosen_rule;
    std::cout << "Rule: ";
    std::getline(std::cin, chosen_rule);
    while (!parser.has_rule(chosen_rule)) {
        std::cout << "Rule not found. Try Again.\n";
        std::cout << "Rule: ";
        std::getline(std::cin, chosen_rule);
    }

    std::string expr;
    while (is_running) {
        std::cout << "> ";
        std::getline(std::cin, expr);
        if (expr.empty()) {is_running = false; continue; }
        bool valid = parser.valid(expr, chosen_rule);
        std::cout << expr << ": " << (valid ? "valido" : "invalido") << '\n';
    }
}