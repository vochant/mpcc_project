#pragma once

#include "ast/base/node.hpp"

class DecorateNode : public Node {
public:
    std::shared_ptr<Node> decorator, inner;
public:
    DecorateNode();
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};