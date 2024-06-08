#pragma once
#include <string>
#include "token.h"
#include <map>

class Lexer {
    std::string code;
    int code_length;
    int cur_i = 0;
    int next_i = 1;
    std::map<std::string, Token> word_to_token;
    std::map<std::string, Token> op_to_token;

    char move();
    char current();
    char next();

    Token error_symbol_not_allowed(char ch);
    Token error_expected_end_of_string();
    Token error_wrong_escape_character();
    bool skip_spaces();
    Token number();
    Token word();
    Token op();
    Token str();
public:
    Lexer(std::string code);
    Token getNextToken();
};

