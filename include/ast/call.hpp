#pragma once

#include <vector>
#include <sstream>

#include "ast/base/node.hpp"

// function-or-class(arg1, arg2, ...)

class CallNode : public Node {
public:
	std::shared_ptr<Node> to_run;
	std::vector<std::shared_ptr<Node>> args;
	CallNode(std::shared_ptr<Node> to_run) : to_run(to_run), Node(Node::Type::Call) {}
public:
	std::string toString() const override {
		std::stringstream ss;
		ss << to_run->toString();
		ss << "(";
		for (size_t i = 0; i < args.size(); i++) {
			if (i) ss << ",";
			ss << args[i]->toString();
		}
		ss << ")";
		return ss.str();
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Node::Type::Call));
		to_run->storeInto(os);
		BinaryOut::write_data(os, args.size());
		for (auto i : args) {
			i->storeInto(os);
		}
	}

	void readFrom(std::istream& is) override {
		to_run = get_node(is);
		size_t _len = BinaryIn::read_data<size_t>(is);
		while (_len--) {
			args.push_back(get_node(is));
		}
	}
};