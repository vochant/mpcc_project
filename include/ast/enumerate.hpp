#pragma once

#include "ast/identifier.hpp"

#include <vector>
#include <sstream>

class EnumerateNode : public Node{
public:
    std::vector<std::string> items;
    std::string _name;
    EnumerateNode();
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
};