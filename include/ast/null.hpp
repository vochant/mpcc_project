#pragma once

#include "ast/base/node.hpp"

class NullNode : public Node {
public:
	NullNode();
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};