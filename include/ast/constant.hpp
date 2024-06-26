#pragma once

#include "ast/base/node.hpp"
#include "object/string.hpp"

class ConstantNode : public Node {
public:
    std::shared_ptr<Object> obj;
    ConstantNode(std::shared_ptr<Object> obj) : obj(obj), Node(Node::Type::Constant) {}
public:
    std::string toString() const override {
		if (obj->type == Object::Type::String) {
			return String::escape(obj->toString());
		}
        return obj->toString();
    }

    void storeInto(std::ostream& os) const override {
        BinaryOut::write_byte(os, char(Node::Type::Constant));
        obj->storeInto(os);
    }

    void readFrom(std::istream& is) override {
        obj = Object::get_object(is);
    }
};