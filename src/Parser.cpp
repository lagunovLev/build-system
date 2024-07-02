#include "Parser.h"
#include <iostream>

Token Parser::current() {
    return *it;
}

void Parser::move() {
    if (current().type == TokenType::EOI)
        return;
    if (current().type == TokenType::Error)
    {
        std::cout << current().string_val.value() << '\n';
        return;
    }
    it++;
}

bool Parser::match(TokenType type) {
    if (current().type == type) {
        it++;
        return true;
    }
    else if (current().type == TokenType::Error)
        std::cout << current().string_val.value() << '\n';
    return false;
}

bool Parser::match_skip(TokenType type) {
    skipNewLine();
    return match(type);
}

Token Parser::current_skip() {
    skipNewLine();
    return current();
}

void Parser::move_skip() {
    skipNewLine();
    move();
}

std::unique_ptr<Stmt> Parser::getAST(std::list<Token>& tokens) {
    this->tokens = &tokens;
    it = tokens.begin();

    //if (auto t_last = tokens.end(); t_last->type == TokenType::Error)
    //{
    //    std::cout << t_last->string_val.value() << '\n';
    //}
    //else
        return stmtBlock();
}

std::unique_ptr<BlockStmt> Parser::stmtBlock() {
    auto block = std::make_unique<BlockStmt>();
    while (true)
    {
        skipStmtEnd();
        if (current().type == TokenType::Var)
            block->add(declaration());
        else if (current().type == TokenType::If)
            block->add(ifStmt());
        else if (current().type == TokenType::While)
            block->add(whileStmt());
        else if (current().type == TokenType::EOI || current().type == TokenType::RBrace)
            break;
        else
            block->add(assignment());
    }
    return std::move(block);
}

std::unique_ptr<Stmt> Parser::stmt() {
    skipStmtEnd();
    if (current().type == TokenType::Var)
        return declaration();
    if (current().type == TokenType::If)
        return ifStmt();
    if (current().type == TokenType::While)
        return whileStmt();
    return assignment();
}

std::unique_ptr<Stmt> Parser::ifStmt() {
    match(TokenType::If);
    skipNewLine();
    auto cond = boolExpr();
    std::unique_ptr<BlockStmt> body;
    std::unique_ptr<BlockStmt> else_body;
    if (current_skip().type == TokenType::LBrace)
    {
        move();
        body = stmtBlock();
        match(TokenType::RBrace);
    }
    else {
        body = std::make_unique<BlockStmt>();
        body->add(stmt());
    }
    if (current_skip().type == TokenType::Else)
    {
        move();
        if (current_skip().type == TokenType::LBrace)
        {
            move();
            else_body = stmtBlock();
            match(TokenType::RBrace);
        }
        else {
            else_body = std::make_unique<BlockStmt>();
            else_body->add(stmt());
        }
    }
    else {
        else_body = std::make_unique<BlockStmt>();
        else_body->add(std::make_unique<NoneStmt>());
    }
    return std::make_unique<IfStmt>(std::move(cond), std::move(body), std::move(else_body));
}

std::unique_ptr<Stmt> Parser::whileStmt() {
    match(TokenType::While);
    skipNewLine();
    auto cond = boolExpr();
    std::unique_ptr<BlockStmt> body;
    if (current_skip().type == TokenType::LBrace)
    {
        move();
        body = stmtBlock();
        match(TokenType::RBrace);
    }
    else {
        body = std::make_unique<BlockStmt>();
        body->add(stmt());
    }
    return std::make_unique<WhileStmt>(std::move(cond), std::move(body));
}

std::unique_ptr<Stmt> Parser::doWhileStmt() {
    match(TokenType::Do);
    std::unique_ptr<BlockStmt> body;
    if (current_skip().type == TokenType::LBrace)
    {
        move();
        body = stmtBlock();
        match(TokenType::RBrace);
    }
    else {
        body = std::make_unique<BlockStmt>();
        body->add(stmt());
    }
    match(TokenType::While);
    auto cond = boolExpr();
    stmtEnd();
    return std::make_unique<DoWhileStmt>(std::move(cond), std::move(body));
}



