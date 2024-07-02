#pragma once
#include <string>
#include <unordered_map>
#include "values.h"
#include <memory>
#include <utility>
#include "Memory.h"

class SymbolTable {
    std::unordered_map<std::string, ValueID> values;
public:
    std::shared_ptr<SymbolTable> up;
    SymbolTable(std::shared_ptr<SymbolTable> table) {
        up = std::move(table);
    }
    SymbolTable() = default;
    void addVariable(std::string& name, ValueID value);
    void removeVariable(std::string& name);
    ValueID getVariable(std::string& name);
    void setVariable(std::string& name, ValueID value);
    void printValues();
};
