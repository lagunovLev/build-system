#pragma once
#include "Lexer.h"

class Parser {
    Lexer* lexer;
public:
    Parser(Lexer& lexer) {
        this->lexer = &lexer;
    }


};
