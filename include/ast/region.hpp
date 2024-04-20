#pragma once

#include "ast/statement.hpp"

#include <vector>
#include <sstream>

class RegionNode : public Node {
public:
    std::vector<std::shared_ptr<Node>> statements;
    RegionNode() : Node(Node::Type::Region) {}
public:
    std::string toString() const override {
        std::stringstream ss;
        ss << "{";
        for (auto i : statements) {
            ss << i->toString();
        }
        ss << "}";
        return ss.str();
    }

    void mergeWith(std::shared_ptr<RegionNode> ano) {
        for (auto i : ano->statements) {
            statements.push_back(i);
        }
    }

    void storeInto(std::ostream& os) const override {
        BinaryOut::write_byte(os, char(Node::Type::Region));
        BinaryOut::write_data(os, statements.size());
        for (auto i : statements) {
            i->storeInto(os);
        }
    }

    void readFrom(std::istream& is) override {
        size_t _len = BinaryIn::read_data<size_t>(is);
        while (_len--) {
            statements.push_back(get_node(is));
        }
    }
};