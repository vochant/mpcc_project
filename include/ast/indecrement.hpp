#pragma once

#include "ast/base/node.hpp"

class InDecrementNode : public Node {
public:
	bool isDecrement, isAfter;
	std::shared_ptr<Node> body;
	InDecrementNode(std::shared_ptr<Node> body, bool isDecrement, bool isAfter) : body(body), isDecrement(isDecrement), isAfter(isAfter), Node(Node::Type::InDecrement) {}
public:
	std::string toString() const override {
		if (isDecrement) {
			if (isAfter) {
				return body->toString() + "--";
			}
			else {
				return "--" + body->toString();
			}
		}
		else {
			if (isAfter) {
				return body->toString() + "++";
			}
			else {
				return "++" + body->toString();
			}
		}
	}
	
	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Node::Type::Assign));
		BinaryOut::write_byte(os, (isDecrement ? 2 : 0) | (isAfter ? 1 : 0));
		body->storeInto(os);
	}

	void readFrom(std::istream& is) override {
		char _mode = BinaryIn::read_byte(is);
		isDecrement = _mode & 2;
		isAfter = _mode & 1;
		body = get_node(is);
	}
};