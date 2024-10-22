#pragma once

#include "ast/base/node.hpp"

class ExprNode : public Node {
public:
    std::shared_ptr<Node> inner;
    ExprNode(std::shared_ptr<Node> inner);
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};