std::unique_ptr<Stmt> Parser::declaration() {
    match(TokenType::Var);
    std::unique_ptr<IdentifierExpr> expr = identifierExpr();
    match(TokenType::Assign);
    std::unique_ptr<Expr> right_expr = boolExpr();
    stmtEnd();
    return std::make_unique<DeclarationStmt>(std::move(expr), std::move(right_expr));
}

std::unique_ptr<Stmt> Parser::assignment() {
    std::unique_ptr<Expr> left_expr = boolExpr();
    if (current().type == TokenType::Assign)
    {
        move();
        std::unique_ptr<Expr> right_expr = boolExpr();
        stmtEnd();
        return std::make_unique<AssignmentStmt>(std::move(left_expr), std::move(right_expr));
    }
    stmtEnd();
    return std::make_unique<ExpressionStmt>(std::move(left_expr));
}

void Parser::stmtEnd() {
    if (current().type == TokenType::Semicolon
        || current().type == TokenType::NewLine
        || current().type == TokenType::EOI)
        move();
    else
        throw std::exception("Expected end of statement");
}

void Parser::skipStmtEnd() {
    while (current().type == TokenType::Semicolon
        || current().type == TokenType::NewLine
        || current().type == TokenType::EOI) {
        if (current().type != TokenType::EOI)
            move();
        else
            return;
    }
}

void Parser::skipNewLine() {
    while (current().type == TokenType::NewLine
           || current().type == TokenType::EOI) {
        if (current().type != TokenType::EOI)
            move();
        else
            return;
    }
}

std::unique_ptr<IdentifierExpr> Parser::identifierExpr() {
    if (current().type != TokenType::Identifier)
        std::cout << "Wrong token type";
    auto str = current().string_val.value();
    move();
    return std::make_unique<IdentifierExpr>(str);
}

std::unique_ptr<Expr> Parser::boolExpr() {
    auto left = join();
    if (current().type == TokenType::Or)
    {
        move();
        auto right = boolExpr();
        return std::make_unique<BinaryOpExpr>(std::move(left), std::move(right), ExprType::Or);
    }
    return std::move(left);
}

std::unique_ptr<Expr> Parser::join() {
    auto left = eq();
    if (current().type == TokenType::And)
    {
        move();
        auto right = join();
        return std::make_unique<BinaryOpExpr>(std::move(left), std::move(right), ExprType::And);
    }
    return std::move(left);
}

std::unique_ptr<Expr> Parser::eq() {
    auto left = rel();
    if (current().type == TokenType::Equal)
    {
        move();
        auto right = eq();
        return std::make_unique<BinaryOpExpr>(std::move(left), std::move(right), ExprType::Eq);
    }
    if (current().type == TokenType::NotEqual)
    {
        move();
        auto right = eq();
        return std::make_unique<BinaryOpExpr>(std::move(left), std::move(right), ExprType::NotEq);
    }
    return std::move(left);
}

std::unique_ptr<Expr> Parser::rel() {
    auto left = expr();
    if (current().type == TokenType::Greater)
    {
        move();
        auto right = rel();
        return std::make_unique<BinaryOpExpr>(std::move(left), std::move(right), ExprType::Greater);
    }
    if (current().type == TokenType::Less)
    {
        move();
        auto right = rel();
        return std::make_unique<BinaryOpExpr>(std::move(left), std::move(right), ExprType::Less);
    }
    if (current().type == TokenType::GreaterEq)
    {
        move();
        auto right = rel();
        return std::make_unique<BinaryOpExpr>(std::move(left), std::move(right), ExprType::GreaterEq);
    }
    if (current().type == TokenType::LessEq)
    {
        move();
        auto right = rel();
        return std::make_unique<BinaryOpExpr>(std::move(left), std::move(right), ExprType::LessEq);
    }
    return std::move(left);
}

