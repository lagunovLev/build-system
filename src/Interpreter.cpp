#include "Interpreter.h"
#include <utility>
#include <iostream>

std::shared_ptr<Value> identifierHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<IdentifierExpr*>(expr);
    auto& name = e->id;
    ValueID id = interpreter->symbolTable->getVariable(name);
    return interpreter->memory.get(id);
}

std::shared_ptr<Value> boolLiteralHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<BoolLiteralExpr*>(expr);
    return std::make_shared<Value>(Value::Bool(e->value));
}

//Value stringLiteralHandler(Interpreter* interpreter, Expr* expr) {
//    auto e = dynamic_cast<StringLiteralExpr*>(expr);
//    return Value::String(e->value);
//}

std::shared_ptr<Value> intLiteralHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<IntLiteralExpr*>(expr);
    return std::make_shared<Value>(Value::Int(e->value));
}

std::shared_ptr<Value> floatLiteralHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<FloatLiteralExpr*>(expr);
    return std::make_shared<Value>(Value::Float(e->value));
}

std::shared_ptr<Value> negHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<UnaryOpExpr*>(expr);
    auto val = interpreter->eval(e->expr.get());
    if (val->type == ValueType::Int || val->type == ValueType::Bool)
        return std::make_shared<Value>(Value::Int(-val->ToInt()));
    if (val->type == ValueType::Float)
        return std::make_shared<Value>(Value::Float(-val->ToFloat()));
}

std::shared_ptr<Value> notHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<UnaryOpExpr*>(expr);
    auto val = interpreter->eval(e->expr.get());
    if (val->IsNumeric())
        return std::make_shared<Value>(Value::Bool(!val->ToBool()));
}

std::shared_ptr<Value> BinOpHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<BinaryOpExpr*>(expr);
    auto val_left = interpreter->eval(e->left_expr.get());
    auto val_right = interpreter->eval(e->right_expr.get());
    return std::move(interpreter->DoBinOp(val_left, val_right, expr->expr_type));
}

std::shared_ptr<Value> Interpreter::DoBinOp(const std::shared_ptr<Value>& v1, const std::shared_ptr<Value>& v2, ExprType type) {
    for (auto i : properties[v1->type])
    {
        for (auto k : properties[v2->type])
        {
            if (binaryOperations.contains({ .opType = type, .t1 = i, .t2 = k }))
                return std::move(binaryOperations[{ .opType = type, .t1 = i, .t2 = k }](this, v1, v2));
        }
    }
}

ValueID LeftIdentifierHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<IdentifierExpr*>(expr);
    auto& name = e->id;
    ValueID id = interpreter->symbolTable->getVariable(name);
    return id;
}

void blockHandler(Interpreter* interpreter, Stmt* stmt) {
    auto s = dynamic_cast<BlockStmt*>(stmt);
    interpreter->symbolTable = std::make_shared<SymbolTable>(interpreter->symbolTable);
    for (auto& i : s->stmts) {
        interpreter->exec(i.get());
    }
    interpreter->symbolTable->printValues(); // Todo
    interpreter->symbolTable = interpreter->symbolTable->up;
}

void declarationHandler(Interpreter* interpreter, Stmt* stmt) {
    auto s = dynamic_cast<DeclarationStmt*>(stmt);
    auto val = interpreter->eval(s->right.get());
    auto name = s->id->id;
    ValueID id = interpreter->memory.newOp(std::move(val));
    interpreter->symbolTable->addVariable(name, id);
}

void assignmentHandler(Interpreter* interpreter, Stmt* stmt) {
    auto s = dynamic_cast<AssignmentStmt*>(stmt);
    auto val = interpreter->eval(s->right.get());
    if (!s->left->left)
        throw std::exception("Expected left expression");
    ValueID id = interpreter->eval_left(s->left.get());
    interpreter->memory.set(id, std::move(val));
}

void ifHandler(Interpreter* interpreter, Stmt* stmt) {
    auto s = dynamic_cast<IfStmt*>(stmt);
    auto val = interpreter->eval(s->cond.get());
    if (val->ToBool()) {
        interpreter->exec(s->action.get());
    } else {
        interpreter->exec(s->else_action.get());
    }
}

