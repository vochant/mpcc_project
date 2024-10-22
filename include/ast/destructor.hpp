#pragma once

#include "ast/base/node.hpp"

#include <vector>
#include <sstream>

class DestructorNode : public Node {
public:
    std::shared_ptr<Node> inner;
    DestructorNode();
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};