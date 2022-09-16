
#include "parser.hpp"

Rule operator"" _rule(const char* text, size_t) { return Rule(text); }
Rule operator"" _text(const char* text, size_t) { return Rule::Text(text); }
Rule operator"" _ref(const char* text, size_t) { return Rule::Ref(text); }
Rule operator"" _range(const char* text, size_t) { return Rule::Range(text); }

int main() {
    /* Parser::Grammar rules = {
        Rule("Formula") << Rule::Ref("Constante") | (Rule::Text("(") & Rule::Ref("Formula") & Rule::Text(")")),
        Rule("Constante") << Rule::Text("T") | Rule::Text("F"),
        Rule("AbreParen") << Rule::Text("("),
        Rule("FechaParen") << Rule::Text(")"),
    }; */

    /* Parser::Grammar rules = {
        Rule("whitespace") << " " | "\\n" | "\\t" | "\\r",
        Rule("letter") << Rule::Range("a-zA-Z"),
        Rule("digit") << Rule::Range("0-9"),
        Rule("symbol") << "[" | "]" | "{" | "}" | "(" | ")" | "<" | ">"  | "=" | "|" | "." | "," | ";" | "'" | (Rule::Text("\\") & "\""),
        Rule("character") << Rule::Ref("letter") | Rule::Ref("digit") | Rule::Ref("symbol") | "_",
        Rule("identifier") << Rule::Ref("letter") & (Rule::Ref("letter") | Rule::Ref("digit") | "_").repetition(),
        Rule("terminal") << (Rule::Text("\"") & (Rule::Ref("whitespace") | Rule::Ref("character")).repetition() & Rule::Text("\"")),
        Rule("lhs") << Rule::Ref("identifier"),
        Rule("rhs") << Rule::Ref("identifier") 
                     | Rule::Ref("terminal") 
                     | (Rule::Text("[") & Rule::Ref("rhs") & Rule::Text("]"))
                     | (Rule::Text("{") & Rule::Ref("rhs") & Rule::Text("}"))
                     | (Rule::Text("(") & Rule::Ref("rhs") & Rule::Text(")"))
                     | (Rule::Ref("rhs") & Rule::Text("|") & Rule::Ref("rhs"))
                     | (Rule::Ref("rhs") & Rule::Text(",") & Rule::Ref("rhs")),
        Rule("rule") << Rule::Ref("lhs") & Rule::Ref("whitespace").repetition() & "=" & Rule::Ref("whitespace").repetition() & Rule::Ref("rhs") & Rule::Ref("whitespace").repetition() &  ";",
        Rule("grammar") << Rule::Ref("rule").repetition(),
    }; */

    Parser::Grammar rules = {
        Rule("FORMULA") << Rule::Ref("CONSTANTE") | Rule::Ref("PROPOSICAO") | Rule::Ref("FORMULAUNARIA") | Rule::Ref("FORMULABINARIA"),
        Rule("CONSTANTE") << "T" | "F",
        Rule("PROPOSICAO") << "A" | "B" | "C" | "D",
        Rule("FORMULAUNARIA") << Rule::Ref("ABREPAREN") & Rule::Ref("OPERATORUNARIO") & Rule::Ref("FORMULA") & Rule::Ref("FECHAPAREN"),
        Rule("FORMULABINARIA") << Rule::Ref("ABREPAREN") & Rule::Ref("OPERATORBINARIO") & Rule::Ref("FORMULA") & Rule::Ref("FORMULA") & Rule::Ref("FECHAPAREN"),
        Rule("ABREPAREN") << "(",
        Rule("FECHAPAREN") << ")",
        Rule("OPERATORUNARIO") << "\\neg",
        Rule("OPERATORBINARIO") << "\\vee" | "\\wedge" | "\\rightarrow" | "\\leftrightarrow",
    };


    std::cout << "Rules:\n";
    for (const Rule& rule : rules) std::cout << rule << '\n';
    std::cout << '\n';

    Parser parser{ &rules };
    
    std::string chosen_rule;
    std::cout << "Rule: ";
    std::getline(std::cin, chosen_rule);
    while (!chosen_rule.empty() && !parser.has_rule(chosen_rule)) {
        std::cout << "Rule not found. Try Again.\n";
        std::cout << "Rule: ";
        std::getline(std::cin, chosen_rule);
    }

    bool is_running = !chosen_rule.empty();
    std::string expr;
    while (is_running) {
        std::cout << "> ";
        std::getline(std::cin, expr);
        if (expr.empty()) {is_running = false; continue; }
        bool valid = parser.valid(expr, chosen_rule);
        std::cout << expr << " : " << (valid ? "valido" : "invalido") << '\n';
    }
}