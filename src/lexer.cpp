#include "lexer.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cctype>
#include <unordered_map>
#include <sstream>

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
        if (line.size() == 0) continue;
        size_t pos = 0;
        std::vector<Token> tokenLine;
        line_number += 1;
        int checked = 0;
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
        auto isNumber = [&](const std::string& str) {
            for (char c : str) {
                if (!std::isdigit(c)) {
                    return false;
                }
            }
            return true;
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
            checked = 1;
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
            checked = 1;
        } else {
            pos = 0;
        }

        if ((pos = line.find("let", pos)) != std::string::npos && pos == 0) {
            tokenLine.push_back({TokenType::LET, "let"});
            pos += 3;
            skipSpaces();
            std::string identifier = "";
            size_t start = pos;
            while (pos < line.size() && std::isalpha(line[pos])) pos++;
            if (start < pos) {
                identifier = line.substr(start, pos - start);
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
                    std::cerr << "expected number after ':' at line number " << line_number << std::endl;
                    exit(1);
                }

                skipSpaces();

                if (line[pos] == ';') {
                    exprTable[identifier] = TokenType::IDENTIFIER;
                    tokenLine.push_back({TokenType::SEMI, ";"});
                    pos++;
                    skipSpaces();
                }
            }
            checked = 1;
        } else {
            pos = 0;
        }
        // evaluation expression
        if (checked == 0) {
            bool hasSemi = false;
            if (line[line.size() - 1] != ';') {
                std::cerr << "\033[1macs: \033[1;31merror: \033[0m";
                std::cerr << "missing semicolon at " << line_number << std::endl;
                exit(1);
            } else {
                hasSemi = true;
                line = line.substr(0, line.size() - 1);
            }
            std::istringstream unidentified_line(line);
            std::string single_word;
            bool wasLastID = false;
            while (std::getline(unidentified_line, single_word, ' ')) {
                if (isNumber(single_word)) {
                    tokenLine.push_back({TokenType::NUMBER, single_word});
                    wasLastID = true;
                    continue;
                }
                if ((pos = single_word.find(':')) != std::string::npos && single_word.size() != 1) {
                    skipSpaces();
                    tokenLine.push_back({TokenType::IDENTIFIER, single_word.substr(0, pos)});
                    tokenLine.push_back({TokenType::EQUAL, "="});
                    wasLastID = true;
                }
                if (exprTable[single_word] == TokenType::IDENTIFIER && !wasLastID) {
                    skipSpaces();
                    tokenLine.push_back({TokenType::IDENTIFIER, single_word});
                    wasLastID = true;
                } else if (wasLastID) {
                    skipSpaces();
                    if (single_word == "+") {
                        tokenLine.push_back({TokenType::PLUS, single_word});
                    }
                    if (single_word == "-") {
                        tokenLine.push_back({TokenType::MINUS, single_word});
                    }
                    if (single_word == "/") {
                        tokenLine.push_back({TokenType::DIVIDE, single_word});
                    }
                    if (single_word == "*") {
                        tokenLine.push_back({TokenType::MULTIPLY, single_word});
                    }
                    if (single_word == ":") {
                        tokenLine.push_back({TokenType::EQUAL, single_word});
                    }
                    wasLastID = false;
                } else {
                    std::cout << single_word << std::endl;
                    std::cerr << "\033[1macs: \033[1;31merror: \033[0m";
                    std::cerr << "undefined reference " << line_number << std::endl;
                    exit(1);
                }
            }
            if (hasSemi && tokenLine[tokenLine.size() - 1].type != TokenType::SEMI) {
                tokenLine.push_back({TokenType::SEMI, ";"});
            }
        }
        tokens.push_back(tokenLine);
    }
    input_file.close();
}

std::vector<std::vector<Token>> Lexer::getTokens() {
    return tokens;
}
