#pragma once
#include "AST.h"
#include <functional>
#include "values.h"

class Interpreter
{
public:
    std::function<Value(Interpreter* interpreter, Expr* expr)> expr_handlers[(int)ExprType::Last];
    Interpreter();

    Value exec(Expr* ast);
};