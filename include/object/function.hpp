#pragma once

#include "ast/base/node.hpp"
#include "object/base.hpp"
#include "object/environment.hpp"

#include <vector>
#include <sstream>

class Function : public Object {
public:
	std::shared_ptr<Node> inner;
	std::vector<std::string> args;
	std::shared_ptr<Environment> outerEnv;
	Function(std::shared_ptr<Environment> outerEnv) : outerEnv(outerEnv), Object(Object::Type::Function) {}
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

	std::string idstr() const override {
		return "function_" + std::to_string(args.size());
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Object::Type::Function));
	}

	void readFrom(std::istream& is) override {}

	void assign(std::shared_ptr<Object> value) override {
		bool _is_mutable = isMutable;
		*this = *std::dynamic_pointer_cast<Function>(value);
		isMutable = _is_mutable;
	}

	std::shared_ptr<Object> copy() const override {
		return std::make_shared<Function>(*this);
	}
};