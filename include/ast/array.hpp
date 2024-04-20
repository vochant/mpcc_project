#pragma once

#include "ast/base/node.hpp"
#include <vector>
#include <memory>
#include <sstream>

class ArrayNode : public Node {
public:
    std::vector<std::shared_ptr<Node>> elements;
    ArrayNode() : Node(Node::Type::Array) {}
public:
    std::string toString() const override {
        std::stringstream result;
        result << "[";
        bool isFirst = true;
        for (auto i : elements) {
            if (isFirst) {
                isFirst = false;
            }
            else {
                result << ", ";
            }
            result << i->toString();
        }
        result << "]";
        return result.str();
    }

    void storeInto(std::ostream& os) const override {
        BinaryOut::write_byte(os, char(Node::Type::Array));
        BinaryOut::write_data(os, elements.size());
        for (auto i : elements) {
            i->storeInto(os);
        }
    }

    void readFrom(std::istream& is) override {
        size_t _len = BinaryIn::read_data<size_t>(is);
        while (_len--) {
            elements.push_back(get_node(is));
        }
    }
};