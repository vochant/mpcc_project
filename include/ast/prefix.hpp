#pragma once

#include "ast/base/node.hpp"

class PrefixNode : public Node {
public:
    std::string _op;
    std::shared_ptr<Node> right;
    PrefixNode(std::string _op) : _op(_op), Node(Node::Type::Prefix) {}
public:
    std::string toString() const override {
        return _op + right->toString();
    }
    
    void storeInto(std::ostream& os) const override {
        BinaryOut::write_byte(os, char(Node::Type::Prefix));
        BinaryOut::write_string(os, _op);
        right->storeInto(os);
    }

    void readFrom(std::istream& is) override {
        _op = BinaryIn::read_string(is);
        right = get_node(is);
    }
};