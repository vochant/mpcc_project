#pragma once

#include "object/base.hpp"

class Null : public Object {
public:
	Null() : Object(Object::Type::Null) {}
public:
	std::string toString() const override {
		return "null";
	}

	std::shared_ptr<Object> copy() const override {
		return std::make_shared<Null>();
	}

	std::string idstr() const override {
		return "null";
	}

	void assign(std::shared_ptr<Object> value) override {
		// Nothing to assign
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Object::Type::Null));
	}
	void readFrom(std::istream& is) override {}
};