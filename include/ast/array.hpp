#pragma once

#include "ast/base/node.hpp"
#include <vector>
#include <memory>
#include <sstream>

class ArrayNode : public Node {
public:
    std::vector<std::shared_ptr<Node>> elements;
    ArrayNode();
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};