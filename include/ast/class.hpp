#pragma once

#include "ast/scope.hpp"

#include <vector>

class ClassNode : public Node {
public:
    std::vector<std::shared_ptr<Node>> inner;
    std::string _ext, _name;
    ClassNode();
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};
