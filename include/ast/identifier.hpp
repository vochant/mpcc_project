#pragma once

#include "ast/base/node.hpp"

class IdentifierNode : public Node {
public:
	std::string id;
	IdentifierNode(std::string id) : id(id), Node(Node::Type::Identifier) {}
public:
	std::string toString() const override {
		return id;
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Node::Type::Identifier));
		BinaryOut::write_string(os, id);
	}

	void readFrom(std::istream& is) override {
		id = BinaryIn::read_string(is);
	}
};