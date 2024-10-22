#pragma once

#include "ast/base/node.hpp"

class IntegerNode : public Node {
public:
	long long value;
	IntegerNode(long long value);
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};