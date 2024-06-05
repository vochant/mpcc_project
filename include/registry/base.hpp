#pragma once

// Mirekintoc's Prescaltum 2024.1 Standard Library
// PLEASE DO NOT REMOVE THIS PLUGIN
// Base Plugin - External Basic Language Features

#include "registry/base/plugin.hpp"
#include "object/integer.hpp"
#include "object/string.hpp"
#include "evaluator/evaluator.hpp"

#include "i18n/interface.hpp"

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

		// OOP External Resolution. For class inheritance.
		env->set("construct_next", std::make_shared<NativeFunction>(construct_next));

		// Dynamic Variable Management. Only for testing, plz use them as less as possible.
		env->set("dynamic_let", std::make_shared<NativeFunction>(dynamic_let));
		env->set("dynamic_remove", std::make_shared<NativeFunction>(dynamic_remove));
		env->set("dynamic_access", std::make_shared<NativeFunction>(dynamic_access));

		// Scope Controlling. Only for testing, plz use them as less as possible.
		env->set("evulate_up", std::make_shared<NativeFunction>(emulate_up));
		env->set("nisolated", std::make_shared<NativeFunction>(nisolated));

		// Language Features, recommended to use obj.method(args), not method(obj, args)
		// MPC is not for optimizing, plz choose other languages if you really need it.
		env->set("map", std::make_shared<NativeFunction>(Map));
		env->set("forEach", std::make_shared<NativeFunction>(ForEach));
		env->set("flat", std::make_shared<NativeFunction>(Flat));
		env->set("flatMap", std::make_shared<NativeFunction>(FlatMap));
		env->set("len", std::make_shared<NativeFunction>(Length));
		env->set("str", std::make_shared<NativeFunction>(Stringify));
		env->set("push", std::make_shared<NativeFunction>(Push));
		env->set("concat", std::make_shared<NativeFunction>(Concat));
		env->set("slice", std::make_shared<NativeFunction>(Slice));
		env->set("reverse", std::make_shared<NativeFunction>(Reverse));

	}
