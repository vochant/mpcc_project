#pragma once

#include "ast/scope.hpp"

#include <vector>

class ClassNode : public Node {
public:
    std::vector<std::shared_ptr<Node>> inner;
    std::string _ext, _name;
    ClassNode();
};
