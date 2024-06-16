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

std::unique_ptr<Expr> Parser::getAST(std::list<Token>& tokens) {
    this->tokens = &tokens;
    it = tokens.begin();

    //if (auto t_last = tokens.end(); t_last->type == TokenType::Error)
    //{
    //    std::cout << t_last->string_val.value() << '\n';
    //}
    //else
        return boolExpr();
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
}