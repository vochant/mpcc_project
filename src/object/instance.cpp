#pragma once

#include "object/instance.hpp"
#include "evaluator/evaluator.hpp"

Instance::~Instance() {
	auto lvl = inner->getThere("__level__");
	if (lvl->type != Object::Type::Integer) {
		lvl_type_error();
		return;
	}
	long long _l = std::dynamic_pointer_cast<Integer>(lvl)->value;
	for (long long i = _l; i >= 0; i--) {
		auto des = inner->getThere("__spec_destructor" + std::to_string(i));
		if (des->type != Object::Type::Null) {
			if (des->type != Object::Type::Function) {
				data_type_error("Destructors must be a function.");
				return;
			}
			auto _func = std::dynamic_pointer_cast<Function>(des);
			if (_func->isLambda) {
				data_value_error();
				return;
			}
			if (_func->args.size() > 0 || _func->hasMore()) {
				args_size_error("Destructors must have no args and cannot use More-Argument.");
				return;
			}
			auto innerEnv = std::make_shared<Environment>(inner);
			if (_func->inner->type != Node::Type::Scope) {
				node_type_error();
            	return;
			}
			Evaluator::evaluate_scope(std::dynamic_pointer_cast<ScopeNode>(_func->inner), innerEnv.get());
		}
	}
}