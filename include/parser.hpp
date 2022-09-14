#include <list>
#include <string>
#include <sstream>
#include <vector>

class Parser {
    public:
        using token_t = char;
        using buffer_t = std::list<token_t>;

    public:
        static bool get_token(buffer_t& tokens, token_t& t) {
            if (tokens.empty()) return false;
            t = tokens.front(); tokens.pop_front();
            return true;
        }

        static void push_token(token_t& token, buffer_t& buffer) { buffer.push_front(token); }

        static void revert_tokens(buffer_t& tokens, buffer_t& buffer) {
            for (const token_t& t : buffer) tokens.push_front(t);
            buffer.clear();
        }
};

std::ostream& operator<<(std::ostream& os, Parser::buffer_t& buffer) {
    for (const Parser::token_t& t : buffer) os << t;
    return os;
}

class Rule {
    // Alias
    public:
        enum class Mode { NONE=0, START, AND, OR };
        enum class Kind { NONE=0, RULE, TEXT, REF };
        using buffer_t = Parser::buffer_t;
        using token_t = Parser::token_t;

    // Attributes
    public:
        Mode _mode = Mode::NONE;
        Kind _kind = Kind::NONE;
        std::string _command;
        std::vector<Rule> _elements;

    // Constructors
    public:
        Rule(Kind kind, const std::string& command): _kind{ kind }, _command{ command } {}
        Rule(const std::string& name): Rule(Kind::RULE, name) {}

    // Static Methods
    public:
        static Rule Text(const std::string& command) { return Rule(Kind::TEXT, command); }
        static Rule Ref(const std::string& command) { return Rule(Kind::REF, command); }

    // Methods
    public:
        Mode mode() const { return this->_mode; }
        void mode(Mode mode) { this->_mode = mode; }

        Kind kidn() const { return this->_kind; }
        void kind(Kind kind) { this->_kind = kind; }

        bool handle_text(buffer_t& tokens, buffer_t& buffer) const {
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

        bool handle_ref(buffer_t& tokens, buffer_t& buffer) const { return false; }
        bool handle_rule(buffer_t& tokens, buffer_t& buffer) const {
            bool valid = false;
            for (const Rule& r : this->_elements) {
                if (r.mode() == Mode::START) {
                    valid = r.valid(tokens, buffer);
                }    
                else if (r.mode() == Mode::OR) {
                    buffer_t temp_buffer;
                    if (valid || r.valid(tokens, temp_buffer)) return true;
                    else {
                        valid = false;
                        Parser::revert_tokens(tokens, temp_buffer);
                    }
                } 
                else if (r.mode() == Mode::AND) {
                    if (!r.valid(tokens, buffer)) { 
                        Parser::revert_tokens(tokens, buffer);
                        return false;
                    }
                }
            }
            return valid;
        }

        bool valid(buffer_t& tokens, buffer_t& buffer) const {
            bool valid = false;
            switch (this->_kind) {
                case Kind::NONE: return false; break;
                case Kind::TEXT: valid = this->handle_text(tokens, buffer); break;
                case Kind::REF: valid = this->handle_ref(tokens, buffer); break;
                case Kind::RULE: valid = this->handle_rule(tokens, buffer); break;
            }
            return valid && tokens.empty();
        }

        bool valid(const std::string& text) const {
            buffer_t tokens(text.begin(), text.end());
            buffer_t buffer;
            return this->valid(tokens, buffer);
        }

    // Operators
    public:
        Rule& operator<<(Rule&& other) {
            this->kind(Kind::RULE);
            other.mode(Mode::START);
            this->_elements.push_back(other);
            return *this;
        }

        Rule& operator&(Rule&& other) {
            other.mode(Mode::AND);
            this->_elements.push_back(other);
            return *this;
        }

        Rule& operator|(Rule&& other) {
            other.mode(Mode::OR);
            this->_elements.push_back(other);
            return *this;
        }
};