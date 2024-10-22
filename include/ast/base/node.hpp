#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <vector>

#include "util.hpp"
#include "asm/asm.hpp"

void illegal_program_error();

class Node {
public:
    enum class Type {
        Creation, Call, If, For, While,
        Infix, Prefix, Scope, Function,
        Identifier, Class, Remove, CFor,
        Program, Import, Assign, Statement,
        Enumerate, Error, Index, TypeName,
        InDecrement, Expr, Array,
        Return, Ternary, Group, Integer, Float,
		Boolean, String, Null, FunctionDef,
        ClassMember, New, Decorate, Constructor,
        Destructor, BreakContinue
    } type;
    Node(Type type);
    virtual std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const = 0;
};