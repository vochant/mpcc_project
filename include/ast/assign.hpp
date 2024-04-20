#pragma once

#include "ast/base/node.hpp"

class AssignNode : public Node {
public:
    std::string _op;
    std::shared_ptr<Node> left, right;
    AssignNode(std::string _op) : _op(_op), Node(Node::Type::Assign) {}
public:
    std::string toString() const override {
        return left->toString() + _op + right->toString();
    }
    
    void storeInto(std::ostream& os) const override {
        BinaryOut::write_byte(os, char(Node::Type::Assign));
        BinaryOut::write_string(os, _op);
        left->storeInto(os);
        right->storeInto(os);
    }

    void readFrom(std::istream& is) override {
        _op = BinaryIn::read_string(is);
        left = get_node(is);
        right = get_node(is);
    }
};