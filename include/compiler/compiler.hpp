#pragma once

#include "ast/base/node.hpp"

class Compiler {
private:
    std::shared_ptr<Node> inner;
public:
    std::vector<std::shared_ptr<Asm>> compile(size_t& flags);
    Compiler(std::shared_ptr<Node> inner);
};