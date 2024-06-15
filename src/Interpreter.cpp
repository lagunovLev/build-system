#include "Interpreter.h"

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

Value mulHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<BinaryOpExpr*>(expr);
    auto val_left = interpreter->exec(e->left_expr.get());
    auto val_right = interpreter->exec(e->right_expr.get());
    return Value::Float(val_left.ToFloat() * val_right.ToFloat());
}

Value divHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<BinaryOpExpr*>(expr);
    auto val_left = interpreter->exec(e->left_expr.get());
    auto val_right = interpreter->exec(e->right_expr.get());
    return Value::Float(val_left.ToFloat() / val_right.ToFloat());
}

Value subHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<BinaryOpExpr*>(expr);
    auto val_left = interpreter->exec(e->left_expr.get());
    auto val_right = interpreter->exec(e->right_expr.get());
    return Value::Float(val_left.ToFloat() - val_right.ToFloat());
}

Value addHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<BinaryOpExpr*>(expr);
    auto val_left = interpreter->exec(e->left_expr.get());
    auto val_right = interpreter->exec(e->right_expr.get());
    return Value::Float(val_left.ToFloat() + val_right.ToFloat());
}

Value negHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<UnaryOpExpr*>(expr);
    auto val = interpreter->exec(e->expr.get());
    return Value::Float(-val.ToFloat());
}

Value eqHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<BinaryOpExpr*>(expr);
    auto val_left = interpreter->exec(e->left_expr.get());
    auto val_right = interpreter->exec(e->right_expr.get());
    return Value::Float(val_left == val_right);
}

Value notEqHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<BinaryOpExpr*>(expr);
    auto val_left = interpreter->exec(e->left_expr.get());
    auto val_right = interpreter->exec(e->right_expr.get());
    return Value::Float(val_left != val_right);
}

Value greaterHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<BinaryOpExpr*>(expr);
    auto val_left = interpreter->exec(e->left_expr.get());
    auto val_right = interpreter->exec(e->right_expr.get());
    return Value::Float(val_left.ToFloat() > val_right.ToFloat());
}

Value lessHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<BinaryOpExpr*>(expr);
    auto val_left = interpreter->exec(e->left_expr.get());
    auto val_right = interpreter->exec(e->right_expr.get());
    return Value::Float(val_left.ToFloat() < val_right.ToFloat());
}

Value greaterEqHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<BinaryOpExpr*>(expr);
    auto val_left = interpreter->exec(e->left_expr.get());
    auto val_right = interpreter->exec(e->right_expr.get());
    return Value::Float(val_left.ToFloat() >= val_right.ToFloat());
}

Value lessEqHandler(Interpreter* interpreter, Expr* expr) {
    auto e = dynamic_cast<BinaryOpExpr*>(expr);
    auto val_left = interpreter->exec(e->left_expr.get());
    auto val_right = interpreter->exec(e->right_expr.get());
    return Value::Float(val_left.ToFloat() <= val_right.ToFloat());
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
    expr_handlers[(int)ExprType::Mul] = mulHandler;
    expr_handlers[(int)ExprType::Div] = divHandler;
    expr_handlers[(int)ExprType::Add] = addHandler;
    expr_handlers[(int)ExprType::Sub] = subHandler;
    expr_handlers[(int)ExprType::Neg] = negHandler;
    expr_handlers[(int)ExprType::Eq] = eqHandler;
    expr_handlers[(int)ExprType::NotEq] = notEqHandler;
    expr_handlers[(int)ExprType::Greater] = greaterHandler;
    expr_handlers[(int)ExprType::Less] = lessHandler;
    expr_handlers[(int)ExprType::GreaterEq] = greaterEqHandler;
    expr_handlers[(int)ExprType::LessEq] = lessEqHandler;
}

