#pragma once

#include "ast/identifier.hpp"

// while(condition) something

class WhileNode : public Node {
public:
	std::shared_ptr<Node> _cond, _body;
	bool isDoWhile;
	WhileNode(bool isDoWhile) : isDoWhile(isDoWhile), Node(Node::Type::While) {}
public:
	std::string toString() const override {
		return std::string(isDoWhile ? "do" : "") + "while(" + _cond->toString() + ")" + _body->toString();
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Node::Type::While));
		_cond->storeInto(os);
		_body->storeInto(os);
	}

	void readFrom(std::istream& is) override {
		_cond = get_node(is);
		_body = get_node(is);
	}
};