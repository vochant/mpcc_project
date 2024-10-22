#pragma once

#include "ast/base/node.hpp"

class ErrorNode : public Node {
public:
    ErrorNode();
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};