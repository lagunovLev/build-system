#include "Interpreter.h"
#include <utility>

Value identifierHandler(Interpreter* interpreter, Expr* expr) {
    return Value::Float(0);
}

Value boolLiteralHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<BoolLiteralExpr*>(expr);
    return Value::Bool(e->value);
}

Value stringLiteralHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<StringLiteralExpr*>(expr);
    return Value::String(e->value);
}

Value intLiteralHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<IntLiteralExpr*>(expr);
    return Value::Int(e->value);
}

Value floatLiteralHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<FloatLiteralExpr*>(expr);
    return Value::Float(e->value);
}

Value negHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<UnaryOpExpr*>(expr);
    auto val = interpreter->exec(e->expr.get());
    if (val.type == ValueType::Int || val.type == ValueType::Bool)
        return Value::Int(-val.ToInt());
    if (val.type == ValueType::Float)
        return Value::Float(-val.ToFloat());
}

Value notHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<UnaryOpExpr*>(expr);
    auto val = interpreter->exec(e->expr.get());
    if (val.IsNumeric())
        return Value::Bool(!val.ToBool());
}

Value BinOpHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<BinaryOpExpr*>(expr);
    auto val_left = interpreter->exec(e->left_expr.get());
    auto val_right = interpreter->exec(e->right_expr.get());
    return interpreter->DoBinOp(val_left, val_right, expr->expr_type);
}

Value Interpreter::DoBinOp(Value v1, Value v2, ExprType type) {
    for (auto i : properties[v1.type])
    {
        for (auto k : properties[v2.type])
        {
            if (binaryOperations.contains({ .opType = type, .t1 = i, .t2 = k }))
                return binaryOperations[{ .opType = type, .t1 = i, .t2 = k }](this, v1, v2);
        }
    }
}

Value Interpreter::exec(Expr* e) {
    return expr_handlers[(int)e->expr_type](this, e);
}

Interpreter::Interpreter() {
    expr_handlers[(int)ExprType::Identifier] = identifierHandler;
    expr_handlers[(int)ExprType::BoolLiteral] = boolLiteralHandler;
    expr_handlers[(int)ExprType::StringLiteral] = stringLiteralHandler;
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

    addOp({ .opType = ExprType::Mul, .t1 = ValueProperty::Integer, .t2 = ValueProperty::Integer },
          [](Interpreter* interpreter, Value v1, Value v2){
            return Value::Int(v1.ToInt() * v2.ToInt());
        });
    addOp({ .opType = ExprType::Mul, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, Value v1, Value v2){
              return Value::Float(v1.ToFloat() * v2.ToFloat());
          });
    // Todo
    //addOp({ .opType = ExprType::Mul, .t1 = ValueProperty::List, .t2 = ValueProperty::Integer },
    //      [](Interpreter* interpreter, Value v1, Value v2){
    //          return Value::Int(v1.ToInt() * v2.ToInt());
    //      });
    addOp({ .opType = ExprType::Div, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, Value v1, Value v2){
              return Value::Float(v1.ToFloat() / v2.ToFloat());
          });
    addOp({ .opType = ExprType::IntDiv, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, Value v1, Value v2){
              return Value::Int(v1.ToFloat() / v2.ToFloat());
          });
    addOp({ .opType = ExprType::Add, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, Value v1, Value v2){
              return Value::Float(v1.ToFloat() + v2.ToFloat());
          });
    addOp({ .opType = ExprType::Add, .t1 = ValueProperty::Integer, .t2 = ValueProperty::Integer },
          [](Interpreter* interpreter, Value v1, Value v2){
              return Value::Int(v1.ToInt() + v2.ToInt());
          });
    addOp({ .opType = ExprType::Sub, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, Value v1, Value v2){
              return Value::Float(v1.ToFloat() - v2.ToFloat());
          });
    addOp({ .opType = ExprType::Sub, .t1 = ValueProperty::Integer, .t2 = ValueProperty::Integer },
          [](Interpreter* interpreter, Value v1, Value v2){
              return Value::Int(v1.ToInt() - v2.ToInt());
          });
    addOp({ .opType = ExprType::Eq, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, Value v1, Value v2){
              return Value::Bool(v1.ToBool() == v2.ToBool());
          });
    addOp({ .opType = ExprType::NotEq, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, Value v1, Value v2){
              return Value::Bool(v1.ToBool() != v2.ToBool());
          });
    addOp({ .opType = ExprType::Greater, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, Value v1, Value v2){
              return Value::Bool(v1.ToFloat() > v2.ToFloat());
          });
    addOp({ .opType = ExprType::GreaterEq, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, Value v1, Value v2){
              return Value::Bool(v1.ToFloat() >= v2.ToFloat());
          });
    addOp({ .opType = ExprType::Less, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, Value v1, Value v2){
              return Value::Bool(v1.ToFloat() < v2.ToFloat());
          });
    addOp({ .opType = ExprType::LessEq, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, Value v1, Value v2){
              return Value::Bool(v1.ToFloat() <= v2.ToFloat());
          });
    addOp({ .opType = ExprType::Or, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, Value v1, Value v2){
              return Value::Bool(v1.ToBool() || v2.ToBool());
          });
    addOp({ .opType = ExprType::And, .t1 = ValueProperty::Numeric, .t2 = ValueProperty::Numeric },
          [](Interpreter* interpreter, Value v1, Value v2){
              return Value::Bool(v1.ToBool() && v2.ToBool());
          });
    addOp({ .opType = ExprType::Mod, .t1 = ValueProperty::Integer, .t2 = ValueProperty::Integer },
          [](Interpreter* interpreter, Value v1, Value v2){
              return Value::Int(v1.ToInt() % v2.ToInt());
          });
}

void Interpreter::addOp(operation op, const std::function<Value(Interpreter* interpreter, Value v1, Value v2)>& f) {
    binaryOperations[op] = f;
    if (op.t1 == op.t2)
        return;
    binaryOperations[{ .opType = op.opType, .t1 = op.t2, .t2 = op.t1 }] = [&f](Interpreter* interpreter, Value v1, Value v2) {
        return f(interpreter, std::move(v2), std::move(v1));
    };
}