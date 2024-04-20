#pragma once

#include "ast/base/node.hpp"

#include <vector>
#include <sstream>

class ImportNode : public Node {
public:
    std::string item;
    ImportNode() : Node(Node::Type::Import) {}
public:
    std::string toString() const override {
        std::stringstream ss;
        ss << "import ";
        std::cout << item;
    }

    void storeInto(std::ostream& os) const override {
        BinaryOut::write_byte(os, char(Node::Type::Import));
        BinaryOut::write_string(os, item);
    }

    void readFrom(std::istream& is) override {
        item = BinaryIn::read_string(is);
    }
};