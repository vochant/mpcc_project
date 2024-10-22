#pragma once

#include "ast/base/node.hpp"

// if (condition) something
// else something

class IfNode : public Node {
public:
    std::shared_ptr<Node> _cond, _then, _else;
    IfNode();
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};