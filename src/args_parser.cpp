#include "args_parser.h"
#include <iostream>

ProgramArguments ArgumentsParser::parse(int argc, char **argv) {
    ProgramArguments args = {
            .success = false,
            .current_directory = std::filesystem::current_path(),
    };

    int i = 1;
    for (; i < argc; i++)
    {
        std::string arg = argv[i];
        if (state == Idle) {
            if (arg == "-s") {
                state = GetCurrentDirectory;
            }
            else
                break;
        }
        else if (state == GetCurrentDirectory) {
            args.current_directory = arg;
            state = Idle;
        }
    }
    if (i == argc && state == Idle)
        args.success = true;
    else
        printUsage();
    return std::move(args);
}

void ArgumentsParser::printUsage() {
    std::cout << "Usage:\n";
    std::cout << "\t-s\tSet current directory\n";
}