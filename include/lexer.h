#pragma once
#include <string>
#include <fstream>
#include <vector>
enum class TokenType {
    NUMBER,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    PRINT,
    PRINTLN,
    IDENTIFIER,
    LET,
    EQUAL,
    SEMI,
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
public:
    Lexer(std::string file_name);
    void tokenize();
    std::vector<std::vector<Token>> getTokens();
};