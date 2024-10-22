#pragma once

#include "ast/identifier.hpp"

class WhileNode : public Node {
public:
    std::shared_ptr<Node> _cond, _body;
    bool isDoWhile;
    WhileNode(bool isDoWhile);
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};