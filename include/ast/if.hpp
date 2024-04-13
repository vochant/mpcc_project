#pragma once

#include "ast/base/node.hpp"

// if (condition) something
// else something

class IfNode : public Node {
public:
	std::shared_ptr<Node> _cond, _then, _else;
	IfNode() : Node(Node::Type::If) {}
public:
	std::string toString() const override {
		std::string _res = "if(" + _cond->toString() + ")" + _then->toString();
		if (_else) {
			if (isalnum(_res[_res.length() - 1]) || _res[_res.length() - 1] == '_') _res += " ";
			_res += "else";
			std::string _el = _else->toString();
			if (isalnum(_el[0]) || _el[0] == '_') {
				_res += " ";
			}
			_res += _el;
		}
		return _res;
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Node::Type::If));
		_cond->storeInto(os);
		_then->storeInto(os);
		BinaryOut::write_byte(os, _else ? 1 : 0);
		if (_else) _else->storeInto(os);
	}

	void readFrom(std::istream& is) override {
		_cond = get_node(is);
		_then = get_node(is);
		bool _has_else = BinaryIn::read_byte(is);
		if (_has_else) _else = get_node(is);
	}
};