#pragma once
#include "AST.h"
#include <functional>
#include "values.h"
#include <boost/container_hash/hash.hpp>
#include <utility>

enum class ValueProperty {
    Numeric, Integer, List
};

struct operation {
    ExprType opType;
    ValueProperty t1;
    ValueProperty t2;

    bool operator==(const operation& other) const {
        return opType == other.opType && t1 == other.t1 && t2 == other.t2;
    }
};

template<>
struct std::hash<operation>
{
    std::size_t operator()(const operation& o) const noexcept
    {
        std::size_t s = 0;
        std::size_t h1 = std::hash<ValueProperty>{}(o.t1);
        std::size_t h2 = std::hash<ValueProperty>{}(o.t2);
        std::size_t h3 = std::hash<ExprType>{}(o.opType);
        boost::hash_combine(s, h1);
        boost::hash_combine(s, h2);
        boost::hash_combine(s, h3);
        return s;
    }
};

class Interpreter
{
    std::unordered_map<operation, std::function<Value(Interpreter* interpreter, Value v1, Value v2)>> binaryOperations;
    void addOp(operation op, const std::function<Value(Interpreter* interpreter, Value v1, Value v2)>& f);
public:
    Value DoBinOp(Value v1, Value v2, ExprType type);
    std::unordered_map<ValueType, std::vector<ValueProperty>> properties = {
            std::pair<ValueType, std::vector<ValueProperty>>(ValueType::Bool, { ValueProperty::Integer, ValueProperty::Numeric }),
            std::pair<ValueType, std::vector<ValueProperty>>(ValueType::Int, { ValueProperty::Integer, ValueProperty::Numeric }),
            std::pair<ValueType, std::vector<ValueProperty>>(ValueType::Float, { ValueProperty::Numeric }),
            std::pair<ValueType, std::vector<ValueProperty>>(ValueType::String, { ValueProperty::List }),
    };

    std::function<Value(Interpreter* interpreter, Expr* expr)> expr_handlers[(int)ExprType::Last];
    Interpreter();

    Value exec(Expr* ast);
};