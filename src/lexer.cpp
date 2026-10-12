#include "lexer.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cctype>
#include <unordered_map>
#include <sstream>
#include <algorithm>

static void fail(int line_number, const std::string& message) {
    std::cerr << "\033[1macs: \033[1;31merror: \033[0m";
    std::cerr << message << " at line number " << line_number << std::endl;
    exit(1);
}

static std::string trimLine(const std::string& line) {
    size_t start = line.find_first_not_of(" \t\r");
    if (start == std::string::npos) return "";
    size_t end = line.find_last_not_of(" \t\r");
    return line.substr(start, end - start + 1);
}

// true if `line` begins with `word` followed by a non-identifier character
static bool startsWithWord(const std::string& line, const std::string& word) {
    if (line.compare(0, word.size(), word) != 0) return false;
    if (line.size() == word.size()) return true;
    char c = line[word.size()];
    return !(std::isalnum(c) || c == '_');
}

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
        line_number += 1;
        line = trimLine(line);
        if (line.size() == 0) continue;
        size_t pos = 0;
        std::vector<Token> tokenLine;
        if (lexControlFlow(line, line_number, tokenLine)) {
            tokens.push_back(tokenLine);
            continue;
        }
        canElse = false;
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
        auto trim = [&](const std::string& line) {
            size_t start = line.find_first_not_of(" \t");
            size_t end = line.find_last_not_of(" \t");
            if (start == std::string::npos) {
                return std::string("");
            }
            return line.substr(start, end - start + 1);
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
        if ((pos = line.find("scan", pos)) != std::string::npos) {
            pos += 4;
            skipSpaces();
            expectAndSkip('(');

            size_t start = pos;
            size_t end = line.find_last_of(')');
            if (end != std::string::npos) {
                std::string strLiteral = line.substr(start, end - start);
                tokenLine.push_back({TokenType::SCAN, strLiteral});
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

        if ((pos = line.find("int", pos)) != std::string::npos && pos == 0 && checked == 0) {
            tokenLine.push_back({TokenType::INT, "int"});
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
                std::cerr << "expected identifier after 'int' at line number " << line_number << std::endl;
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
                    declare(identifier);
                    tokenLine.push_back({TokenType::SEMI, ";"});
                    pos++;
                    skipSpaces();
                }
            } else if (pos < line.size() && line[pos] == ';') {
                declare(identifier);
                tokenLine.push_back({TokenType::EQUAL, "="});
                tokenLine.push_back({TokenType::NUMBER, "0.0"});
                tokenLine.push_back({TokenType::SEMI, ";"});
                skipSpaces();
            }
            checked = 1;
        } else {
            pos = 0;
        }
        if ((pos = line.find("float", pos)) != std::string::npos && pos == 0 && checked == 0) {
            tokenLine.push_back({TokenType::FLOAT, "float"});
            pos += 5;
            skipSpaces();
            std::string identifier = "";
            size_t start = pos;
            while (pos < line.size() && std::isalpha(line[pos])) pos++;
            if (start < pos) {
                identifier = line.substr(start, pos - start);
                tokenLine.push_back({TokenType::IDENTIFIER, identifier});
            } else {
                std::cerr << "\033[1macs: \033[1;31merror: \033[0m";
                std::cerr << "expected identifier after 'float' at line number " << line_number << std::endl;
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
                    declare(identifier);
                    tokenLine.push_back({TokenType::SEMI, ";"});
                    pos++;
                    skipSpaces();
                }
            } else if (pos < line.size() && line[pos] == ';') {
                declare(identifier);
                tokenLine.push_back({TokenType::EQUAL, "="});
                tokenLine.push_back({TokenType::NUMBER, "0.0"});
                tokenLine.push_back({TokenType::SEMI, ";"});
                skipSpaces();
            }
            checked = 1;
        } else {
            pos = 0;
        }
        if ((pos = line.find("double", pos)) != std::string::npos && pos == 0 && checked == 0) {
            tokenLine.push_back({TokenType::DOUBLE, "double"});
            pos += 6;
            skipSpaces();
            std::string identifier = "";
            size_t start = pos;
            while (pos < line.size() && std::isalpha(line[pos])) pos++;
            if (start < pos) {
                identifier = line.substr(start, pos - start);
                tokenLine.push_back({TokenType::IDENTIFIER, identifier});
            } else {
                std::cerr << "\033[1macs: \033[1;31merror: \033[0m";
                std::cerr << "expected identifier after 'double' at line number " << line_number << std::endl;
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
                    declare(identifier);
                    tokenLine.push_back({TokenType::SEMI, ";"});
                    pos++;
                    skipSpaces();
                }
            } else if (pos < line.size() && line[pos] == ';') {
                declare(identifier);
                tokenLine.push_back({TokenType::EQUAL, "="});
                tokenLine.push_back({TokenType::NUMBER, "0.0"});
                tokenLine.push_back({TokenType::SEMI, ";"});
                skipSpaces();
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
                    declare(identifier);
                    tokenLine.push_back({TokenType::SEMI, ";"});
                    pos++;
                    skipSpaces();
                }
            } else if (pos < line.size() && line[pos] == ';') {
                declare(identifier);
                tokenLine.push_back({TokenType::EQUAL, "="});
                tokenLine.push_back({TokenType::NUMBER, "0.0"});
                tokenLine.push_back({TokenType::SEMI, ";"});
                skipSpaces();
            }
            checked = 1;
        } else {
            pos = 0;
        }
        // evaluation expression
        if (checked == 0) {
            line = trim(line);
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
                    tokenLine.push_back({TokenType::IDENTIFIER, single_word.substr(0, pos)});
                    tokenLine.push_back({TokenType::EQUAL, "="});
                    wasLastID = false;
                    continue;
                }
                if (exprTable[single_word] == TokenType::IDENTIFIER && !wasLastID) {
                    tokenLine.push_back({TokenType::IDENTIFIER, single_word});
                    wasLastID = true;
                } else if (wasLastID) {
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
    if (!scopes.empty()) {
        fail(line_number, "expected '}' before end of file");
    }
}

