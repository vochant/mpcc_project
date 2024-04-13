#pragma once

#include "ast/identifier.hpp"

// for varname(range-as-list) something

class ForNode : public Node {
public:
	std::shared_ptr<Node> _var;
	std::shared_ptr<Node> _elem, _body;
	ForNode() : Node(Node::Type::For) {}
public:
	std::string toString() const override {
		return "for " + _var->toString() + "(" + _elem->toString() + ")" + _body->toString();
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Node::Type::For));
		_var->storeInto(os);
		_elem->storeInto(os);
		_body->storeInto(os);
	}

	void readFrom(std::istream& is) override {
		_var = get_node(is);
		_elem = get_node(is);
		_body = get_node(is);
	}
};