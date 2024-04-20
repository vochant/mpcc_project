#pragma once

#include <string>
#include <iostream>
#include <memory>

#include "storage/binary.hpp"

void illegal_program_error();

class Node {
public:
    enum class Type {
        Creation, Call, If, For, While,
        Infix, Prefix, Region, Function,
        Identifier, Constant, Class, Remove,
        Program, Import, Assign, Statement,
        Enumerate, Error, Index, TypeName,
        InDecrement, Expr, Array, Function,
        Return, Ternary
    } type;
    Node(Type type) : type(type) {}
public:
    virtual std::string toString() const = 0;
    virtual void storeInto(std::ostream& os) const = 0;
    virtual void readFrom(std::istream& is) = 0;
public:
    static std::shared_ptr<Node> get_node(std::istream& is);
};