void whileHandler(Interpreter* interpreter, Stmt* stmt) {
    auto s = dynamic_cast<WhileStmt*>(stmt);
    while (interpreter->eval(s->cond.get())->ToBool())
        interpreter->exec(s->action.get());
}

void doWhileHandler(Interpreter* interpreter, Stmt* stmt) {
    auto s = dynamic_cast<DoWhileStmt*>(stmt);
    do
        interpreter->exec(s->action.get());
    while (interpreter->eval(s->cond.get())->ToBool());
}

void noneHandler(Interpreter* interpreter, Stmt* stmt) {

}

std::shared_ptr<Value> Interpreter::eval(Expr* ast) {
    return std::move(expr_handlers[(int)ast->expr_type](this, ast));
}

void Interpreter::exec(Stmt* ast) {
    stmt_handlers[(int)ast->stmt_type](this, ast);
}

ValueID Interpreter::eval_left(Expr* ast) {
    return left_expr_handlers[(int)ast->expr_type](this, ast);
}

Interpreter::Interpreter() {
    symbolTable = std::make_shared<SymbolTable>();

    expr_handlers[(int)ExprType::Identifier] = identifierHandler;
    expr_handlers[(int)ExprType::BoolLiteral] = boolLiteralHandler;
    //expr_handlers[(int)ExprType::StringLiteral] = stringLiteralHandler;
    expr_handlers[(int)ExprType::IntLiteral] = intLiteralHandler;
    expr_handlers[(int)ExprType::FloatLiteral] = floatLiteralHandler;
    expr_handlers[(int)ExprType::Mul] = BinOpHandler;
    expr_handlers[(int)ExprType::Div] = BinOpHandler;
    expr_handlers[(int)ExprType::Add] = BinOpHandler;
    expr_handlers[(int)ExprType::Sub] = BinOpHandler;
    expr_handlers[(int)ExprType::Neg] = negHandler;
    expr_handlers[(int)ExprType::Eq] = BinOpHandler;
    expr_handlers[(int)ExprType::NotEq] = BinOpHandler;
    expr_handlers[(int)ExprType::Greater] = BinOpHandler;
    expr_handlers[(int)ExprType::Less] = BinOpHandler;
    expr_handlers[(int)ExprType::GreaterEq] = BinOpHandler;
    expr_handlers[(int)ExprType::LessEq] = BinOpHandler;
    expr_handlers[(int)ExprType::Not] = notHandler;
    left_expr_handlers[(int)ExprType::Identifier] = LeftIdentifierHandler;

    stmt_handlers[(int)StmtType::Declaration] = declarationHandler;
    stmt_handlers[(int)StmtType::Assignment] = assignmentHandler;
    stmt_handlers[(int)StmtType::Block] = blockHandler;
    stmt_handlers[(int)StmtType::If] = ifHandler;
    stmt_handlers[(int)StmtType::While] = whileHandler;
    stmt_handlers[(int)StmtType::DoWhile] = doWhileHandler;
    stmt_handlers[(int)StmtType::None] = noneHandler;

    addOp({ .opType = ExprType::Mul, .t1 = ValueProperty::Integer, .t2 = ValueProperty::Integer },
          [](Interpreter* interpreter, const std::shared_ptr<Value>& v1, const std::shared_ptr<Value>& v2){
            return std::make_shared<Value>(Value::Int(v1->ToInt() * v2->ToInt()));
        });
    addOp({ .opType = ExprType::Mul, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, const std::shared_ptr<Value>& v1, const std::shared_ptr<Value>& v2){
              return std::make_shared<Value>(Value::Float(v1->ToFloat() * v2->ToFloat()));
          });
    // Todo
    //addOp({ .opType = ExprType::Mul, .t1 = ValueProperty::List, .t2 = ValueProperty::Integer },
    //      [](Interpreter* interpreter, Value v1, Value v2){
    //          return Value::Int(v1.ToInt() * v2.ToInt());
    //      });
    addOp({ .opType = ExprType::Div, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, const std::shared_ptr<Value>& v1, const std::shared_ptr<Value>& v2){
              return std::make_shared<Value>(Value::Float(v1->ToFloat() / v2->ToFloat()));
          });
    addOp({ .opType = ExprType::IntDiv, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, const std::shared_ptr<Value>& v1, const std::shared_ptr<Value>& v2){
              return std::make_shared<Value>(Value::Int(v1->ToFloat() / v2->ToFloat()));
          });
    addOp({ .opType = ExprType::Add, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, const std::shared_ptr<Value>& v1, const std::shared_ptr<Value>& v2){
              return std::make_shared<Value>(Value::Float(v1->ToFloat() + v2->ToFloat()));
          });
    addOp({ .opType = ExprType::Add, .t1 = ValueProperty::Integer, .t2 = ValueProperty::Integer },
          [](Interpreter* interpreter, const std::shared_ptr<Value>& v1, const std::shared_ptr<Value>& v2){
              return std::make_shared<Value>(Value::Int(v1->ToInt() + v2->ToInt()));
          });
    addOp({ .opType = ExprType::Sub, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, const std::shared_ptr<Value>& v1, const std::shared_ptr<Value>& v2){
              return std::make_shared<Value>(Value::Float(v1->ToFloat() - v2->ToFloat()));
          });
    addOp({ .opType = ExprType::Sub, .t1 = ValueProperty::Integer, .t2 = ValueProperty::Integer },
          [](Interpreter* interpreter, const std::shared_ptr<Value>& v1, const std::shared_ptr<Value>& v2){
              return std::make_shared<Value>(Value::Int(v1->ToInt() - v2->ToInt()));
          });
    addOp({ .opType = ExprType::Eq, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, const std::shared_ptr<Value>& v1, const std::shared_ptr<Value>& v2){
              return std::make_shared<Value>(Value::Bool(v1->ToBool() == v2->ToBool()));
          });
    addOp({ .opType = ExprType::NotEq, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, const std::shared_ptr<Value>& v1, const std::shared_ptr<Value>& v2){
              return std::make_shared<Value>(Value::Bool(v1->ToBool() != v2->ToBool()));
          });
    addOp({ .opType = ExprType::Greater, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, const std::shared_ptr<Value>& v1, const std::shared_ptr<Value>& v2){
              return std::make_shared<Value>(Value::Bool(v1->ToFloat() > v2->ToFloat()));
          });
    addOp({ .opType = ExprType::GreaterEq, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, const std::shared_ptr<Value>& v1, const std::shared_ptr<Value>& v2){
              return std::make_shared<Value>(Value::Bool(v1->ToFloat() >= v2->ToFloat()));
          });
    addOp({ .opType = ExprType::Less, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, const std::shared_ptr<Value>& v1, const std::shared_ptr<Value>& v2){
              return std::make_shared<Value>(Value::Bool(v1->ToFloat() < v2->ToFloat()));
          });
    addOp({ .opType = ExprType::LessEq, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, const std::shared_ptr<Value>& v1, const std::shared_ptr<Value>& v2){
              return std::make_shared<Value>(Value::Bool(v1->ToFloat() <= v2->ToFloat()));
          });
    addOp({ .opType = ExprType::Or, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, const std::shared_ptr<Value>& v1, const std::shared_ptr<Value>& v2){
              return std::make_shared<Value>(Value::Bool(v1->ToBool() || v2->ToBool()));
          });
    addOp({ .opType = ExprType::And, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, const std::shared_ptr<Value>& v1, const std::shared_ptr<Value>& v2){
              return std::make_shared<Value>(Value::Bool(v1->ToBool() && v2->ToBool()));
          });
    addOp({ .opType = ExprType::Mod, .t1 = ValueProperty::Integer, .t2 = ValueProperty::Integer },
          [](Interpreter* interpreter, const std::shared_ptr<Value>& v1, const std::shared_ptr<Value>& v2){
              return std::make_shared<Value>(Value::Int(v1->ToInt() % v2->ToInt()));
          });
}

void Interpreter::addOp(operation op, const std::function<std::shared_ptr<Value>(Interpreter* interpreter, const std::shared_ptr<Value>& v1, const std::shared_ptr<Value>& v2)>& f) {
    binaryOperations[op] = f;
    if (op.t1 == op.t2)
        return;
    binaryOperations[{ .opType = op.opType, .t1 = op.t2, .t2 = op.t1 }] = [&f](Interpreter* interpreter, const std::shared_ptr<Value>& v1, const std::shared_ptr<Value>& v2) {
        return f(interpreter, v2, v1);
    };
}