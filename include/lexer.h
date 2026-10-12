#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
enum class TokenType {
    NUMBER,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    PRINT,
    PRINTLN,
    SCAN,
    IDENTIFIER,
    LET,
    INT,
    FLOAT,
    DOUBLE,
    EQUAL,
    SEMI,
    IF,
    ELSE,
    LBRACE,
    RBRACE,
    LPAREN,
    RPAREN,
    EQUAL_EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    AND,
    OR,
    NOT,
    END,
};
struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
    std::string file_name;
    std::ifstream input_file;
    std::vector<std::vector<Token>> tokens;
    std::unordered_map<std::string, TokenType> exprTable;
    // names declared in each open block, removed from exprTable when the block closes
    std::vector<std::vector<std::string>> scopes;
    // whether each open block was opened by `if` / `else if` (so an `else` may follow it)
    std::vector<bool> blockIsIf;
    // true right after the closing brace of an if/else-if block, so the next line may be `else`
    bool canElse = false;
    bool lexControlFlow(const std::string& line, int line_number, std::vector<Token>& tokenLine);
    void lexCondition(const std::string& cond, int line_number, std::vector<Token>& tokenLine);
    void declare(const std::string& identifier);
public:
    Lexer(std::string file_name);
    void tokenize();
    std::vector<std::vector<Token>> getTokens();
};