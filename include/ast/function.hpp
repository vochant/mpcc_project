#pragma once

#include "ast/base/node.hpp"
#include "object/base.hpp"

#include <vector>
#include <sstream>

class FunctionNode : public Node {
public:
	std::shared_ptr<Node> inner;
	std::vector<std::string> args;
	bool isLambda;
	FunctionNode() : Node(Node::Type::Function) {}
public:
	std::string toString() const override {
		std::stringstream ss;
		ss << "function(";
		for (size_t i = 0; i < args.size(); i++) {
			if (i != 0) {
				ss << ",";
			}
			ss << args[i];
		}
		ss << ")";
		ss << inner->toString();
		return ss.str();
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Node::Type::Function));
		BinaryOut::write_byte(os, isLambda);
		BinaryOut::write_data(os, args.size());
		for (auto i : args) {
			BinaryOut::write_string(os, i);
		}
		inner->storeInto(os);
	}

	void readFrom(std::istream& is) override {
		isLambda = BinaryIn::read_byte(is);
		size_t _len = BinaryIn::read_data<size_t>(is);
		while (_len--) {
			args.push_back(BinaryIn::read_string(is));
		}
		inner = Node::get_node(is);
	}
};