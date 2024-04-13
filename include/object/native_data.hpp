#pragma once

#include "object/base.hpp"
#include <any>
#include <program/vm_options.hpp>

class NativeData : public Object {
public:
	std::any value;
	NativeData(std::any value = 0ll) : value(value), Object(Object::Type::NativeData) {}
public:
	std::string toString() const override {
		if (COLORED_OUTPUTS) {
			return "\033[32m[Native Data]\033[0m";
		}
		else {
			return "[Native Data]";
		}
	}

	std::shared_ptr<Object> copy() const override {
		return std::make_shared<NativeData>(*this);
	}

	std::string idstr() const override {
		return "native_" + std::string(value.type().name());
	}

	void assign(std::shared_ptr<Object> value) override {
		bool _is_mutable = isMutable;
		*this = *std::dynamic_pointer_cast<NativeData>(value);
		isMutable = _is_mutable;
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Object::Type::NativeData));
	}
	void readFrom(std::istream& is) override {}
};