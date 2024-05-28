#pragma once

#include "ast/base/node.hpp"

class GroupNode : public Node {
public:
    std::shared_ptr<Node> v;
    GroupNode(std::shared_ptr<Node> v) : v(v), Node(Node::Type::Group) {}
public:
    std::string toString() const override {
        return "(" + v->toString() + ")";
    }
    
    void storeInto(std::ostream& os) const override {
        BinaryOut::write_byte(os, char(Node::Type::Group));
        v->storeInto(os);
    }

    void readFrom(std::istream& is) override {
        v = get_node(is);
    }
};