cmake_minimum_required(VERSION 3.26)
project(BMake)

set(CMAKE_CXX_STANDARD 20)

add_executable(BMake src/main.cpp
        src/args_parser.cpp
        src/args_parser.h
        src/constants.cpp
        src/constants.h
        src/token.h
        src/Lexer.cpp
        src/Lexer.h
        src/Parser.cpp
        src/Parser.h
        src/AST.h
        src/Interpreter.cpp
        src/Interpreter.h
        src/values.h)
