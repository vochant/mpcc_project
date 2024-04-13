#pragma once

#include "ast/base/node.hpp"
#include "ast/identifier.hpp"


// <creation> name1, name2=value

class RemoveNode : public Node {
public:
	std::string toRemove;
	RemoveNode(std::string toRemove) : toRemove(toRemove), Node(Node::Type::Remove) {}
public:
	std::string toString() {
		return "delete " + toRemove;
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Node::Type::Remove));
		BinaryOut::write_string(os, toRemove);
	}

	void readFrom(std::istream& is) override {
		toRemove = BinaryIn::read_string(is);
	}
};