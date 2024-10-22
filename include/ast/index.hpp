#pragma once

#include "ast/base/node.hpp"

class IndexNode : public Node {
public:
    std::shared_ptr<Node> left, index;
    IndexNode(std::shared_ptr<Node> left, std::shared_ptr<Node> index);
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};