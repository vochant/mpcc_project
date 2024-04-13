#pragma once

#include "ast/base/node.hpp"

class TernaryNode : public Node{
public:
	std::shared_ptr<Node> _cond, _if, _else;
	TernaryNode(std::shared_ptr<Node> _cond) : _cond(_cond), Node(Node::Type::Ternary) {}
public:
	std::string toString() const override {
		return _cond->toString() + "?" + _if->toString() + ":" + _else->toString();
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Node::Type::Ternary));
		_cond->storeInto(os);
		_if->storeInto(os);
		_else->storeInto(os);
	}

	void readFrom(std::istream& is) override {
		_cond = get_node(is);
		_if = get_node(is);
		_else = get_node(is);
	}
};