#pragma once

#include "ast/base/node.hpp"

class ClassMember : public Node{
public:
    enum AccessControl : char {
        PUBLIC, PROTECTED, PRIVATE
    } ac;
    enum Type : char {
        METHOD, STATIC, VALUE
    } ctype;
    std::string name;
    std::shared_ptr<Node> inner;
    bool isFinal;
public:
    ClassMember(std::string name, std::shared_ptr<Node> inner);
};