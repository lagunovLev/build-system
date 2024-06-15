#include <iostream>
#include <string>
#include "args_parser.h"
#include "constants.h"
#include <fstream>
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"

int main(int argc, char* argv[]) {
    auto args_parser = ArgumentsParser();
    ProgramArguments args = args_parser.parse(argc, argv);
    if (!args.success)
        return 0;

    std::filesystem::path path_to_script = args.current_directory.append(script_default_name);
    std::ifstream inputFile(path_to_script);
    std::string code((std::istreambuf_iterator<char>(inputFile)), (std::istreambuf_iterator<char>()));
    inputFile.close();

    auto lexer = Lexer();
    auto token_list = lexer.tokenize(code);
    auto parser = Parser();
    auto ast = parser.getAST(token_list);
    auto interpreter = Interpreter();
    float val = interpreter.exec(ast.get()).ToFloat();
    std::cout << val;
}
