#pragma once

// Mirekintoc's Prescaltum 2024.1 Standard Library
// PLEASE DO NOT REMOVE THIS PLUGIN
// Base Plugin - External Basic Language Features

#include "registry/base/plugin.hpp"
#include "object/integer.hpp"
#include "object/string.hpp"
#include "evaluator/evaluator.hpp"

/**
 * @brief MPC Base Language Features, most of MPC source codes and OSL (Offical Standard Library) requires it to run.
 * @class
 */
class BasePlugin : public Plugin {
public:
	/**
	 * @brief Construct a new Base Plugin object
	 */
	BasePlugin() : Plugin() {}

	/**
	 * @brief Attach to an environment
	 * 
	 * @param env The environment to attach
	 */
	void attach(std::shared_ptr<Environment> env) const override {
		env->set("construct_next", std::make_shared<NativeFunction>(construct_next));
		env->set("dynamic_let", std::make_shared<NativeFunction>(dynamic_let));
		env->set("dynamic_remove", std::make_shared<NativeFunction>(dynamic_remove));
		env->set("dynamic_access", std::make_shared<NativeFunction>(dynamic_access));
		env->set("emulate_up", std::make_shared<NativeFunction>(emulate_up));
		env->set("nisolated", std::make_shared<NativeFunction>(nisolated));
	}
public:
	/**
	 * @brief Constuct parent class
	 * 
	 * @param args NativeFunction arguments 
	 * @param env Environment
	 * @return NativeFunction::resulttype (Status & Return-Value)
	 */
	static NativeFunction::resulttype construct_next(NativeFunction::arglist args, Environment* env) {
		auto _construct_level = env->get("__construct__");
		if (_construct_level->type != Object::Type::Integer) {
			return std::make_pair(NativeFunction::Result::UNHANDLED_ERR, std::make_shared<Null>());
		}
		long long _clevel = std::dynamic_pointer_cast<Integer>(_construct_level)->value;
		if (_clevel == 0) {
			return std::make_pair(NativeFunction::Result::DATA_ERR, std::make_shared<Null>());
		}
		auto _cr = env->get("__spec_constructor" + std::to_string(_clevel - 1) + "_" + std::to_string(args.size()));
		if (_cr->type != Object::Type::Function) {
			return std::make_pair(NativeFunction::Result::DATA_ERR, std::make_shared<Null>());
		}
		auto _f = std::dynamic_pointer_cast<Function>(_cr);
		if (_f->isLambda) {
			return std::make_pair(NativeFunction::Result::DATA_ERR, std::make_shared<Null>());
		}
		auto innerEnv = std::make_shared<Environment>(env);
		Evaluator::bind_args(args, _f->args, _f->moreName, innerEnv);
		if (_f->inner->type != Node::Type::Region) {
			return std::make_pair(NativeFunction::Result::DATA_ERR, std::make_shared<Null>());
		}
		auto res = Object::toCommon(Evaluator::evaluate_region(std::dynamic_pointer_cast<RegionNode>(_f->inner), innerEnv));
		env->get("__construct__")->assign(std::make_shared<Integer>(_clevel - 1));
		return std::make_pair(NativeFunction::Result::OK, res);
	}

	/**
	 * @brief Create or Redefine a variable, the name is from a String.
	 * 
	 * @param args NativeFunction arguments
	 * @param env Environment
	 * @return NativeFunction::resulttype (Status & Return-Value)
	 */
	static NativeFunction::resulttype dynamic_let(NativeFunction::arglist args, Environment* env) {
		if (args.size() != 2) {
			return std::make_pair(NativeFunction::Result::FORMAT_ERR, std::make_shared<Null>());
		}
		if (args[0]->type != Object::Type::String) {
			return std::make_pair(NativeFunction::Result::DATA_ERR, std::make_shared<Null>());
		}
		env->set(std::dynamic_pointer_cast<String>(args[0])->value, args[1]->copy());
		return std::make_pair(NativeFunction::Result::OK, std::make_shared<Null>());
	}

	/**
	 * @brief Remove a variable, the name is from a String.
	 * 
	 * @param args NativeFunction arguments
	 * @param env Environment
	 * @return NativeFunction::resulttype (Status & Return-Value)
	 */
	static NativeFunction::resulttype dynamic_remove(NativeFunction::arglist args, Environment* env) {
		if (args.size() != 1) {
			return std::make_pair(NativeFunction::Result::FORMAT_ERR, std::make_shared<Null>());
		}
		if (args[0]->type != Object::Type::String) {
			return std::make_pair(NativeFunction::Result::DATA_ERR, std::make_shared<Null>());
		}
		auto str = std::dynamic_pointer_cast<String>(args[0])->value;
		if (env->get(str)->type == Object::Type::Null) {
			return std::make_pair(NativeFunction::Result::DATA_ERR, std::make_shared<Null>());
		}
		env->remove(str);
		return std::make_pair(NativeFunction::Result::OK, std::make_shared<Null>());
	}

