#pragma once

#include "ast/base/node.hpp"
#include "object/base.hpp"
#include "object/environment.hpp"
#include "program/vm_options.hpp"

class Class : public Object {
public:
	std::shared_ptr<Environment> inner, outer;
	unsigned long long _signature;
	Class(std::shared_ptr<Environment> outer) : outer(outer), Object(Object::Type::Class), inner(std::make_shared<Environment>(outer)), _signature(_cert++) {}
public:
	std::string toString() const override {
		return "Class:" + inner->toString();
	}

	std::shared_ptr<Object> copy() const override {
		std::shared_ptr<Class> result = std::make_shared<Class>();
		result->inner = std::dynamic_pointer_cast<Environment>(inner->copy());
		return result;
	}

	std::string idstr() const override {
		return "class_" + std::to_string(_signature);
	}

	void assign(std::shared_ptr<Object> obj) override {
		auto _cast = std::dynamic_pointer_cast<Class>(obj);
		inner = std::dynamic_pointer_cast<Environment>(_cast->inner->copy());
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Object::Type::Class));
	}

	void readFrom(std::istream& is) override {}
};