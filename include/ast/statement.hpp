#pragma once

#include "ast/base/node.hpp"

class StatementNode : public Node {
public:
	std::shared_ptr<Node> inner;
	StatementNode() : Node(Node::Type::Statement) {}
public:
	std::string toString() const override {
		return inner->toString() + ";";
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Node::Type::Statement));
		inner->storeInto(os);
	}

	void readFrom(std::istream& is) override {
		inner = get_node(is);
	}
};