#pragma once
#include <unordered_map>
#include <memory>
class Value;

using ValueID = unsigned long long;

class Memory {
    std::unordered_map<ValueID, std::shared_ptr<Value>> mem;
    ValueID generateRandomID();
public:
    ValueID newOp(std::shared_ptr<Value> val);
    void deleteOP(ValueID id);
    std::shared_ptr<Value> get(ValueID id);
    void set(ValueID id, std::shared_ptr<Value> val);
    void print();
};