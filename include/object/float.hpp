#pragma once

#include "object/base.hpp"

class Float : public Object {
public:
	double value;
	Float(double value = 0) : value(value), Object(Object::Type::Float) {}
	Float(std::istream& is) : Object(Object::Type::Float) {
		readFrom(is);
	}
public:
	std::string toString() const override {
		return toString();
	}

	std::shared_ptr<Object> copy() const override {
		return std::make_shared<Float>(*this);
	}

	std::string idstr() const override {
		return "float";
	}

	void assign(std::shared_ptr<Object> value) override {
		bool _is_mutable = isMutable;
		*this = *std::dynamic_pointer_cast<Float>(value);
		isMutable = _is_mutable;
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Object::Type::Float));
		BinaryOut::write_data(os, value);
	}

	void readFrom(std::istream& is) override {
		value = BinaryIn::read_data<double>(is);
	}
};