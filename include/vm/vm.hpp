#pragma once

#include "asm/asm.hpp"
#include "object/object.hpp"
#include "env/environment.hpp"

#include "eflags.hpp"

#include <stack>
#include <vector>

class VirtualMachine {
private:
    std::stack<std::shared_ptr<Object>> RuntimeStack;
    std::shared_ptr<Environment> outer, inner, current;
    std::stack<std::shared_ptr<Environment>> envsck;
    std::vector<std::shared_ptr<Asm>>* code;
    size_t pointer;
public:
    std::shared_ptr<Object> ExecuteAll();
};

// VM 应当函数调用级隔离，也就是函数调用新开 VM