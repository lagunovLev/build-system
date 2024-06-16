#pragma once
#include <string>
#include <memory>
#include <optional>

enum class ValueType {
    Int, String, Bool, Float,
};

class Value
{
public:
    ValueType type;
    union {
        int int_val;
        float float_val;
        bool bool_val;
        std::string* str_val = nullptr;
    };

    Value() { }

    ~Value() {
        if (type == ValueType::String)
            delete str_val;
    }

    Value(const Value& other) {
        type = other.type;
        if (type == ValueType::String)
            str_val = new std::string(*other.str_val);
        else
            int_val = other.int_val;
    }

    Value& operator=(const Value& other) {
        if (this != &other) {
            if (type == ValueType::String)
                delete str_val;
            type = other.type;
            if (type == ValueType::String)
                str_val = new std::string(*other.str_val);
            else
                int_val = other.int_val;
        }
        return *this;
    }

    Value(Value&& other) noexcept {
        type = other.type;
        if (type == ValueType::String)
        {
            str_val = other.str_val;
            other.str_val = nullptr;
        }
        else int_val = other.int_val;
    }

    Value& operator=(Value&& other) noexcept {
        if (this != &other) {
            if (type == ValueType::String)
                delete str_val;
            type = other.type;
            if (type == ValueType::String)
            {
                str_val = other.str_val;
                other.str_val = nullptr;
            }
            else
                int_val = other.int_val;
        }
        return *this;
    }

    static Value Int(int value) {
        Value val;
        val.type = ValueType::Int;
        val.int_val = value;
        return std::move(val);
    }

    static Value Float(float value) {
        Value val;
        val.type = ValueType::Float;
        val.float_val = value;
        return std::move(val);
    }

    static Value Bool(bool value) {
        Value val;
        val.type = ValueType::Bool;
        val.bool_val = value;
        return std::move(val);
    }

    static Value String(std::string& value) {
        Value val;
        val.type = ValueType::String;
        val.str_val = new std::string(value);
        return std::move(val);
    }

    float ToFloat() {
        if (type == ValueType::Float)
            return float_val;
        if (type == ValueType::Bool)
            return bool_val;
        if (type == ValueType::Int)
            return int_val;
    }

    bool ToBool() {
        if (type == ValueType::Float)
            return bool(float_val);
        if (type == ValueType::Bool)
            return bool_val;
        if (type == ValueType::Int)
            return int_val;
    }

    int ToInt() {
        if (type == ValueType::Float)
            return int(float_val);
        if (type == ValueType::Bool)
            return bool_val;
        if (type == ValueType::Int)
            return int_val;
    }

    bool IsNumeric() {
        return type != ValueType::String;
    }

    bool IsInteger() {
        return type == ValueType::Int || type == ValueType::Bool;
    }

    bool operator==(Value& val) {
        //if (!IsNumeric() && !val.IsNumeric())
        //    return type == val.type && str_val == val.str_val;
        //if (IsNumeric() && val.IsNumeric())
        //    return ToFloat() == val.ToFloat();
        //return false;
        return type == val.type && int_val == val.int_val && str_val == val.str_val;
    }
};