	/**
	 * @brief Access a variable, the name is from a String.
	 * 
	 * @param args NativeFunction arguments.
	 * @param env Environment
	 * @return NativeFunction::resulttype (Status & Return-Value)
	 */
	static NativeFunction::resulttype dynamic_access(NativeFunction::arglist args, Environment* env) {
		if (args.size() != 1) {
			return std::make_pair(NativeFunction::Result::FORMAT_ERR, std::make_shared<Null>());
		}
		if (args[0]->type != Object::Type::String) {
			return std::make_pair(NativeFunction::Result::DATA_ERR, std::make_shared<Null>());
		}
		auto str = std::dynamic_pointer_cast<String>(args[0])->value;
		if (env->get(str)->type == Object::Type::Null) {
			return std::make_pair(NativeFunction::Result::DATA_ERR, std::make_shared<Null>());
		}
		return std::make_pair(NativeFunction::Result::OK, env->get(str));
	}

	/**
	 * @brief Run a Function/NativeFunction in parent environment.
	 * 
	 * @param args NativeFunction arguments.
	 * @param env Environment
	 * @return NativeFunction::resulttype (Status & Return-Value)
	 */
	static NativeFunction::resulttype emulate_up(NativeFunction::arglist args, Environment* env) {
		if (args.size() < 2) {
			return std::make_pair(NativeFunction::Result::FORMAT_ERR, std::make_shared<Null>());
		}
		if (args[0]->type != Object::Type::Integer) {
			return std::make_pair(NativeFunction::Result::DATA_ERR, std::make_shared<Null>());
		}
		long long level = std::dynamic_pointer_cast<Integer>(args[0])->value;
		auto ptr = env;
		for (long long i = 0; i != level; i++) {
			if (ptr->_parent) ptr = ptr->_parent;
			else break;
		}
		std::vector<std::shared_ptr<Object>> arguments(args.begin() + 2, args.end());
		if (args[1]->type == Object::Type::Function) {
			auto _fc = std::dynamic_pointer_cast<Function>(args[1]);
			if (_fc->args.size() > arguments.size() && (_fc->hasMore() || _fc->args.size() == arguments.size())) {
				args_size_error([_fc, arguments]()->std::string {
					if (_fc->hasMore()) {
						return "Required at least " + std::to_string(_fc->args.size()) + " args, but found only " + std::to_string(arguments.size()) + " args.";
					}
					else {
						return "Required " + std::to_string(_fc->args.size()) + " args, but found only " + std::to_string(arguments.size()) + " args.";
					}
				}());
				return std::make_pair(NativeFunction::Result::DATA_ERR, std::make_shared<Null>());
			}
			auto innerEnv = std::make_shared<Environment>(ptr);
			Evaluator::bind_args(arguments, _fc->args, _fc->moreName, innerEnv);
			if (_fc->inner->type != Node::Type::Region) {
				node_type_error();
				return std::make_pair(NativeFunction::Result::UNHANDLED_ERR, std::make_shared<Null>());
			}
			return std::make_pair(NativeFunction::Result::OK, Object::toCommon(Evaluator::evaluate_region(std::dynamic_pointer_cast<RegionNode>(_fc->inner), innerEnv)));
		}
		if (args[1]->type == Object::Type::NativeFunction) {
			auto _func = std::dynamic_pointer_cast<NativeFunction>(args[1]);
            auto _res = _func->func(arguments, ptr);
            return _res;
		}
	}

	/**
	 * @brief Run a Function without isolate.
	 * 
	 * @param args NativeFunction arguments.
	 * @param env Environment
	 * @return NativeFunction::resulttype (Status & Return-Value)
	 */
	static NativeFunction::resulttype nisolated(NativeFunction::arglist args, Environment* env) {
		if (args.size() < 1) {
			return std::make_pair(NativeFunction::Result::FORMAT_ERR, std::make_shared<Null>());
		}
		if (args[0]->type != Object::Type::Function) {
			return std::make_pair(NativeFunction::Result::DATA_ERR, std::make_shared<Null>());
		}
		auto _fc = std::dynamic_pointer_cast<Function>(args[1]);
		std::vector<std::shared_ptr<Object>> arguments(args.begin() + 1, args.end());
		if (_fc->args.size() > arguments.size() && (_fc->hasMore() || _fc->args.size() == arguments.size())) {
			args_size_error([_fc, arguments]()->std::string {
				if (_fc->hasMore()) {
					return "Required at least " + std::to_string(_fc->args.size()) + " args, but found only " + std::to_string(arguments.size()) + " args.";
				}
				else {
					return "Required " + std::to_string(_fc->args.size()) + " args, but found only " + std::to_string(arguments.size()) + " args.";
				}
			}());
			return std::make_pair(NativeFunction::Result::DATA_ERR, std::make_shared<Null>());
		}
		Evaluator::bind_args(arguments, _fc->args, _fc->moreName, env);
		if (_fc->inner->type != Node::Type::Region) {
			node_type_error();
			return std::make_pair(NativeFunction::Result::UNHANDLED_ERR, std::make_shared<Null>());
		}
		return std::make_pair(NativeFunction::Result::OK, Object::toCommon(Evaluator::evaluate_region(std::dynamic_pointer_cast<RegionNode>(_fc->inner), env)));
	}	
};