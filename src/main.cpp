#include <iostream>
#include <string>
#include <fstream>
#include "lexer.h"
#include "parser.h"
int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cout << "\033[1m";
        std::cout << "acs: ";
        std::cout << "\033[1;31m";
        std::cout << "fatal error: ";
        std::cout << "\033[0m";
        std::cout << "no input files" << std::endl;
        std::cout << "compilation failed" << std::endl;
        return 1;
    }
    std::string file_name = argv[1];
    int file_name_length = file_name.length();
    if (!(file_name_length >= 4 && file_name.substr(file_name_length - 4) == ".acs")) {
        std::cout << "\033[1m";
        std::cout << "acs: ";
        std::cout << "\033[1;31m";
        std::cout << "error: ";
        std::cout << "\033[0m";
        std::cout << "input file(s) not supported" << std::endl;
        return 1;
    }
    Lexer obj(file_name);
    obj.tokenize();
    std::vector<Token> tokens = obj.getTokens();
    ASTNode* ast = parse(tokens);
    std::string generatedCode = generateCCode(ast);
    std::string file_name_c = file_name.substr(0, file_name_length - 4) + ".c";
    std::string preprocessors = "#include <stdio.h>\n";
    std::string mainDefinition = "int main(int argc, char* argv[]) {\n";
    std::string mainClosure = "return 0;\n}";
    std::ofstream output_file(file_name_c);
    output_file << preprocessors;
    output_file << mainDefinition;
    output_file << generatedCode;
    output_file << mainClosure;
    output_file.close();
    std::string gccCommand = "gcc " + file_name_c + " -o ../build/output";
    system(gccCommand.c_str());
    system("../build/output");
    system(("rm -rf " + file_name_c).c_str());
    return 0;
}
