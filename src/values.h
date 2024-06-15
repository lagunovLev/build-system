#pragma once
#include <string>
#include <memory>
#include <optional>

enum class ValueType {
    Int, String, Bool, Float
};

class Value
{
public:
    ValueType type;
    std::optional<int> int_val;
    std::optional<std::string> str_val;
    std::optional<float> float_val;
    std::optional<bool> bool_val;

    static Value Int(int value) {
        return Value { .type = ValueType::Int, .int_val = value };
    }

    static Value Float(float value) {
        return Value { .type = ValueType::Float, .float_val = value };
    }

    static Value Bool(bool value) {
        return Value { .type = ValueType::Bool, .bool_val = value };
    }

    static Value String(std::string& value) {
        return Value { .type = ValueType::String, .str_val = value };
    }

    float ToFloat() {
        if (type == ValueType::Float)
            return float_val.value();
        if (type == ValueType::Bool)
            return bool_val.value();
        if (type == ValueType::Int)
            return int_val.value();
    }

    bool IsNumeric() {
        return type != ValueType::String;
    }

    bool operator==(Value& val) {
        if (!IsNumeric() && !val.IsNumeric())
            return type == val.type && str_val == val.str_val;
        if (IsNumeric() && val.IsNumeric())
            return ToFloat() == val.ToFloat();
        return false;
    }
};