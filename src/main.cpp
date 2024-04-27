#include <iostream>
#include <string>
#include <fstream>
#include "lexer.h"
#include "parser.h"

void versionDetails() {
    std::cout << "ACS Language Compiler version 1.4.2\n";
    std::cout << "Author: Ankit Dwivedi\n";
    std::cout << "Repository: https://github.com/dwivedi-ai/acs.git\n";
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        std::string arg = argv[1];
        if (arg == "--author" || arg == "-a") {
            versionDetails();
            return 0;
        }
    }
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
    std::vector<std::vector<Token>> tokens = obj.getTokens();
    std::string finalCode = "";
    for (const auto &token: tokens) {
        ASTNode* ast = parse(token);
        std::string generatedCode = generateCCode(ast);
        finalCode += generatedCode;
    }
    std::string file_name_cpp = "../build/" + file_name.substr(0, file_name_length - 4) + ".cpp";
    std::string preprocessors = "#include <iostream>\n#include <string>\n";
    std::string mainDefinition = "int main(int argc, char* argv[]) {\n";
    std::string mainClosure = "return 0;\n}";
    std::ofstream output_file(file_name_cpp);
    output_file << preprocessors;
    output_file << mainDefinition;
    output_file << finalCode;
    output_file << mainClosure;
    output_file.close();
    std::string gccCommand = "g++ " + file_name_cpp + " -o ../build/output";
    system(gccCommand.c_str());
    system("../build/output");
    system(("rm -rf " + file_name_cpp).c_str());
    return 0;
}
