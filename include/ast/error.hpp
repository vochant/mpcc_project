#pragma once

#include "ast/base/node.hpp"

class ErrorNode : public Node {
public:
	ErrorNode() : Node(Node::Type::Error) {}
public:
	std::string toString() const override {
		return "<Error>";
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Node::Type::Error));
	}

	void readFrom(std::istream& is) override {}
};