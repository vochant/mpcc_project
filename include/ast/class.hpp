#pragma once

#include "ast/scope.hpp"

#include <vector>

class ClassNode : public Node {
public:
    std::shared_ptr<Node> inner;
    std::string _ext;
    ClassNode() : Node(Node::Type::Class), _ext(":") {}
public:
    std::string toString() const override {
        std::string str = "class";
        if (_ext != ":") {
            str += ":" + _ext;
        }
        str += "{";
        str += inner->toString();
        str += "}";
        return str;
    }

    void storeInto(std::ostream& os) const override {
        BinaryOut::write_byte(os, char(Node::Type::Class));
        BinaryOut::write_string(os, _ext);
        inner->storeInto(os);
    }

    void readFrom(std::istream& is) override {
        _ext = BinaryIn::read_string(is);
        inner = get_node(is);
    }
};
