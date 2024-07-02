#pragma once
#include "Lexer.h"
#include "AST.h"

class Parser {
    std::list<Token>* tokens = nullptr;
    std::list<Token>::iterator it;
    void move();
    Token current();
    bool match(TokenType type);
    bool match_skip(TokenType type);
    Token current_skip();
    void move_skip();

    void stmtEnd();
    void skipStmtEnd();
    void skipNewLine();

    std::unique_ptr<Stmt> stmt();
    std::unique_ptr<Stmt> ifStmt();
    std::unique_ptr<Stmt> whileStmt();
    std::unique_ptr<Stmt> doWhileStmt();
    //std::unique_ptr<Stmt> ifStmt();
    std::unique_ptr<BlockStmt> stmtBlock();
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> assignment();
    std::unique_ptr<IdentifierExpr> identifierExpr();
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

    std::unique_ptr<Stmt> getAST(std::list<Token>& tokens);
};
