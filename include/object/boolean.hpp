#pragma once

#include "object/base.hpp"

class Boolean : public Object {
public:
	bool _case;
	Boolean(bool _case = false) : _case(_case), Object(Object::Type::Boolean) {}
	Boolean(std::istream& is) : Object(Object::Type::Boolean) {
		readFrom(is);
	}
public:
	std::string toString() const override {
		return _case ? "true" : "false";
	}
	std::shared_ptr<Object> copy() const override {
		return std::make_shared<Boolean>(*this);
	}

	void assign(std::shared_ptr<Object> value) override {
		bool _is_mutable = isMutable;
		*this = *std::dynamic_pointer_cast<Boolean>(value);
		isMutable = _is_mutable;
	}

	std::string idstr() const override {
		return "boolean";
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Object::Type::Boolean));
		BinaryOut::write_byte(os, _case);
	}

	void readFrom(std::istream& is) override {
		_case = BinaryIn::read_byte(is);
	}
};