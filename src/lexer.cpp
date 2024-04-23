#include "lexer.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cctype>

Lexer::Lexer(std::string file_name) : file_name(file_name), input_file(file_name) {
    if (!input_file.is_open()) {
        std::cerr << "\033[1macs: \033[1;31merror: \033[0m";
        std::cerr << "failed to open file '" << file_name << "'" << std::endl;
        exit(1);
    }
}

void Lexer::tokenize() {
    std::string line;
    int line_number = 0;
    while (std::getline(input_file, line)) {
        size_t pos = 0;
        std::vector<Token> tokenLine;
        line_number += 1;

        auto skipSpaces = [&]() {
            while (pos < line.size() && std::isspace(line[pos])) pos++;
        };

        auto expectAndSkip = [&](char expected) {
            if (pos < line.size() && line[pos] == expected) {
                pos++;
                skipSpaces();
            } else {
                std::cerr << "\033[1macs: \033[1;31merror: \033[0m";
                std::cerr << "expected '" << expected << "' at line number " << line_number << std::endl;
                exit(1);
            }
        };

        if ((pos = line.find("println", pos)) != std::string::npos) {
            pos += 7;
            skipSpaces();
            expectAndSkip('(');

            size_t start = pos;
            size_t end = line.find_last_of(')');
            if (end != std::string::npos) {
                std::string strLiteral = line.substr(start, end - start);
                tokenLine.push_back({TokenType::PRINTLN, strLiteral});
                pos = end + 1;
                skipSpaces();
                if (line[pos] == ';') {
                    tokenLine.push_back({TokenType::SEMI, ";"});
                    pos++;
                    skipSpaces();
                }
            }
        } else {
            pos = 0;
        }

        if ((pos = line.find("print", pos)) != std::string::npos) {
            pos += 5;
            skipSpaces();
            expectAndSkip('(');

            size_t start = pos;
            size_t end = line.find_last_of(')');
            if (end != std::string::npos) {
                std::string strLiteral = line.substr(start, end - start);
                tokenLine.push_back({TokenType::PRINT, strLiteral});
                pos = end + 1;
                skipSpaces();
                if (line[pos] == ';') {
                    tokenLine.push_back({TokenType::SEMI, ";"});
                    pos++;
                    skipSpaces();
                }
            }
        } else {
            pos = 0;
        }

        if ((pos = line.find("let", pos)) != std::string::npos) {
            tokenLine.push_back({TokenType::LET, "let"});
            pos += 3;
            skipSpaces();

            size_t start = pos;
            while (pos < line.size() && std::isalpha(line[pos])) pos++;
            if (start < pos) {
                std::string identifier = line.substr(start, pos - start);
                tokenLine.push_back({TokenType::IDENTIFIER, identifier});
            } else {
                std::cerr << "\033[1macs: \033[1;31merror: \033[0m";
                std::cerr << "expected identifier after 'let' at line number " << line_number << std::endl;
                exit(1);
            }

            skipSpaces();

            if (pos < line.size() && line[pos] == ':') {
                tokenLine.push_back({TokenType::EQUAL, "="});
                pos++;
                skipSpaces();

                start = pos;
                while (pos < line.size() && (std::isdigit(line[pos]) || line[pos] == '.')) pos++;
                if (start < pos) {
                    std::string value = line.substr(start, pos - start);
                    tokenLine.push_back({TokenType::NUMBER, value});
                } else {
                    std::cerr << "\033[1macs: \033[1;31merror: \033[0m";
                    std::cerr << "expected number after '=' at line number " << line_number << std::endl;
                    exit(1);
                }

                skipSpaces();

                if (line[pos] == ';') {
                    tokenLine.push_back({TokenType::SEMI, ";"});
                    pos++;
                    skipSpaces();
                }
            }
        }
        tokens.push_back(tokenLine);
    }
    input_file.close();
}

std::vector<std::vector<Token>> Lexer::getTokens() {
    return tokens;
}
