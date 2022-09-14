#include "parser.hpp"

/* Rule:: */
// Constructors
Rule::Rule(Kind kind, const std::string& command): _kind{ kind }, _command{ command } {}
Rule::Rule(const std::string& name): Rule(Kind::RULE, name) {}

// Static Methods
Rule Rule::Text(const std::string& command) { return Rule(Kind::TEXT, command); }
Rule Rule::Ref(const std::string& command) { return Rule(Kind::REF, command); }

// Methods
std::string Rule::name() const { return this->_command; }

Rule::Mode Rule::mode() const { return this->_mode; }
void Rule::mode(Mode mode) { this->_mode = mode; }

Rule::Kind Rule::kind() const { return this->_kind; }
void Rule::kind(Kind kind) { this->_kind = kind; }

bool Rule::handle_text(buffer_t& tokens, buffer_t& buffer) const {
    std::size_t len = this->_command.size();
    std::size_t i = 0;
    token_t c;
    while (i < len && Parser::get_token(tokens, c)) {
        Parser::push_token(c, buffer);
        if (c != this->_command[i]) break;
        ++i;
    }
    if (i != len) { Parser::revert_tokens(tokens, buffer); return false; }
    return true;
}

bool Rule::handle_ref(Parser& parser, buffer_t& tokens, buffer_t& buffer) const {
    const Rule& rule = parser.get_rule(this->_command);
    return rule.valid(parser, tokens, buffer);
}
bool Rule::handle_rule(Parser& parser, buffer_t& tokens, buffer_t& buffer) const {
    bool valid = false;
    for (const Rule& r : this->_elements) {
        if (r.mode() == Mode::START) {
            valid = r.valid(parser, tokens, buffer);
        }    
        else if (r.mode() == Mode::OR) {
            buffer_t temp_buffer;
            if (valid || r.valid(parser, tokens, temp_buffer)) return true;
            else {
                valid = false;
                Parser::revert_tokens(tokens, temp_buffer);
            }
        } 
        else if (r.mode() == Mode::AND) {
            if (!r.valid(parser, tokens, buffer)) { 
                Parser::revert_tokens(tokens, buffer);
                return false;
            }
        }
    }
    return valid;
}

bool Rule::valid(Parser& parser, buffer_t& tokens, buffer_t& buffer) const {
    bool valid = false;
    switch (this->_kind) {
        case Kind::NONE: return false; break;
        case Kind::TEXT: valid = this->handle_text(tokens, buffer); break;
        case Kind::REF: valid = this->handle_ref(parser, tokens, buffer); break;
        case Kind::RULE: valid = this->handle_rule(parser, tokens, buffer); break;
    }
    return valid && tokens.empty();
}

bool Rule::valid(Parser& parser, const std::string& text) const {
    buffer_t tokens(text.begin(), text.end());
    buffer_t buffer;
    return this->valid(parser, tokens, buffer);
}

// Operators
Rule& Rule::operator<<(Rule&& other) {
    this->kind(Kind::RULE);
    other.mode(Mode::START);
    this->_elements.push_back(other);
    return *this;
}

Rule& Rule::operator&(Rule&& other) {
    other.mode(Mode::AND);
    this->_elements.push_back(other);
    return *this;
}

Rule& Rule::operator|(Rule&& other) {
    other.mode(Mode::OR);
    this->_elements.push_back(other);
    return *this;
}

// Functions
std::ostream& operator<<(std::ostream& os, const Rule& rule) {
    using Mode = Rule::Mode;
    using Kind = Rule::Kind;

    switch (rule.mode()) {
        case Mode::AND: os << std::string(" , "); break;
        case Mode::OR: os << std::string(" | "); break;
        default: break;
    }

    switch (rule.kind()) {
        case Kind::TEXT: return os << "\"" + rule.name() + "\"";
        case Kind::REF: return os << rule.name();
        case Kind::RULE: {
            os << rule.name() << std::string(" = ");
            for (const Rule& r : rule._elements) os << r;
            return os;
        }
        default: break;
    }
    return os;
}

/* Parser:: */
// Constructors
Parser::Parser(Grammar* grammar) { for (const Rule& r : *grammar) this->add_rule(r); }

// Methods
void Parser::add_rule(const Rule& rule) { this->rules[rule.name()] = rule; }

// Static Methods
bool Parser::get_token(buffer_t& tokens, token_t& t) {
    if (tokens.empty()) return false;
    t = tokens.front(); tokens.pop_front();
    return true;
}

void Parser::push_token(token_t& token, buffer_t& buffer) { buffer.push_front(token); }

void Parser::revert_tokens(buffer_t& tokens, buffer_t& buffer) {
    for (const token_t& t : buffer) tokens.push_front(t);
    buffer.clear();
}

// Methods
const Rule& Parser::get_rule(const std::string& name) const { return this->rules.at(name); }

bool Parser::valid(const std::string& text, const std::string& rule_name) {
    const Rule& rule = this->get_rule(rule_name);
    return rule.valid(*this, text);
}


/* Functions */
std::ostream& operator<<(std::ostream& os, Parser::buffer_t& buffer) {
    for (const Parser::token_t& t : buffer) os << std::string(1, t);
    return os;
}