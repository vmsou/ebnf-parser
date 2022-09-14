#include <iostream>

#include "parser.hpp"

int main() {
    Parser parser;
    Rule r = Rule("Constante") << Rule::Text("T") | Rule::Text("F");
    std::cout << r.valid(parser, "T") << '\n';
}