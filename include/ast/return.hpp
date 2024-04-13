#pragma once

#include "ast/base/node.hpp"

class ReturnNode : public Node {
public:
	std::shared_ptr<Node> obj;
	ReturnNode(std::shared_ptr<Node> obj) : obj(obj), Node(Node::Type::Return) {}
public:
	std::string toString() const override {
		return "return " + obj->toString();
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Node::Type::Return));
		obj->storeInto(os);
	}

	void readFrom(std::istream& is) override {
		obj = get_node(is);
	}
};