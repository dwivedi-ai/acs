#include <vector>
#include <iostream>
#include "lexer.h"
#include "parser.h"


ASTNode* parse(const std::vector<Token>& tokens) {
    ASTNode* root = new ASTNode{TokenType::END, ""};
    for (const auto& token : tokens) {
        if (token.type == TokenType::PRINTLN) {
            ASTNode* printNode = new ASTNode{token.type, token.value};
            root -> children.push_back(printNode);
        }
    }
    return root;
}

std::string generateCCode(ASTNode* node) {
    std::string generatedCode = "";
    if (node -> type == TokenType::PRINTLN) {
        generatedCode += "printf(\"" + node -> value + "\\n\");\n";
    }
    for (auto* child : node -> children) {
        generatedCode += generateCCode(child);
    }
    return generatedCode;
}

