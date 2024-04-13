#pragma once

#include "ast/identifier.hpp"

#include <vector>
#include <sstream>

class EnumerateNode : public Node{
public:
	std::vector<std::string> items;
	EnumerateNode() : Node(Node::Type::Enumerate) {}
public:
	std::string toString() const override {
		std::stringstream ss;
		ss << "enum{";
		for (size_t i = 0; i < items.size(); i++) {
			if (i != 0) {
				ss << ",";
			}
			ss << items[i];
		}
		ss << "}";
		return ss.str();
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Node::Type::Enumerate));
		BinaryOut::write_data(os, items.size());
		for (auto i : items) {
			BinaryOut::write_string(os, i);
		}
	}

	void readFrom(std::istream& is) override {
		size_t _len = BinaryIn::read_data<size_t>(is);
		while (_len--) {
			items.push_back(BinaryIn::read_string(is));
		}
	}
};