#pragma once

#include "ast/base/node.hpp"

class BreakContinueNode : public Node {
public:
    bool isContinue;
public:
    BreakContinueNode(bool isContinue);
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};