#include <initializer_list>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class Parser;

struct CharRange { 
    char from, to; 
    bool operator()(char c) const;
    friend std::istream& operator>>(std::istream& is, CharRange& r);
};

class Rule {
    // Alias
    public:
        enum class Mode { NONE=0, START, AND, OR };
        enum class Kind { NONE=0, RULE, TEXT, REF, GROUP, RANGE };
        enum class Flag { NONE=0, OPTIONAL, REPETITION };
        using token_t = char;
        using buffer_t = std::list<token_t>;

    // Attributes
    private:
        Mode _mode = Mode::NONE;
        Kind _kind = Kind::NONE;
        Flag _flag = Flag::NONE;
        std::string _command;
        std::vector<Rule> _elements;
        std::vector<CharRange> _ranges;

    // Constructors
    public:
        Rule() = default;
        Rule(Kind kind, const std::string& command);
        Rule(const std::string& name);

    // Static Methods
    public:
        static Rule Text(const std::string& command);
        static Rule Ref(const std::string& command);
        static Rule Range(const std::string& command);

    // Methods
    public:
        std::string name() const;

        Mode mode() const;
        void mode(Mode mode);

        Kind kind() const;
        void kind(Kind kind);

        Flag flag() const;
        void flag(Flag flag);

        bool handle_text(buffer_t& tokens, buffer_t& buffer) const;
        bool handle_ref(Parser& parser, buffer_t& tokens, buffer_t& buffer) const;
        bool handle_rule(Parser& parser, buffer_t& tokens, buffer_t& buffer) const;
        bool handle_range(Parser& parser, buffer_t& tokens, buffer_t& buffer) const;

        bool handle(Parser& parser, buffer_t& tokens, buffer_t& buffer) const;

        bool valid(Parser& parser, buffer_t& tokens, buffer_t& buffer) const;
        bool valid(Parser& parser, const std::string& text) const;

        Rule& repetition();
        Rule& optional();

    // Operators
    public:
        Rule& operator()(Flag flag);

        Rule& operator<<(Rule& other);
        Rule& operator&(Rule& other);
        Rule& operator|(Rule& other);

        inline Rule& operator<<(const std::string& other) { return this->operator<<(Rule::Text(other)); }
        inline Rule& operator&(const std::string& other) { return this->operator&(Rule::Text(other)); }
        inline Rule& operator|(const std::string& other) { return this->operator|(Rule::Text(other)); }

        inline Rule& operator<<(Rule&& other) { return this->operator<<(other); }
        inline Rule& operator&(Rule&& other) { return this->operator&(other); }
        inline Rule& operator|(Rule&& other) { return this->operator|(other); }

    // Functions
    public:
        friend std::ostream& operator<<(std::ostream& os, const Rule& rule);
};

class Parser {
    // Aliases
    public:
        using token_t = Rule::token_t;
        using buffer_t = Rule::buffer_t;
        using Grammar = std::vector<Rule>;
        using RulesMap = std::unordered_map<std::string, Rule>;

    // Attributes
    private:
        RulesMap rules;

    // Constructors
    public:
        Parser() = default;
        Parser(Grammar* grammar);

    // Methods
    public:
        void add_rule(const Rule& rule);

    // Static Methods
    public:
        static bool get_token(buffer_t& tokens, token_t& t);
        static void push_token(token_t& token, buffer_t& buffer);
        static void revert_tokens(buffer_t& tokens, buffer_t& buffer);

    // Methods
    public:
        bool has_rule(const std::string& name) const;
        const Rule& get_rule(const std::string& name) const;
        bool valid(const std::string& text, const std::string& rule_name);
};

// Functions
std::string escape_string(const std::string& str);

std::ostream& operator<<(std::ostream& os, Parser::buffer_t& buffer);