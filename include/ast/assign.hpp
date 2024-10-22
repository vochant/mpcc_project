#pragma once

#include "ast/base/node.hpp"

class AssignNode : public Node {
public:
    std::string _op;
    std::shared_ptr<Node> left, right;
    AssignNode(std::string _op);
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};