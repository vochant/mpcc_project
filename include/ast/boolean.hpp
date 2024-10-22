#pragma once

#include "ast/base/node.hpp"

class BooleanNode : public Node {
public:
	bool value;
	BooleanNode(bool value);
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};