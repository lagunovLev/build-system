#pragma once
#include "Lexer.h"
#include "AST.h"

class Parser {
    std::list<Token>* tokens = nullptr;
    std::list<Token>::iterator it;
    void move();
    Token current();
    bool match(TokenType type);

    std::unique_ptr<Expr> boolExpr();
    std::unique_ptr<Expr> join();
    std::unique_ptr<Expr> eq();
    std::unique_ptr<Expr> rel();
    std::unique_ptr<Expr> expr();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();
public:
    Parser() { }

    std::unique_ptr<Expr> getAST(std::list<Token>& tokens);
};
