#pragma once
#include <vector>
#include <string>
#include "lexer.h"

struct ASTNode {
    TokenType type;
    std::string value;
    std::vector<ASTNode*> children;
};

ASTNode* parse(const std::vector<Token>& tokens);
std::string generateCCode(ASTNode* node);
