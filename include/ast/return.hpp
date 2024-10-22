#pragma once

#include "ast/base/node.hpp"

class ReturnNode : public Node {
public:
    std::shared_ptr<Node> obj;
    ReturnNode(std::shared_ptr<Node> obj);
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};