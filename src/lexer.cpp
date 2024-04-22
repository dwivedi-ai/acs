#include "lexer.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cctype>

Lexer::Lexer(std::string file_name) {
    this->file_name = file_name;
    this->input_file = std::ifstream(file_name);
    if (!input_file.is_open()) {
        std::cout << "\033[1m";
        std::cout << "acs: ";
        std::cout << "\033[1;31m";
        std::cout << "error: ";
        std::cout << "\033[0m";
        std::cout << "failed to open file '" << file_name << "'" << std::endl;
        exit(1);
    }
}

void Lexer::tokenize() {
    std::string line;
    while (std::getline(input_file, line)) {
        size_t pos = 0;
        while (pos < line.size()) {
            if ((pos = line.find("println", pos)) != std::string::npos) {
                pos += 7; // Move past "print"
                while (pos < line.size() && std::isspace(line[pos])) pos++; // Skip spaces
                               if (pos < line.size() && line[pos] == '(') {
                    pos++; // Move past "("
                    while (pos < line.size() && std::isspace(line[pos])) pos++; // Skip spaces
                                       if (pos < line.size() && line[pos] == '\"') {
                        size_t start = ++pos; // Move past the opening quote
                                               while (pos < line.size() && line[pos] != '\"') pos++; // Find the closing quote
                        if (pos < line.size()) {
                            std::string strLiteral = line.substr(start, pos - start);
                            tokens.push_back({TokenType::PRINTLN, strLiteral});
                            pos++; // Move past the closing quote
                            while (pos < line.size() && std::isspace(line[pos])) pos++; // Skip spaces
                            if (pos < line.size() && line[pos] == ')') {
                                pos++; // Move past ")"
                                while (pos < line.size() && std::isspace(line[pos])) pos++; // Skip spaces
                                if (pos < line.size() && line[pos] == ';') {
                                    tokens.push_back({TokenType::SEMI, ";"});
                                    pos++; // Move past ";"
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    input_file.close();
}

std::vector<Token> Lexer::getTokens() {
    return tokens;
}
