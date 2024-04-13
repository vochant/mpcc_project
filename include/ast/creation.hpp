#pragma once

#include "ast/base/node.hpp"
#include "ast/identifier.hpp"
#include "object/base.hpp"

#include <utility>
#include <sstream>
#include <vector>

// <creation> name1, name2=value

class CreationNode : public Node {
public:
	std::vector<std::pair<std::string, std::shared_ptr<Node>>> creations;
	bool isGlobal, isPrivate, allowOverwrite, isConst;
	CreationNode(bool isGlobal, bool isPrivate, bool allowOverwrite, bool isConst) : isGlobal(isGlobal), isPrivate(isPrivate), allowOverwrite(allowOverwrite), isConst(isConst), Node(Node::Type::Creation) {}
public:
	std::string toString() {
		std::stringstream ss;
		if (isConst) {
			ss << "const ";
		}
		if (isGlobal) {
			ss << "global ";
		}
		else if (isPrivate) {
			ss << "private ";
		}
		else if (allowOverwrite) {
			ss << "let ";
		}
		else {
			ss << "var ";
		}
		for (size_t i = 0; i < creations.size(); i++) {
			if (i != 0) {
				ss << ",";
			}
			ss << creations[i].first;
			if (creations[i].second) {
				ss << "=" << creations[i].second->toString();
			}
		}
		ss << ";";
		return ss.str();
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Node::Type::Creation));
		BinaryOut::write_byte(os, (isConst ? 8 : 0) | (allowOverwrite ? 4 : 0) | (isGlobal ? 2 : 0) | (isPrivate ? 1 : 0));
		BinaryOut::write_data(os, creations.size());
		for (auto i : creations) {
			BinaryOut::write_string(os, i.first);
			i.second->storeInto(os);
		}
	}

	void readFrom(std::istream& is) override {
		char _info = BinaryIn::read_byte(is);
		isConst = _info & 8;
		allowOverwrite = _info & 4;
		isGlobal = _info & 2;
		isPrivate = _info & 1;
		size_t _len = BinaryIn::read_data<size_t>(is);
		while (_len--) {
			creations.push_back(std::make_pair(BinaryIn::read_string(is), get_node(is)));
		}
	}
};