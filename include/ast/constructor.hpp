#pragma once

#include "ast/base/node.hpp"

#include <vector>
#include <sstream>
#include <map>

class ConstructorNode : public Node {
public:
    std::shared_ptr<Node> inner;
    std::vector<std::string> args;
    std::map<size_t, std::string> typechecks;
    size_t argcount;
    ConstructorNode();
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};