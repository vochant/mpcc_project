#pragma once

#include "ast/base/node.hpp"

class GroupNode : public Node {
public:
    std::shared_ptr<Node> v;
    GroupNode(std::shared_ptr<Node> v);
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};