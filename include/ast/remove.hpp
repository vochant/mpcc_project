#pragma once

#include "ast/base/node.hpp"
#include "ast/identifier.hpp"


// <creation> name1, name2=value

class RemoveNode : public Node {
public:
    std::string toRemove;
    RemoveNode(std::string toRemove);
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};