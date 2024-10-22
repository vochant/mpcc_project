#pragma once

#include "ast/base/node.hpp"

class TernaryNode : public Node{
public:
    std::shared_ptr<Node> _cond, _if, _else;
    TernaryNode(std::shared_ptr<Node> _cond);
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};