public:
	static NativeFunction::resulttype Reverse(NativeFunction::arglist args, Environment* env) {
		if (args.size() != 1) {
			return FormatError();
		}
		if (args[0]->type == Object::Type::Array) {
			auto ac = std::dynamic_pointer_cast<Array>(args[0]->copy());
			std::vector<std::shared_ptr<Object>> res;
			res.resize(ac->elements.size());
			std::reverse_copy(ac->elements.begin(), ac->elements.end(), res.begin());
			return OK(std::make_shared<Array>(res));
		}
		if (args[0]->type == Object::Type::String) {
			auto str = std::dynamic_pointer_cast<String>(args[0])->value;
			std::reverse(str.begin(), str.end());
			return OK(std::make_shared<String>(str));
		}
		return FormatError();
	}

	static NativeFunction::resulttype Slice(NativeFunction::arglist args, Environment* env) {
		if (args.size() != 3) {
			return FormatError();
		}
		if (args[1]->type != Object::Type::Integer || args[2]->type != Object::Type::Integer) {
			return FormatError();
		}
		auto s = std::dynamic_pointer_cast<Integer>(args[1])->value, d = std::dynamic_pointer_cast<Integer>(args[2])->value;
		if (args[0]->type == Object::Type::Array) {
			std::vector<std::shared_ptr<Object>> res;
			auto ac = std::dynamic_pointer_cast<Array>(args[0]);
			if (s + d > res.size()) d = res.size() - s;
			res.insert(res.end(), ac->elements.begin() + s, ac->elements.begin() + s + d);
			return OK(std::make_shared<Array>(res));
		}
		else if (args[0]->type == Object::Type::String) {
			return OK(std::make_shared<String>(std::dynamic_pointer_cast<String>(args[0])->value.substr(s, d)));
		}
		return FormatError();
	}

	static NativeFunction::resulttype Push(NativeFunction::arglist args, Environment* env) {
		if (args.size() < 1) {
			return FormatError();
		}
		if (args[0]->type != Object::Type::Array) {
			return FormatError();
		}
		auto _cas = std::dynamic_pointer_cast<Array>(args[0]);
		for (size_t i = 1; i < args.size(); i++) {
			_cas->elements.push_back(args[i]->copy());
		}
		return OK(_cas);
	}

	static NativeFunction::resulttype Concat(NativeFunction::arglist args, Environment* env) {
		if (args.size() < 1) {
			return FormatError();
		}
		if (args[0]->type == Object::Type::Array) {
			auto _cas = std::dynamic_pointer_cast<Array>(args[0]);
			for (size_t i = 1; i < args.size(); i++) {
				if (args[i]->type != Object::Type::Array) return FormatError();
			}
			for (size_t i = 1; i < args.size(); i++) {
				auto _cas2 = std::dynamic_pointer_cast<Array>(args[i]);
				_cas->elements.insert(_cas->elements.end(), _cas2->elements.begin(), _cas2->elements.end());
			}
			return OK(_cas);
		}
		if (args[0]->type == Object::Type::String) {
			auto _cas = std::dynamic_pointer_cast<String>(args[0]);
			for (size_t i = 1; i < args.size(); i++) {
				_cas->value += args[i]->toString();
			}
			return OK(_cas);
		}
		return FormatError();
	}

	static NativeFunction::resulttype Length(NativeFunction::arglist args, Environment* env) {
		if (args.size() != 1) {
			return FormatError();
		}
		if (args[0]->type == Object::Type::String) {
			return OK(std::make_shared<Integer>(std::dynamic_pointer_cast<String>(args[0])->value.length()));
		}
		if (args[0]->type == Object::Type::Array) {
			return OK(std::make_shared<Integer>(std::dynamic_pointer_cast<Array>(args[0])->elements.size()));
		}
		return FormatError();
	}

	static NativeFunction::resulttype Stringify(NativeFunction::arglist args, Environment* env) {
		if (args.size() != 1) {
			return FormatError();
		}
		return OK(std::make_shared<String>(args[0]->toString()));
	}

	static NativeFunction::resulttype Map(NativeFunction::arglist args, Environment* env) {
		if (args.size() != 2) {
			return FormatError();
		}
		if (args[0]->type != Object::Type::Array) {
			return FormatError();
		}
		if (args[1]->type != Object::Type::Function && args[1]->type != Object::Type::NativeFunction) {
			return FormatError();
		}
		auto ae = std::dynamic_pointer_cast<Array>(args[0]), as = std::make_shared<Array>();
		auto fcaller = [args, env](std::shared_ptr<Object> obj)->std::shared_ptr<Object> {
			auto fcast = std::dynamic_pointer_cast<Function>(args[1]);
			auto innerEnv = std::make_shared<Environment>(env);
			Evaluator::bind_args({obj}, fcast->args, fcast->moreName, innerEnv);
			return Object::toCommon(Evaluator::evaluate_scope(std::dynamic_pointer_cast<ScopeNode>(fcast->inner), innerEnv.get()));
		};
		auto ncaller = [args, env](std::shared_ptr<Object> obj)->std::shared_ptr<Object> {
			auto ncast = std::dynamic_pointer_cast<NativeFunction>(args[1]);
			auto[r, v] = ncast->_func({obj}, env);
			if (r == NativeFunction::Result::FORMAT_ERR) {
				args_size_error(i18n.lookup("error.call.nativeFormat"));
				return std::make_shared<Error>();
			}
			if (r == NativeFunction::Result::DATA_ERR) {
				arg_data_error();
				return std::make_shared<Error>();
			}
			if (r == NativeFunction::Result::UNHANDLED_ERR) {
				unhandled_error(i18n.lookup("error.call.nativeUnhandled"));
				return std::make_shared<Error>();
			}
			return v;
		};
		for (auto& i : ae->elements) {
			as->elements.push_back((args[1]->type == Object::Type::Function) ? fcaller(i) : ncaller(i));
		}
		return OK(as);
	}

	static NativeFunction::resulttype ForEach(NativeFunction::arglist args, Environment* env) {
		if (args.size() != 2) {
			return FormatError();
		}
		if (args[0]->type != Object::Type::Array) {
			return FormatError();
		}
		if (args[1]->type != Object::Type::Function && args[1]->type != Object::Type::NativeFunction) {
			return FormatError();
		}
		auto ae = std::dynamic_pointer_cast<Array>(args[0]);
		auto fcaller = [args, env](std::shared_ptr<Object> obj)->std::shared_ptr<Object> {
			auto fcast = std::dynamic_pointer_cast<Function>(args[1]);
			auto innerEnv = std::make_shared<Environment>(env);
			Evaluator::bind_args({obj}, fcast->args, fcast->moreName, innerEnv);
			return Object::toCommon(Evaluator::evaluate_scope(std::dynamic_pointer_cast<ScopeNode>(fcast->inner), innerEnv.get()));
		};
		auto ncaller = [args, env](std::shared_ptr<Object> obj)->std::shared_ptr<Object> {
			auto ncast = std::dynamic_pointer_cast<NativeFunction>(args[1]);
			auto[r, v] = ncast->_func({obj}, env);
			if (r == NativeFunction::Result::FORMAT_ERR) {
				args_size_error(i18n.lookup("error.call.nativeFormat"));
				return std::make_shared<Error>();
			}
			if (r == NativeFunction::Result::DATA_ERR) {
				arg_data_error();
				return std::make_shared<Error>();
			}
			if (r == NativeFunction::Result::UNHANDLED_ERR) {
				unhandled_error(i18n.lookup("error.call.nativeUnhandled"));
				return std::make_shared<Error>();
			}
			return v;
		};
		for (auto& i : ae->elements) {
			if (args[1]->type == Object::Type::Function) fcaller(i);
			else ncaller(i);
		}
		return OK(std::make_shared<Null>());
	}

	static NativeFunction::resulttype Flat(NativeFunction::arglist args, Environment* env) {
		if (args.size() < 1 || args.size() > 2) {
			return FormatError();
		}
		if (args[0]->type != Object::Type::Array) {
			return FormatError();
		}
		long long times = 1;
		if (args.size() == 2) {
			if (args[1]->type != Object::Type::Integer) {
				return FormatError();
			}
			times = std::dynamic_pointer_cast<Integer>(args[1])->value;
		}
		std::function<std::shared_ptr<Array>(std::shared_ptr<Array>, long long)> _flat;
		_flat = [_flat](std::shared_ptr<Array> objs, long long max_times)->std::shared_ptr<Array> {
			bool found_arr = false;
			auto res = std::make_shared<Array>();
			for (auto i : objs->elements) {
				if (i->type == Object::Type::Array) {
					auto cas = std::dynamic_pointer_cast<Array>(i);
					for (auto j : cas->elements) {
						if (j->type == Object::Type::Array) found_arr = true;
						res->elements.push_back(j);
					}
				}
				else res->elements.push_back(i);
			}
			if (found_arr && max_times) return _flat(res, max_times - 1);
			else return res;
		};
		return OK(_flat(std::dynamic_pointer_cast<Array>(args[0]), times));
	}

	static NativeFunction::resulttype FlatMap(NativeFunction::arglist args, Environment* env) {
		if (args.size() < 2 || args.size() > 3) {
			return FormatError();
		}
		if (args[0]->type != Object::Type::Array) {
			return FormatError();
		}
		if (args[1]->type != Object::Type::Function && args[1]->type != Object::Type::NativeFunction) {
			return FormatError();
		}
		if (args.size() == 3 && args[2]->type != Object::Type::Integer) {
			return FormatError();
		}
		auto _Map = Map({args[0], args[1]}, env);
		if (_Map.first != NativeFunction::Result::OK) return _Map;
		if (args.size() == 3) return Flat({_Map.second, args[2]}, env);
		else return Flat({_Map.second}, env);
	}
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
			return UnhandledError();
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
		if (_f->inner->type != Node::Type::Scope) {
			return std::make_pair(NativeFunction::Result::DATA_ERR, std::make_shared<Null>());
		}
		auto res = Object::toCommon(Evaluator::evaluate_scope(std::dynamic_pointer_cast<ScopeNode>(_f->inner), innerEnv.get()));
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
						return i18n.lookup("error.call.tooLessArgs", {{"[Expected]", std::to_string(_fc->args.size())}, {"[Current]", std::to_string(arguments.size())}});
					}
					else {
						return i18n.lookup("error.call.incorrectArgs", {{"[Expected]", std::to_string(_fc->args.size())}, {"[Current]", std::to_string(arguments.size())}});
					}
				}());
				return std::make_pair(NativeFunction::Result::DATA_ERR, std::make_shared<Null>());
			}
			auto innerEnv = std::make_shared<Environment>(ptr);
			Evaluator::bind_args(arguments, _fc->args, _fc->moreName, innerEnv);
			if (_fc->inner->type != Node::Type::Scope) {
				node_type_error();
				return std::make_pair(NativeFunction::Result::UNHANDLED_ERR, std::make_shared<Null>());
			}
			return std::make_pair(NativeFunction::Result::OK, Object::toCommon(Evaluator::evaluate_scope(std::dynamic_pointer_cast<ScopeNode>(_fc->inner), innerEnv.get())));
		}
		if (args[1]->type == Object::Type::NativeFunction) {
			auto _func = std::dynamic_pointer_cast<NativeFunction>(args[1]);
            auto _res = _func->_func(arguments, ptr);
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
					return i18n.lookup("error.call.tooLessArgs", {{"[Expected]", std::to_string(_fc->args.size())}, {"[Current]", std::to_string(arguments.size())}});
				}
				else {
					return i18n.lookup("error.call.incorrectArgs", {{"[Expected]", std::to_string(_fc->args.size())}, {"[Current]", std::to_string(arguments.size())}});
				}
			}());
			return std::make_pair(NativeFunction::Result::DATA_ERR, std::make_shared<Null>());
		}
		Evaluator::bind_args(arguments, _fc->args, _fc->moreName, env);
		if (_fc->inner->type != Node::Type::Scope) {
			node_type_error();
			return std::make_pair(NativeFunction::Result::UNHANDLED_ERR, std::make_shared<Null>());
		}
		return std::make_pair(NativeFunction::Result::OK, Object::toCommon(Evaluator::evaluate_scope(std::dynamic_pointer_cast<ScopeNode>(_fc->inner), env)));
	}	
};