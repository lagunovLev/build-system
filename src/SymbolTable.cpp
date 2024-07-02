#include "SymbolTable.h"
#include <iostream>
#include <utility>

void SymbolTable::addVariable(std::string& name, ValueID value) {
    values[name] = value;
}

void SymbolTable::removeVariable(std::string& name) {
    values.erase(name);
}

ValueID SymbolTable::getVariable(std::string& name) {
    if (values.contains(name))
        return values[name];
    return up->getVariable(name);
}

void SymbolTable::setVariable(std::string& name, ValueID value) {
    if (values.contains(name))
        values[name] = value;
    else
        up->setVariable(name, value);
}

void SymbolTable::printValues() {
    std::cout << "Symbol table values:\n";
    for (auto e : values) {
        std::cout << e.first << '\t' << e.second << '\n';
    }
}