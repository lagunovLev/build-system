#pragma once
#include <string>
#include <filesystem>

struct ProgramArguments
{
    bool success;
    std::filesystem::path current_directory;
};

class ArgumentsParser
{
    enum state {
        Idle,
        GetCurrentDirectory,
    } state = Idle;

    void printUsage();
public:
    ProgramArguments parse(int argc, char* argv[]);
};