void Lexer::declare(const std::string& identifier) {
    if (exprTable[identifier] == TokenType::IDENTIFIER) return;
    exprTable[identifier] = TokenType::IDENTIFIER;
    if (!scopes.empty()) scopes.back().push_back(identifier);
}

// Handles `if (cond) {`, `} else if (cond) {`, `} else {`, `else {` and `}`.
// Returns false when the line is not a control-flow line.
bool Lexer::lexControlFlow(const std::string& line, int line_number, std::vector<Token>& tokenLine) {
    std::string rest = line;
    bool closes = false;
    if (rest[0] == '}') {
        if (scopes.empty()) {
            fail(line_number, "unexpected '}'");
        }
        for (const auto& name : scopes.back()) exprTable.erase(name);
        scopes.pop_back();
        canElse = blockIsIf.back();
        blockIsIf.pop_back();
        tokenLine.push_back({TokenType::RBRACE, "}"});
        closes = true;
        rest = trimLine(rest.substr(1));
        if (rest.empty()) {
            return true;
        }
    }
    bool isElse = startsWithWord(rest, "else");
    if (isElse) {
        if (!canElse) {
            fail(line_number, "'else' without a previous 'if'");
        }
        tokenLine.push_back({TokenType::ELSE, "else"});
        rest = trimLine(rest.substr(4));
    } else if (closes) {
        fail(line_number, "unexpected '" + rest + "' after '}'");
    }
    bool isIf = startsWithWord(rest, "if");
    if (!isElse && !isIf) {
        return false;
    }
    if (isIf) {
        tokenLine.push_back({TokenType::IF, "if"});
        rest = trimLine(rest.substr(2));
        if (rest.empty() || rest[0] != '(') {
            fail(line_number, "expected '(' after 'if'");
        }
        size_t depth = 0, close = std::string::npos;
        for (size_t i = 0; i < rest.size(); i++) {
            if (rest[i] == '(') depth++;
            if (rest[i] == ')' && --depth == 0) {
                close = i;
                break;
            }
        }
        if (close == std::string::npos) {
            fail(line_number, "expected ')' after condition");
        }
        std::string cond = trimLine(rest.substr(1, close - 1));
        if (cond.empty()) {
            fail(line_number, "expected condition inside 'if ( )'");
        }
        tokenLine.push_back({TokenType::LPAREN, "("});
        lexCondition(cond, line_number, tokenLine);
        tokenLine.push_back({TokenType::RPAREN, ")"});
        rest = trimLine(rest.substr(close + 1));
    }
    if (rest != "{") {
        fail(line_number, std::string("expected '{' after '") + (isIf ? "if (...)" : "else") + "'");
    }
    tokenLine.push_back({TokenType::LBRACE, "{"});
    scopes.push_back({});
    blockIsIf.push_back(isIf);
    canElse = false;
    return true;
}

