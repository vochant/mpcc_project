#pragma once

#include "ast/base/node.hpp"

#include <vector>
#include <sstream>

class DestructorNode : public Node {
public:
    std::shared_ptr<Node> inner;
    DestructorNode();
};