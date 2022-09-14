#include <iostream>

#include "parser.hpp"

int main() {
    Rule r = Rule("Constante") << Rule::Text("T") | Rule::Text("F");
    std::cout << r.valid("T") << '\n';
}