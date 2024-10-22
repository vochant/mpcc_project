#pragma once

#include "ast/base/node.hpp"

class StringNode : public Node {
public:
	std::string value;
	StringNode(std::string value);
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};