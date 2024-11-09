#pragma once

#include "ast/base/node.hpp"

class ClassMember : public Node{
public:
    enum AccessControl : char {
        PUBLIC, PRIVATE
    } ac;
    enum Type : char {
        METHOD, STATIC, VALUE
    } type;
    std::string name;
    std::shared_ptr<Node> inner;
    bool isFinal;
public:
    ClassMember(std::string name, std::shared_ptr<Node> inner);
};