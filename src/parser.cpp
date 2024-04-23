#include <vector>
#include <iostream>
#include "lexer.h"
#include "parser.h"


ASTNode* parse(const std::vector<Token>& tokens) {
    ASTNode* root = new ASTNode{TokenType::END, ""};
    for (const auto& token : tokens) {
        if (token.type == TokenType::PRINTLN) {
            ASTNode* printlnNode = new ASTNode{token.type, token.value};
            root -> children.push_back(printlnNode);
        }
        if (token.type == TokenType::PRINT) {
            ASTNode* printNode = new ASTNode{token.type, token.value};
            root -> children.push_back(printNode);
        }
        if (token.type == TokenType::LET) {
            ASTNode* letNode = new ASTNode{token.type, token.value};
            root->children.push_back(letNode);
        }
        if (token.type == TokenType::IDENTIFIER) {
            ASTNode* identifierNode = new ASTNode{token.type, token.value};
            root -> children.push_back(identifierNode);
        }
        if (token.type == TokenType::EQUAL) {
            ASTNode* equalNode = new ASTNode{token.type, token.value};
            root -> children.push_back(equalNode);
        }
        if (token.type == TokenType::NUMBER) {
            ASTNode* numberNode = new ASTNode{token.type, token.value};
            root -> children.push_back(numberNode);
        }
        if (token.type == TokenType::SEMI) {
            ASTNode* semiNode = new ASTNode{token.type, token.value};
            root -> children.push_back(semiNode);
        }
        if (token.type == TokenType::PLUS) {
            ASTNode* plusNode = new ASTNode{token.type, token.value};
            root -> children.push_back(plusNode);
        }
        if (token.type == TokenType::MINUS) {
            ASTNode* minusNode = new ASTNode{token.type, token.value};
            root -> children.push_back(minusNode);
        }
        if (token.type == TokenType::MULTIPLY) {
            ASTNode* mulNode = new ASTNode{token.type, token.value};
            root -> children.push_back(mulNode);
        }
        if (token.type == TokenType::DIVIDE) {
            ASTNode* divNode = new ASTNode{token.type, token.value};
            root -> children.push_back(divNode);
        }
    }
    return root;
}

std::string generateCCode(ASTNode* node) {
    std::string generatedCode = "";
    if (node -> type == TokenType::PRINTLN) {
        generatedCode += "std::cout << " + node -> value + " << std::endl";
    }
    if (node -> type == TokenType::PRINT) {
        generatedCode += "std::cout << " + node -> value + "";
    }
    if (node->type == TokenType::LET) {
        generatedCode += "auto ";
    }
    if (node -> type == TokenType::IDENTIFIER) {
        generatedCode += node -> value;
    }
    if (node -> type == TokenType::EQUAL) {
        generatedCode += " = ";
    }
    if (node -> type == TokenType::NUMBER) {
        generatedCode += node -> value;
    }
    if (node -> type == TokenType::SEMI) {
        generatedCode += ";\n";
    }
    if (node -> type == TokenType::PLUS) {
        generatedCode += " + ";
    }
    if (node -> type == TokenType::MINUS) {
        generatedCode += " - ";
    }
    if (node -> type == TokenType::MULTIPLY) {
        generatedCode += " * ";
    }
    if (node -> type == TokenType::DIVIDE) {
        generatedCode += " / ";
    }
    for (auto* child : node -> children) {
        generatedCode += generateCCode(child);
    }
    return generatedCode;
}

