#pragma once
#include "token.h"
#include <memory>
#include <vector>


// Basic Nodes
enum class ExprType {
    Identifier,
    BoolLiteral,
    StringLiteral,
    IntLiteral,
    FloatLiteral,
    ToBool,
    ToString,
    ToInt,
    ToFloat,
    Mul,
    Div,
    Add,
    Sub,
    Eq,
    NotEq,
    Not,
    Greater,
    Less,
    GreaterEq,
    LessEq,
    FnCall,
};

enum class StmtType {
    Declaration,
    Assignment,
    If,
    While,
    DoWhile,
    For,
    Block,
};

struct Node
{
    enum class NodeType { Expr, Stmt } node_type;
};

struct Expr : Node
{
    ExprType expr_type;
    bool left;

    Expr() {
        node_type = NodeType::Expr;
        left = false;
    }
};

struct Stmt : Node
{
    StmtType stmt_type;

    Stmt() {
        node_type = NodeType::Stmt;
    }
};


// Expressions
struct IdentifierExpr : Expr
{
    std::string id;

    IdentifierExpr(std::string value) : Expr() {
        expr_type = ExprType::Identifier;
        this->id = value;
        left = true;
    }
};

struct BoolLiteralExpr : Expr
{
    bool value;

    BoolLiteralExpr(bool value) : Expr() {
        expr_type = ExprType::BoolLiteral;
        this->value = value;
    }
};

struct StringLiteralExpr : Expr
{
    std::string value;

    StringLiteralExpr(std::string value) : Expr() {
        expr_type = ExprType::StringLiteral;
        this->value = value;
    }
};

struct IntLiteralExpr : Expr
{
    int value;

    IntLiteralExpr(int value) : Expr() {
        expr_type = ExprType::IntLiteral;
        this->value = value;
    }
};

struct FloatLiteralExpr : Expr
{
    float value;

    FloatLiteralExpr(float value) : Expr() {
        expr_type = ExprType::FloatLiteral;
        this->value = value;
    }
};

struct UnaryOpExpr : Expr
{
    std::unique_ptr<Expr> expr;

    UnaryOpExpr(std::unique_ptr<Expr> expr, ExprType type) : Expr() {
        this->expr_type = type;
        this->expr = std::move(expr);
    }
};

struct BinaryOpExpr : Expr
{
    std::unique_ptr<Expr> left_expr;
    std::unique_ptr<Expr> right_expr;

    BinaryOpExpr(std::unique_ptr<Expr> left_expr, std::unique_ptr<Expr> right_expr) : Expr() {
        expr_type = ExprType::Mul;
        this->left_expr = std::move(left_expr);
        this->right_expr = std::move(right_expr);
    }
};

struct FnCallExpr : Expr
{
    std::unique_ptr<Expr> expr;

    FnCallExpr(std::unique_ptr<Expr> expr) : Expr() {
        expr_type = ExprType::FnCall;
        this->expr = std::move(expr);
    }
};


// Statements
struct DeclarationStmt : Stmt
{
    std::unique_ptr<IdentifierExpr> id;

    DeclarationStmt(std::unique_ptr<IdentifierExpr> id) : Stmt() {
        stmt_type = StmtType::Declaration;
        this->id = std::move(id);
    }
};

struct AssignmentStmt : Stmt
{
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;

    AssignmentStmt(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right) : Stmt() {
        stmt_type = StmtType::Assignment;
        this->left = std::move(left);
        this->right = std::move(right);
    }
};

struct BlockStmt : Stmt
{
    std::vector<std::unique_ptr<Stmt>> stmts;

    BlockStmt() : Stmt() {
        stmt_type = StmtType::Block;
    }

    void add(std::unique_ptr<Stmt> stmt) {
        stmts.push_back(std::move(stmt));
    }
};

struct IfStmt : Stmt
{
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Stmt> action;
    std::unique_ptr<Stmt> else_action;

    IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> action, std::unique_ptr<Stmt> else_action) : Stmt() {
        stmt_type = StmtType::If;
        this->cond = std::move(cond);
        this->action = std::move(action);
        this->else_action = std::move(else_action);
    }
};

struct WhileStmt : Stmt
{
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Stmt> action;

    WhileStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> action) : Stmt() {
        stmt_type = StmtType::While;
        this->cond = std::move(cond);
        this->action = std::move(action);
    }
};

struct DoWhileStmt : Stmt
{
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Stmt> action;

    DoWhileStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> action) : Stmt() {
        stmt_type = StmtType::DoWhile;
        this->cond = std::move(cond);
        this->action = std::move(action);
    }
};

struct ForStmt : Stmt
{
    std::unique_ptr<Stmt> init;
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Stmt> post;
    std::unique_ptr<Stmt> action;

    ForStmt(std::unique_ptr<Stmt> init, std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> post, std::unique_ptr<Stmt> action) : Stmt() {
        stmt_type = StmtType::For;
        this->cond = std::move(cond);
        this->action = std::move(action);
        this->init = std::move(init);
        this->post = std::move(post);
    }
};