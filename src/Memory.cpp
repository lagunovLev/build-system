#include "Memory.h"
#include <iostream>
#include <utility>
#include "values.h"

ValueID Memory::generateRandomID() {
    static ValueID id = 0;
    id++;
    return id;
}

ValueID Memory::newOp(std::shared_ptr<Value> val) {
    auto id = generateRandomID();
    while (mem.contains(id))
        id = generateRandomID();
    mem[id] = std::move(val);
    return id;
}

void Memory::deleteOP(ValueID id) {
    if (mem.contains(id))
        mem.erase(id);
    else
        throw std::exception("Not found in memory");
}

std::shared_ptr<Value> Memory::get(ValueID id) {
    if (mem.contains(id))
        return mem[id];
    else
        throw std::exception("Not found in memory");
}

void Memory::set(ValueID id, std::shared_ptr<Value> val) {
    mem[id] = std::move(val);
}

void Memory::print() {
    std::cout << "Memory:" << "\n";
    for (auto e : mem) {
        std::cout << e.first << '\t' << e.second->ToFloat() << '\n';
    }
}