std::unique_ptr<Expr> Parser::expr() {
    auto left = term();
    if (current().type == TokenType::Plus)
    {
        move();
        auto right = expr();
        return std::make_unique<BinaryOpExpr>(std::move(left), std::move(right), ExprType::Add);
    }
    if (current().type == TokenType::Minus)
    {
        move();
        auto right = expr();
        return std::make_unique<BinaryOpExpr>(std::move(left), std::move(right), ExprType::Sub);
    }
    return std::move(left);
}

std::unique_ptr<Expr> Parser::term() {
    auto left = unary();
    if (current().type == TokenType::Asterisk)
    {
        move();
        auto right = term();
        return std::make_unique<BinaryOpExpr>(std::move(left), std::move(right), ExprType::Mul);
    }
    if (current().type == TokenType::Slash)
    {
        move();
        auto right = term();
        return std::make_unique<BinaryOpExpr>(std::move(left), std::move(right), ExprType::Div);
    }
    if (current().type == TokenType::DoubleSlash)
    {
        move();
        auto right = term();
        return std::make_unique<BinaryOpExpr>(std::move(left), std::move(right), ExprType::IntDiv);
    }
    if (current().type == TokenType::Percent)
    {
        move();
        auto right = term();
        return std::make_unique<BinaryOpExpr>(std::move(left), std::move(right), ExprType::Mod);
    }
    return std::move(left);
}

std::unique_ptr<Expr> Parser::unary() {
    if (current().type == TokenType::Not)
    {
        move();
        auto expr = unary();
        return std::make_unique<UnaryOpExpr>(std::move(expr), ExprType::Not);
    }
    if (current().type == TokenType::Minus)
    {
        move();
        auto expr = unary();
        return std::make_unique<UnaryOpExpr>(std::move(expr), ExprType::Neg);
    }
    return primary();
}

std::unique_ptr<Expr> Parser::primary() {
    auto cur = current();
    move();
    if (cur.type == TokenType::FloatLiteral)
        return std::make_unique<FloatLiteralExpr>(cur.float_val.value());
    if (cur.type == TokenType::StringLiteral)
        return std::make_unique<StringLiteralExpr>(cur.string_val.value());
    if (cur.type == TokenType::IntegerLiteral)
        return std::make_unique<IntLiteralExpr>(cur.int_val.value());
    if (cur.type == TokenType::BoolLiteral)
        return std::make_unique<BoolLiteralExpr>(cur.bool_val.value());

    if (cur.type == TokenType::BoolType)
    {
        match(TokenType::LParent);
        auto expr = boolExpr();
        match(TokenType::RParent);
        return std::make_unique<UnaryOpExpr>(std::move(expr), ExprType::ToBool);
    }
    if (cur.type == TokenType::StringType)
    {
        match(TokenType::LParent);
        auto expr = boolExpr();
        match(TokenType::RParent);
        return std::make_unique<UnaryOpExpr>(std::move(expr), ExprType::ToString);
    }
    if (cur.type == TokenType::IntType)
    {
        match(TokenType::LParent);
        auto expr = boolExpr();
        match(TokenType::RParent);
        return std::make_unique<UnaryOpExpr>(std::move(expr), ExprType::ToInt);
    }
    if (cur.type == TokenType::FloatType)
    {
        match(TokenType::LParent);
        auto expr = boolExpr();
        match(TokenType::RParent);
        return std::make_unique<UnaryOpExpr>(std::move(expr), ExprType::ToFloat);
    }

    if (cur.type == TokenType::LParent)
    {
        auto expr = boolExpr();
        match(TokenType::RParent);
        return std::move(expr);
    }

    if (cur.type == TokenType::Identifier)
    {
        auto expr = std::make_unique<IdentifierExpr>(cur.string_val.value());
        //while (true)
        //{
        //    if (current().type == TokenType::LParent)
        //    {
        //        move();
        //
        //    }
        //    break;
        //}
        return std::move(expr);
    }
    throw std::exception("Unknown token");
}