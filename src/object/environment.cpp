#pragma once

#include "object/environment.hpp"
#include "object/function.hpp"

std::shared_ptr<Object> Environment::get(std::string _name) {
	if (_name == "this") {
		if (_this) return *_this;
		if (_parent) return _parent->get("this");
		return std::make_shared<Null>();
	}
	auto it = values.find(_name);
	if (it != values.end()) {
		auto res = it->second.value;
		if (res->type == Object::Type::Function) {
			std::dynamic_pointer_cast<Function>(res)->outerEnv = this;
		}
	}
	else if (_parent) return _parent->get(_name);
	else return std::make_shared<Null>();
}