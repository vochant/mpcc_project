#pragma once

#include "ast/region.hpp"

#include <vector>

class ClassNode : public Node {
public:
	std::shared_ptr<Node> inner;
	ClassNode() : Node(Node::Type::Class) {}
public:
	std::string toString() const override {
		std::string str = "class";
		str += "{";
		str += inner->toString();
		str += "}";
		return str;
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Node::Type::Class));
		inner->storeInto(os);
	}

	void readFrom(std::istream& is) override {
		inner = get_node(is);
	}
};
