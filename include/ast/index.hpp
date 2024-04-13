#pragma once

#include "ast/base/node.hpp"

class IndexNode : public Node {
public:
	std::shared_ptr<Node> left, index;
	IndexNode(std::shared_ptr<Node> left, std::shared_ptr<Node> index) : left(left), index(index), Node(Node::Type::Index) {}
public:
	std::string toString() const override {
		return left->toString() + "[" + index->toString() + "]";
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Node::Type::Identifier));
		left->storeInto(os);
		index->storeInto(os);
	}

	void readFrom(std::istream& is) override {
		left = get_node(is);
		index = get_node(is);
	}
};