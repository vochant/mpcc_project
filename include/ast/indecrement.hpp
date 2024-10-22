#pragma once

#include "ast/base/node.hpp"

class InDecrementNode : public Node {
public:
    bool isDecrement, isAfter;
    std::shared_ptr<Node> body;
    InDecrementNode(std::shared_ptr<Node> body, bool isDecrement, bool isAfter);
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};