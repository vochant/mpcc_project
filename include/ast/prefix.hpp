#pragma once

#include "ast/base/node.hpp"

class PrefixNode : public Node {
public:
    std::string _op;
    std::shared_ptr<Node> right;
    PrefixNode(std::string _op);
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};