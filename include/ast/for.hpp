#pragma once

#include "ast/identifier.hpp"

// for varname(range-as-list) something

class ForNode : public Node {
public:
    std::shared_ptr<Node> _var;
    std::shared_ptr<Node> _elem, _body;
    ForNode();
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};