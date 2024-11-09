#pragma once

#include "object/object.hpp"
#include "env/environment.hpp"

#include <vector>

class Executable : public Object {
public:
    enum ExecType {
        Function, // A pointer in code segment
        NativeFunction, // A C++ std::function object
        FarFunction, // A sequence of assembly code
        Placeholder // A placeholder (like super)
    } etype;
    Executable(ExecType etype);
    virtual std::shared_ptr<Object> call(std::vector<std::shared_ptr<Object>> args) = 0;
};