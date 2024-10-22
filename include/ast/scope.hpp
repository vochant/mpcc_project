#pragma once

#include "ast/base/node.hpp"

#include <vector>
#include <sstream>

class ScopeNode : public Node {
public:
    std::vector<std::shared_ptr<Node>> statements;
    ScopeNode();
    std::vector<std::shared_ptr<Asm>> to_asm(ToAsmArgs args) const override;
    void mergeWith(std::shared_ptr<ScopeNode> ano) {
        statements.insert(statements.end(), ano->statements.begin(), ano->statements.end());
        // for (auto i : ano->statements) {
        //     statements.push_back(i);
        // }
    }
};