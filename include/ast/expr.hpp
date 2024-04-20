#pragma once

#include "ast/base/node.hpp"

class ExprNode : public Node {
public:
    std::shared_ptr<Node> inner;
    ExprNode(std::shared_ptr<Node> inner) : inner(inner), Node(Node::Type::Expr) {}
public:
    std::string toString() const override {
        return inner->toString();
    }
    
    void storeInto(std::ostream& os) const override {
        BinaryOut::write_byte(os, char(Node::Type::Expr));
        inner->storeInto(os);
    }

    void readFrom(std::istream& is) override {
        inner = get_node(is);
    }
};