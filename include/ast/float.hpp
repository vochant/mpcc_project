#pragma once

#include "ast/base/node.hpp"

class FloatNode : public Node {
public:
	double value;
	FloatNode(double value);
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};