void Lexer::lexCondition(const std::string& cond, int line_number, std::vector<Token>& tokenLine) {
    int depth = 0;
    size_t pos = 0;
    while (pos < cond.size()) {
        char c = cond[pos];
        char next = pos + 1 < cond.size() ? cond[pos + 1] : '\0';
        if (std::isspace(c)) {
            pos++;
        } else if (std::isdigit(c) || c == '.') {
            size_t start = pos;
            while (pos < cond.size() && (std::isdigit(cond[pos]) || cond[pos] == '.')) pos++;
            tokenLine.push_back({TokenType::NUMBER, cond.substr(start, pos - start)});
        } else if (std::isalpha(c) || c == '_') {
            size_t start = pos;
            while (pos < cond.size() && (std::isalnum(cond[pos]) || cond[pos] == '_')) pos++;
            std::string name = cond.substr(start, pos - start);
            auto it = exprTable.find(name);
            if (it == exprTable.end() || it->second != TokenType::IDENTIFIER) {
                fail(line_number, "undefined reference to '" + name + "'");
            }
            tokenLine.push_back({TokenType::IDENTIFIER, name});
        } else if (c == '=' && next == '=') {
            tokenLine.push_back({TokenType::EQUAL_EQUAL, "=="});
            pos += 2;
        } else if (c == '!' && next == '=') {
            tokenLine.push_back({TokenType::NOT_EQUAL, "!="});
            pos += 2;
        } else if (c == '<' && next == '=') {
            tokenLine.push_back({TokenType::LESS_EQUAL, "<="});
            pos += 2;
        } else if (c == '>' && next == '=') {
            tokenLine.push_back({TokenType::GREATER_EQUAL, ">="});
            pos += 2;
        } else if (c == '&' && next == '&') {
            tokenLine.push_back({TokenType::AND, "&&"});
            pos += 2;
        } else if (c == '|' && next == '|') {
            tokenLine.push_back({TokenType::OR, "||"});
            pos += 2;
        } else if (c == '<') {
            tokenLine.push_back({TokenType::LESS, "<"});
            pos++;
        } else if (c == '>') {
            tokenLine.push_back({TokenType::GREATER, ">"});
            pos++;
        } else if (c == '!') {
            tokenLine.push_back({TokenType::NOT, "!"});
            pos++;
        } else if (c == '+') {
            tokenLine.push_back({TokenType::PLUS, "+"});
            pos++;
        } else if (c == '-') {
            tokenLine.push_back({TokenType::MINUS, "-"});
            pos++;
        } else if (c == '*') {
            tokenLine.push_back({TokenType::MULTIPLY, "*"});
            pos++;
        } else if (c == '/') {
            tokenLine.push_back({TokenType::DIVIDE, "/"});
            pos++;
        } else if (c == '(') {
            tokenLine.push_back({TokenType::LPAREN, "("});
            depth++;
            pos++;
        } else if (c == ')') {
            if (--depth < 0) {
                fail(line_number, "unbalanced ')' in condition");
            }
            tokenLine.push_back({TokenType::RPAREN, ")"});
            pos++;
        } else if (c == '=') {
            fail(line_number, "unexpected '=' in condition (use '==' to compare)");
        } else {
            fail(line_number, std::string("unexpected '") + c + "' in condition");
        }
    }
    if (depth != 0) {
        fail(line_number, "unbalanced '(' in condition");
    }
}

std::vector<std::vector<Token>> Lexer::getTokens() {
    return tokens;
}
