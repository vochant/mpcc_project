#pragma once

#include "evaluator/evaluator.hpp"
#include <cmath>

/**
 * @brief Evaluate a Program.
 * 
 * @param _prog ProgramNode, contains a RegionNode.
 * @param env Environment
 * @return std::shared_ptr<Object> Result
 */
std::shared_ptr<Object> Evaluator::evaluate_program(std::shared_ptr<ProgramNode> _prog, Environment* env) {
    auto _inner = _prog->mainRegion;
    if (_inner->type != Node::Type::Region) {
        program_error();
        return make_error();
    }
    auto _cast = std::dynamic_pointer_cast<RegionNode>(_inner);
    return evaluate_region(_cast, env);
}

/**
 * @brief Evaluate a Constant.
 * 
 * @param _constant ConstantNode, contains an Object.
 * @return std::shared_ptr<Object> Result
 */
std::shared_ptr<Object> Evaluator::evaluate_constant(std::shared_ptr<ConstantNode> _constant) {
    auto obj = _constant->obj;
    obj->markMutable(false);
}

/**
 * @brief Evaluate a RegionNode
 * 
 * @param _region RegionNode, contains statement(s).
 * @param env Environment
 * @param no_isolate Isolate or not.
 * @return std::shared_ptr<Object> Result
 */
std::shared_ptr<Object> Evaluator::evaluate_region(std::shared_ptr<RegionNode> _region, Environment* env, bool no_isolate = true) {
	std::shared_ptr<Environment> own;
	Environment* to_use;
	if (no_isolate) to_use = env;
	else {
		own = std::make_shared<Environment>(env);
		to_use = own.get();
	}
    std::shared_ptr<Object> res = std::make_shared<Null>();
    for (auto i : _region->statements) {
        res = evaluate_one(i, to_use);
        if (res->isReturn || res->type == Object::Type::Error) {
            return Object::toConstant(res->copy());
        }
    }
    return Object::toConstant(res->copy());
}

std::shared_ptr<Object> Evaluator::evaluate_function(std::shared_ptr<FunctionNode> _func, Environment* env) {
    return std::make_shared<Function>(_func, env);
}

std::shared_ptr<Object> Evaluator::evaluate_call(std::shared_ptr<CallNode> _call, Environment* env) {
    auto body = evaluate_value(_call->to_run, env);
    std::vector<std::shared_ptr<Object>> args;
    for (auto i : _call->args) {
        args.push_back(evaluate_value(i, env));
    }
    if (_call->more) {
        auto _more = evaluate_value(_call->more, env);
        if (_more->type != Object::Type::Array) {
            more_type_error("Expected Array, got " + _more->typeOf() + ".");
            return make_error();
        }
        auto _moreCast = std::dynamic_pointer_cast<Array>(_more);
        for (auto i : _moreCast->elements) {
            args.push_back(i);
        }
    }
    if (body->type == Object::Type::Function) {
        auto _funcCast = std::dynamic_pointer_cast<Function>(body);
        if (_funcCast->args.size() > args.size() && (_funcCast->hasMore() || _funcCast->args.size() == args.size())) {
            args_size_error([_funcCast, args]()->std::string {
				if (_funcCast->hasMore()) {
					return "Required at least " + std::to_string(_funcCast->args.size()) + " args, but found only " + std::to_string(args.size()) + " args.";
				}
				else {
					return "Required " + std::to_string(_funcCast->args.size()) + " args, but found only " + std::to_string(args.size()) + " args.";
				}
			}());
            return make_error();
        }
        auto innerEnv = std::make_shared<Environment>(_funcCast->isLambda ? env : _funcCast->outerEnv);
        bind_args(args, _funcCast->args, _funcCast->moreName, innerEnv);
        if (_funcCast->inner->type != Node::Type::Region) {
            node_type_error();
            return make_error();
        }
        return Object::toCommon(evaluate_region(std::dynamic_pointer_cast<RegionNode>(_funcCast->inner), innerEnv.get()));
    }
    else if (body->type == Object::Type::Class) {
        auto _classCast = std::dynamic_pointer_cast<Class>(body);
        auto _inst = std::make_shared<Instance>(_classCast);
		_inst->inner->set("__construct__", _inst->inner->get("__level__")->copy());
        auto _cobj = _inst->inner->getThere("__spec_constructor" + _inst->inner->getThere("__level__")->toString() + "_" + std::to_string(args.size()));
        if (_cobj->type == Object::Type::Null) {
            invalid_constructor_error();
            return make_error();
        }
        if (_cobj->type != Object::Type::Function) {
            data_type_error("Constructors must be a function.");
            return make_error();
        }
        auto _func = std::dynamic_pointer_cast<Function>(_cobj);
        if (_func->isLambda) {
            data_value_error();
            return make_error();
        }
        auto _innerEnv = std::make_shared<Environment>(_inst->inner);
        bind_args(args, _func->args, _func->moreName, _innerEnv);
        if (_func->inner->type != Node::Type::Region) {
            node_type_error();
            return make_error();
        }
        _inst->inner->globalLock = true;
        evaluate_region(std::dynamic_pointer_cast<RegionNode>(_func->inner), _innerEnv.get());
		_inst->inner->_this = &std::dynamic_pointer_cast<Object>(_inst);
        return _inst;
    }
    else if (body->type == Object::Type::Instance) {
        auto _instCast = std::make_shared<Instance>(body);
        auto _cobj = _instCast->inner->getThere("operator()");
        if (_cobj->type == Object::Type::Null) {
            not_found_error("[Instance]:operator()");
            return make_error();
        }
        if (_cobj->type == Object::Type::Function) {
            auto _func = std::dynamic_pointer_cast<Function>(_cobj);
            if (_func->args.size() > args.size() && (_func->hasMore() || _func->args.size() == args.size())) {
                args_size_error([_func, args]()->std::string {
				if (_func->hasMore()) {
					return "Required at least " + std::to_string(_func->args.size()) + " args, but found only " + std::to_string(args.size()) + " args.";
				}
				else {
					return "Required " + std::to_string(_func->args.size()) + " args, but found only " + std::to_string(args.size()) + " args.";
				}
			}());
                return make_error();
            }
            if (_func->isLambda) {
                data_value_error();
                return make_error();
            }
            auto _innerEnv = std::make_shared<Environment>(_instCast->inner);
            bind_args(args, _func->args, _func->moreName, _innerEnv);
            if (_func->inner->type != Node::Type::Region) {
                unhandled_error("Function->inner should be RegionNode, but found somewhere not.");
                return make_error();
            }
            return Object::toCommon(evaluate_region(std::dynamic_pointer_cast<RegionNode>(_func->inner), _innerEnv.get()));
        }
        else if (_cobj->type == Object::Type::NativeFunction) {
            auto _func = std::dynamic_pointer_cast<NativeFunction>(_cobj);
            auto[_sta, _rval] = _func->_func(args, env);
            if (_sta == NativeFunction::Result::FORMAT_ERR) {
                args_size_error("NativeFunction reported FORMAT_ERROR.");
                return make_error();
            }
            else if (_sta == NativeFunction::Result::DATA_ERR) {
                arg_data_error();
                return make_error();
            }
            else if (_sta == NativeFunction::Result::UNHANDLED_ERR) {
                unhandled_error("NativeFunction reported UNHANDLED_ERROR.");
                return make_error();
            }
            else {
                return _rval;
            }
        }
		else {
			data_type_error("Not a function/native-function - Instance->inner->getThere(\"operator()\").");
			return make_error();
		}
    }
    else if (body->type == Object::Type::NativeFunction) {
        auto _funcCast = std::dynamic_pointer_cast<NativeFunction>(body);
        auto[_sta, _rval] = _funcCast->_func(args, env);
        if (_sta == NativeFunction::Result::FORMAT_ERR) {
            args_size_error("NativeFunction reported FORMAT_ERROR");
            return make_error();
        }
        else if (_sta == NativeFunction::Result::DATA_ERR) {
            arg_data_error();
            return make_error();
        }
        else if (_sta == NativeFunction::Result::UNHANDLED_ERR) {
            unhandled_error("NativeFunction reported UNHANDLED_ERROR.");
            return make_error();
        }
        else {
            return _rval;
        }
    }
    else {
        data_type_error("Not a function/native-function");
        return make_error();
    }
}

std::shared_ptr<Object> Evaluator::evaluate_array(std::shared_ptr<ArrayNode> _arr, Environment* env) {
    auto arr = std::make_shared<Array>();
    for (auto i : _arr->elements) {
        arr->elements.push_back(evaluate_value(i, env));
    }
	arr->markMutable(false);
    return arr;
}

std::shared_ptr<Object> Evaluator::evaluate_assign(std::shared_ptr<AssignNode> _assign, Environment* env) {
    auto w = evaluate_value(_assign->left, env);
    if (!w->isMutable) {
        not_mutable_error();
        return make_error();
    }
    auto v = evaluate_value(_assign->right, env);
	std::shared_ptr<Object> val;
    if (_assign->_op != "=") {
        auto bef = _assign->_op.substr(0, _assign->_op.length() - 1);
        val = calcuate_infix(w, v, bef, env);
    }
	else val = v;
	if (w->type == Object::Type::Null) {
		if (_assign->left->type != Node::Type::Identifier) {
			err_begin();
			std::cout << "Unexpected re-definition for NULL.";
			err_end();
			return make_error();
		}
		auto id = std::dynamic_pointer_cast<IdentifierNode>(_assign->left)->id;
		while (env->values.count(id) == 0) {
			env = env->_parent;
		}
		env->set(id, v);
		return Object::toConstant(v);
	}
    if (w->type != v->type) {
        type_different_error(w->typeOf(), v->typeOf());
        return make_error();
    }
	if (w->type == Object::Type::Instance) {
		if (!check_class_relationship(std::dynamic_pointer_cast<Instance>(w)->inner->get("__indexes__"), std::dynamic_pointer_cast<Instance>(v)->inner->get("__indexes__"))) {
			type_different_error(w->typeOf(), v->typeOf());
        	return make_error();
		}
	}
    w->assign(v);
	return Object::toConstant(w->copy());
}

std::shared_ptr<Object> Evaluator::evaluate_class(std::shared_ptr<ClassNode> _class, Environment* env) {
    std::shared_ptr<Class> cls;
    if (_class->_ext != ":") {
        auto ext = env->get(_class->_ext);
        if (ext->type != Object::Type::Class) {
            extand_nothing_error(_class->_ext);
            return make_error();
        }
        cls = std::dynamic_pointer_cast<Class>(ext->copy());
        auto _lvl = cls->inner->get("__level__");
        if (_lvl->type != Object::Type::Integer) {
            lvl_type_error();
            return make_error();
        }
        _lvl->assign(std::make_shared<Integer>(std::dynamic_pointer_cast<Integer>(_lvl)->value + 1));
		auto _ids = cls->inner->get("__indexes__");
		if (_ids->type != Object::Type::Array) {
			lvl_type_error();
			return make_error();
		}
		std::dynamic_pointer_cast<Array>(_ids)->elements.push_back(std::make_shared<Integer>(cls->_signature));
    }
    else {
        cls = std::make_shared<Class>(env);
        cls->inner->set("__level__", std::make_shared<Integer>(0));
		auto _ids = std::make_shared<Array>();
		_ids->elements.push_back(std::make_shared<Integer>(cls->_signature));
		cls->inner->set("__indexes__", _ids);
    }
    if (_class->inner->type != Node::Type::Region) {
        unhandled_error("Function->inner should be RegionNode, but found somewhere not.");
        return make_error();
    }
    evaluate_region(std::dynamic_pointer_cast<RegionNode>(_class->inner), cls->inner.get());
    return cls;
}

std::shared_ptr<Object> Evaluator::evaluate_if(std::shared_ptr<IfNode> _if, Environment* env) {
    auto _cond = evaluate_value(_if->_cond, env);
    std::shared_ptr<Object> _res;
    if (isTrue(_cond)) {
        auto innerEnv = std::make_shared<Environment>(env);
        _res = evaluate_one(_if->_then, innerEnv.get());
    }
    else if (_if->_else) {
        auto innerEnv = std::make_shared<Environment>(env);
        _res = evaluate_one(_if->_else, innerEnv.get());
    }
    if (_res->isReturn) {
        return _res;
    }
    return std::shared_ptr<Null>();
}

std::shared_ptr<Object> Evaluator::evaluate_creation(std::shared_ptr<CreationNode> _cr, Environment* env) {
	if (_cr->isGlobal) {
		while (env->_parent) env = env->_parent;
	}
    for (auto i : _cr->creations) {
        auto v = evaluate_value(i.second, env);
        std::string _name = i.first;
        if (i.first == "__spec_constructor" || i.first == "__spec_destructor") {
            if (v->type != Object::Type::Function) {
                conde_type_error();
                return make_error();
            }
            auto _lvl = env->get("__level__");
            if (_lvl->type != Object::Type::Integer) {
                lvl_type_error();
                return make_error();
            }
            _name += _lvl->toString();
        }
        if (i.first == "__spec_constructor") {
            _name += "_" + std::to_string(std::dynamic_pointer_cast<Function>(v)->args.size());
        }
        if (env->getThere(_name)) {
            if (_cr->allowOverwrite) {
                env->remove(_name);
            }
            else {
                already_valid_error(_name);
                return make_error();
            }
        }
        env->create(_name, v);
        if (_cr->isConst) {
            env->get(_name)->markMutable(false);
        }
        if (_cr->isPrivate) {
            env->setAT(_name, Item::AccessToken::Private);
        }
    }
    return std::make_shared<Null>();
}

std::shared_ptr<Object> Evaluator::evaluate_enumerate(std::shared_ptr<EnumerateNode> _enum, Environment* env) {
    std::vector<std::pair<std::string, long long>> v;
    long long _idx = 0;
    for (auto i : _enum->items) {
        v.push_back(std::make_pair(i, _idx++));
    }
    auto e = std::make_shared<Enumerate>(v);
	e->markMutable(false);
    return e;
}

std::shared_ptr<Object> Evaluator::evaluate_expr(std::shared_ptr<ExprNode> _expr, Environment* env) {
    return evaluate_value(_expr->inner, env);
}

std::shared_ptr<Object> Evaluator::evaluate_for(std::shared_ptr<ForNode> _for, Environment* env) {
    std::shared_ptr<Object> _res = std::make_shared<Null>();
    auto innerEnv = std::make_shared<Environment>(env);
    auto elems = evaluate_value(_for->_elem, env);
    if (elems->type != Object::Type::Array) {
        for_elem_error(elems->typeOf());
        return make_error();
    }
    auto arr = std::dynamic_pointer_cast<Array>(elems);
    for (auto i : arr->elements) {
        innerEnv->set(_for->_var->toString(), i);
        _res = evaluate_one(_for->_body, innerEnv.get());
        if (_res->isReturn) {
            return _res;
        }
    }
    return std::make_shared<Null>();
}

std::shared_ptr<Object> Evaluator::evaluate_id(std::shared_ptr<IdentifierNode> _id, Environment* env) {
    return env->get(_id->toString());
}

std::shared_ptr<Object> Evaluator::evaluate_indecrement(std::shared_ptr<InDecrementNode> _idc, Environment* env) {
	auto v = evaluate_value(_idc->body, env);
	if (v->type == Object::Type::Instance) {
		std::string _fname = _idc->isAfter ? (_idc->isDecrement ? "operator--" : "operator++") : (_idc->isDecrement ? "prefix--" : "prefix++");
        auto _func = std::dynamic_pointer_cast<Instance>(v)->inner->getThere(_fname);
        if (_func->type == Object::Type::Function) {
            auto _fc = std::dynamic_pointer_cast<Function>(_func);
            if (_fc->args.size() > 0 || _fc->hasMore()) {
                args_size_error("Increment/Decrement functions must not have args.");
                return make_error();
            }
            auto innerEnv = std::make_shared<Environment>(_fc->isLambda ? env : _fc->outerEnv);
            if (_fc->inner->type != Node::Type::Region) {
                unhandled_error("Function->inner should be RegionNode, but found somewhere not.");
                return make_error();
            }
            return Object::toCommon(evaluate_region(std::dynamic_pointer_cast<RegionNode>(_fc->inner), innerEnv.get()));
        }
        else if (_func->type == Object::Type::NativeFunction) {
            auto _fc = std::dynamic_pointer_cast<NativeFunction>(_func);
            auto res = _fc->_func(NativeFunction::arglist(), env);
            if (res.first == NativeFunction::Result::FORMAT_ERR) {
                args_size_error("Increment/Decrement function(native) reported FORMAT_ERROR.");
                return make_error();
            }
            if (res.first == NativeFunction::Result::DATA_ERR) {
                arg_data_error();
                return make_error();
            }
            if (res.first == NativeFunction::Result::UNHANDLED_ERR) {
                unhandled_error("NativeFunction reported UNHANDLED_ERROR.");
                return make_error();
            }
            return res.second;
        }
        else {
            data_type_error("Not a function/native-function - Instance->inner->getThere(\"" + _fname + "\").");
            return make_error();
        }
		
	}
    std::shared_ptr<Object> _copy;
	if (_idc->isAfter) {
		_copy = v->copy();
		_copy->markMutable(false);
	}
	if (v->type == Object::Type::Integer) {
		std::dynamic_pointer_cast<Integer>(v)->value += _idc->isDecrement ? -1 : 1;
	}
	else if (v->type == Object::Type::Float) {
		std::dynamic_pointer_cast<Float>(v)->value += _idc->isDecrement ? -1.0 : 1.0;
	}
	else {
		idc_error(v->typeOf());
        return make_error();
	}
    if (_idc->isAfter) {
        return _copy;
    }
    else {
        return Object::toConstant(v->copy());
    }
}

std::shared_ptr<Object> Evaluator::evaluate_index(std::shared_ptr<IndexNode> _idx, Environment* env) {
	auto left = evaluate_value(_idx->left, env), index = evaluate_value(_idx->index, env);
	if (left->type == Object::Type::Instance) {
		auto func = std::dynamic_pointer_cast<Instance>(left)->inner->getThere("operator[]");
		if (func->type == Object::Type::Function) {
			auto _fc = std::dynamic_pointer_cast<Function>(func);
			if (_fc->args.size() != 1 || _fc->hasMore()) {
				args_size_error("Index functions can contain 1 arg only and cannot use MoreArgument.");
				return make_error();
			}
			auto innerEnv = std::make_shared<Environment>(env);
			innerEnv->set(_fc->args[0], index);
			innerEnv->get(_fc->args[0])->markMutable(true);
			if (_fc->inner->type != Node::Type::Region) {
				unhandled_error("Function->inner should be RegionNode, but found somewhere not.");
				return make_error();
			}
			return Object::toCommon(evaluate_region(std::dynamic_pointer_cast<RegionNode>(_fc->inner), innerEnv.get()));
		}
		if (func->type == Object::Type::NativeFunction) {
			auto _fc = std::dynamic_pointer_cast<NativeFunction>(func);
            auto res = _fc->_func(NativeFunction::arglist{index}, env);
            if (res.first == NativeFunction::Result::FORMAT_ERR) {
                args_size_error("Index function(native) reported FORMAT_ERROR.");
                return make_error();
            }
            if (res.first == NativeFunction::Result::DATA_ERR) {
                arg_data_error();
                return make_error();
            }
            if (res.first == NativeFunction::Result::UNHANDLED_ERR) {
                unhandled_error("NativeFunction reported UNHANDLED_ERROR.");
                return make_error();
            }
            return res.second;
		}
		data_type_error("Not a function/native-function - Instance->inner->getThere(\"operator()\").");
		return make_error();
	}
	if (left->type == Object::Type::String) {
		if (index->type == Object::Type::Integer) {
			auto _idx = std::dynamic_pointer_cast<Integer>(index);
			auto _val = std::dynamic_pointer_cast<String>(left);
			if (_idx->value >= _val->value.length() || _idx->value < 0) {
				out_of_range_error("Index Statement: String: Expected [0, " + std::to_string(_val->value.length() - 1) + "], got " + std::to_string(_idx->value) + ".");
				return make_error();
			}
			return std::make_shared<String>(_val->value[_idx->value]);
		}
		if (index->type == Object::Type::Array) {
			auto _idx = std::dynamic_pointer_cast<Array>(index);
			auto _val = std::dynamic_pointer_cast<String>(left);
			std::string result;
			for (size_t i = 0; i + 1 < _idx->elements.size(); i += 2) {
				if (_idx->elements[i]->type != Object::Type::Integer || _idx->elements[i + 1]->type != Object::Type::Integer) {
					data_type_error("Indexes must be Integer or Array of Integer.");
					return make_error();
				}
				auto _beg = std::dynamic_pointer_cast<Integer>(_idx->elements[i])->value, _end = std::dynamic_pointer_cast<Integer>(_idx->elements[i + 1])->value;
				if (_end < _beg) {
					out_of_range_error("Index Statement: String: Negative Width: End(" + std::to_string(_end) + ") < Begin(" + std::to_string(_beg) + ").");
					return make_error();
				}
				if (_end > _val->value.length() || _beg < 0) {
					out_of_range_error("Index Statement: String: Expected [0, " + std::to_string(_val->value.length() - 1) + "], got [" + std::to_string(_beg) + ", " + std::to_string(_end) + "].");
					return make_error();
				}
				result += _val->value.substr(_beg, _end - _beg + 1);
			}
			if (_idx->elements.size() & 1) {
				auto _beg = std::dynamic_pointer_cast<Integer>(_idx->elements[_idx->elements.size() - 1])->value;
				if (_beg >= _val->value.length() || _beg < 0) {
					out_of_range_error("Index Statement: String: Expected [0, " + std::to_string(_val->value.length() - 1) + "], got " + std::to_string(_beg) + ".");
					return make_error();
				}
				result += _val->value.substr(_beg);
			}
			return std::make_shared<String>(result);
		}
		data_type_error("Indexes must be Integer of Array of Integer.");
		return make_error();
	}
	if (left->type == Object::Type::Array) {
		if (index->type == Object::Type::Integer) {
			auto _idx = std::dynamic_pointer_cast<Integer>(index);
			auto _val = std::dynamic_pointer_cast<Array>(left);
			if (_idx->value >= _val->elements.size() || _idx->value < 0) {
				out_of_range_error("Index Statement: Array: Expected [0, " + std::to_string(_val->elements.size() - 1) + "], got " + std::to_string(_idx->value) + ".");
				return make_error();
			}
			return Object::toConstant(_val->elements[_idx->value]->copy());
		}
		if (index->type == Object::Type::Array) {
			auto _idx = std::dynamic_pointer_cast<Array>(index);
			auto _val = std::dynamic_pointer_cast<Array>(left);
			std::vector<std::shared_ptr<Object>> result;
			for (size_t i = 0; i + 1 < _idx->elements.size(); i += 2) {
				if (_idx->elements[i]->type != Object::Type::Integer || _idx->elements[i + 1]->type != Object::Type::Integer) {
					data_type_error("Indexes must be Integer or Array of Integer.");
					return make_error();
				}
				auto _beg = std::dynamic_pointer_cast<Integer>(_idx->elements[i])->value, _end = std::dynamic_pointer_cast<Integer>(_idx->elements[i + 1])->value;
				if (_end < _beg) {
					out_of_range_error("Index Statement: Array: Negative Width: End(" + std::to_string(_end) + ") < Begin(" + std::to_string(_beg) + ").");
					return make_error();
				}
				if (_end > _val->elements.size() || _beg < 0) {
					out_of_range_error("Index Statement: Array: Expected [0, " + std::to_string(_val->elements.size() - 1) + "], got [" + std::to_string(_beg) + ", " + std::to_string(_end) + "].");
					return make_error();
				}
				result.insert(result.end(), _val->elements.begin() + _beg, _val->elements.begin() + _end + 1);
			}
			if (_idx->elements.size() & 1) {
				auto _beg = std::dynamic_pointer_cast<Integer>(_idx->elements[_idx->elements.size() - 1])->value;
				if (_beg >= _val->elements.size() || _beg < 0) {
					out_of_range_error("Index Statement: Array: Expected [0, " + std::to_string(_val->elements.size() - 1) + "], got " + std::to_string(_beg) + ".");
					return make_error();
				}
				result.insert(result.end(), _val->elements.begin() + _beg, _val->elements.end());
			}
			return std::make_shared<Array>(result);
		}
		data_type_error("Indexes must be Integer of Array of Integer.");
		return make_error();
	}
	data_type_error("Index Statement supports Instances, Strings and Arrays only.");
	return make_error();
}

std::shared_ptr<Object> Evaluator::evaluate_infix(std::shared_ptr<InfixNode> _infix, Environment* env) {
	auto left = evaluate_value(_infix->left, env);
	if (_infix->_op == ".") {
		if (_infix->right->type != Node::Type::Identifier) {
			sub_error("Supports Object->[Identifier] only.");
			return make_error();
		}
		if (left->type == Object::Type::Instance) {
			auto innerEnv = std::dynamic_pointer_cast<Instance>(left)->inner;
			auto str = std::dynamic_pointer_cast<IdentifierNode>(_infix->right)->id;
			if (innerEnv->getThere(str)->type != Object::Type::Null) {
				sub_error("Instance does not have member \"" + str + "\".");
				return make_error();
			}
			if (innerEnv->getAT(str) != Item::AccessToken::Public) {
				auto cll = innerEnv->getThere("__indexes__");
				auto clr = env->get("__indexes__");
				if (!check_class_relationship(cll, clr)) {
					sub_error("Instance member \"" + str + "\" is private.");
					return make_error();
				}
			}
			return innerEnv->getThere(str);
		}
		if (left->type == Object::Type::Enumerate) {
			auto eu = std::dynamic_pointer_cast<Enumerate>(left);
			auto str = std::dynamic_pointer_cast<IdentifierNode>(_infix->right)->id;
			if (eu->_enum.count(str) > 0) {
				return std::make_shared<Integer>(eu->_enum[str]);
			}
			else {
				sub_error("Enumerate value " + str + " not found.");
				return make_error();
			}
		}
		// "Reversed Call"
		// a.func(args) is same as func(a, args).
		// Exactly:
		// function(..._args_) {
		//     return func(a, ..._args);
		// }(args);
		auto _call_stat = std::make_shared<CallNode>();
		_call_stat->to_run = _infix->right;
		_call_stat->args.push_back(std::make_shared<ConstantNode>(left));
		_call_stat->more = std::make_shared<IdentifierNode>("_arg_");
		auto _ret_stat = std::make_shared<ReturnNode>(_call_stat);
		auto _sta = std::make_shared<StatementNode>();
		_sta->inner = _ret_stat;
		auto inner = std::make_shared<RegionNode>();
		inner->statements.push_back(_sta);
		auto fc = std::make_shared<FunctionNode>();
		fc->inner = inner;
		fc->moreName = "_arg_";
		auto func = std::make_shared<Function>(fc, env);
		return func;
	}
	auto right = evaluate_value(_infix->right, env);
	return calcuate_infix(left, right, _infix->_op, env);
}

std::shared_ptr<Object> Evaluator::evaluate_prefix(std::shared_ptr<PrefixNode> _prefix, Environment* env) {
	auto body = evaluate_value(_prefix->right, env);
	if (body->type == Object::Type::Instance) {
		auto func = std::dynamic_pointer_cast<Instance>(body)->inner->getThere("prefix" + _prefix->_op);
		if (func->type == Object::Type::Function) {
			auto _fc = std::dynamic_pointer_cast<Function>(func);
			if (_fc->args.size() != 0 || _fc->hasMore()) {
				args_size_error("Prefix functions must not have args.");
				return make_error();
			}
			auto innerEnv = std::make_shared<Environment>(env);
			if (_fc->inner->type != Node::Type::Region) {
				unhandled_error("Function->inner should be RegionNode, but found somewhere not.");
				return make_error();
			}
			return Object::toCommon(evaluate_region(std::dynamic_pointer_cast<RegionNode>(_fc->inner), innerEnv.get()));
		}
		if (func->type == Object::Type::NativeFunction) {
			auto _fc = std::dynamic_pointer_cast<NativeFunction>(func);
            auto res = _fc->_func(NativeFunction::arglist(), env);
            if (res.first == NativeFunction::Result::FORMAT_ERR) {
                args_size_error("Prefix function(native) reported FORMAT_ERROR.");
                return make_error();
            }
            if (res.first == NativeFunction::Result::DATA_ERR) {
                arg_data_error();
                return make_error();
            }
            if (res.first == NativeFunction::Result::UNHANDLED_ERR) {
                unhandled_error("NativeFunction reported UNHANDLED_ERROR.");
                return make_error();
            }
            return res.second;
		}
		data_type_error("Not a function/native-function - Instance->inner->getThere(\"prefix" + _prefix->_op + "\").");
		return make_error();
	}
	return calcuate_prefix(body, _prefix->_op);
}

std::shared_ptr<Object> Evaluator::evaluate_remove(std::shared_ptr<RemoveNode> _rem, Environment* env) {
	if (env->get(_rem->toRemove)->type == Object::Type::Null) {
		not_found_error(_rem->toRemove);
		return make_error();
	}
	env->remove(_rem->toRemove);
	return std::make_shared<Null>();
}

std::shared_ptr<Object> Evaluator::evaluate_return(std::shared_ptr<ReturnNode> _ret, Environment* env) {
	auto retval = evaluate_value(_ret->obj, env)->copy();
	retval->isReturn = true; // Deprecate it later. Enumerate "spec" will replace it.
	return retval;
}

std::shared_ptr<Object> Evaluator::evaluate_statement(std::shared_ptr<StatementNode> _sta, Environment* env) {
	if (_sta->inner->type == Node::Type::Creation) {
		return evaluate_creation(std::dynamic_pointer_cast<CreationNode>(_sta->inner), env);
	}
	if (_sta->inner->type == Node::Type::Import) {
		bool err = import_file(std::dynamic_pointer_cast<ImportNode>(_sta->inner), env);
		if (err) {
			import_error();
			return make_error();
		}
		return std::make_shared<Null>();
	}
	if (_sta->inner->type == Node::Type::Return) {
		return evaluate_return(std::dynamic_pointer_cast<ReturnNode>(_sta->inner), env);
	}
	if (_sta->inner->type == Node::Type::Remove) {
		return evaluate_remove(std::dynamic_pointer_cast<RemoveNode>(_sta->inner), env);
	}
	if (_sta->inner->type == Node::Type::Expr) {
		return evaluate_expr(std::dynamic_pointer_cast<ExprNode>(_sta->inner), env);
	}
	unhandled_error("Statement contained unexcepted type.");
	return make_error();
}

std::shared_ptr<Object> Evaluator::evaluate_ternary(std::shared_ptr<TernaryNode> _ter, Environment* env) {
	auto _cond = evaluate_value(_ter->_cond, env);
    if (isTrue(_cond)) {
        return evaluate_value(_ter->_if, env);
    }
    else {
        return evaluate_value(_ter->_else, env);
    }
}

std::shared_ptr<Object> Evaluator::evaluate_while(std::shared_ptr<WhileNode> _while, Environment* env) {
	bool last_check = _while->isDoWhile ? true : isTrue(evaluate_value(_while->_cond, env));
	std::shared_ptr<Object> _res = std::make_shared<Null>();
	while (last_check) {
		auto innerEnv = std::make_shared<Environment>(env);
		_res = evaluate_one(_while->_body, innerEnv.get());
		if (_res->isReturn || _res->type == Object::Type::Error) {
			return _res;
		}
		last_check = isTrue(evaluate_value(_while->_cond, env));
	}
	return _res;
}

std::shared_ptr<Object> Evaluator::evaluate_value(std::shared_ptr<Node> _val, Environment* env) {
	if (_val->type == Node::Type::Array) {
		return evaluate_array(std::dynamic_pointer_cast<ArrayNode>(_val), env);
	}
	if (_val->type == Node::Type::Assign) {
		return evaluate_assign(std::dynamic_pointer_cast<AssignNode>(_val), env);
	}
	if (_val->type == Node::Type::Call) {
		return evaluate_call(std::dynamic_pointer_cast<CallNode>(_val), env);
	}
	if (_val->type == Node::Type::Class) {
		return evaluate_class(std::dynamic_pointer_cast<ClassNode>(_val), env);
	}
	if (_val->type == Node::Type::Constant) {
		return evaluate_constant(std::dynamic_pointer_cast<ConstantNode>(_val));
	}
	if (_val->type == Node::Type::Enumerate) {
		return evaluate_enumerate(std::dynamic_pointer_cast<EnumerateNode>(_val), env);
	}
	if (_val->type == Node::Type::Error) {
		return make_error();
	}
	if (_val->type == Node::Type::Expr) {
		return evaluate_expr(std::dynamic_pointer_cast<ExprNode>(_val), env);
	}
	if (_val->type == Node::Type::Function) {
		return evaluate_function(std::dynamic_pointer_cast<FunctionNode>(_val), env);
	}
	if (_val->type == Node::Type::Identifier) {
		return evaluate_id(std::dynamic_pointer_cast<IdentifierNode>(_val), env);
	}
	if (_val->type == Node::Type::InDecrement) {
		return evaluate_indecrement(std::dynamic_pointer_cast<InDecrementNode>(_val), env);
	}
	if (_val->type == Node::Type::Index) {
		return evaluate_index(std::dynamic_pointer_cast<IndexNode>(_val), env);
	}
	if (_val->type == Node::Type::Infix) {
		return evaluate_infix(std::dynamic_pointer_cast<InfixNode>(_val), env);
	}
	if (_val->type == Node::Type::Prefix) {
		return evaluate_prefix(std::dynamic_pointer_cast<PrefixNode>(_val), env);
	}
	if (_val->type == Node::Type::Ternary) {
		return evaluate_ternary(std::dynamic_pointer_cast<TernaryNode>(_val), env);
	}
	if (_val->type == Node::Type::Group) {
		return evaluate_group(std::dynamic_pointer_cast<GroupNode>(_val), env);
	}
	data_type_error("Unknown value type.");
	return make_error();
}

std::shared_ptr<Object> Evaluator::evaluate_one(std::shared_ptr<Node> _sta, Environment* env) {
	if (_sta->type == Node::Type::Region) {
		return evaluate_region(std::dynamic_pointer_cast<RegionNode>(_sta), env, false);
	}
	if (_sta->type == Node::Type::Creation) {
		return evaluate_creation(std::dynamic_pointer_cast<CreationNode>(_sta), env);
	}
	if (_sta->type == Node::Type::Statement) {
		return evaluate_statement(std::dynamic_pointer_cast<StatementNode>(_sta), env);
	}
	if (_sta->type == Node::Type::If) {
		return evaluate_if(std::dynamic_pointer_cast<IfNode>(_sta), env);
	}
	if (_sta->type == Node::Type::For) {
		return evaluate_for(std::dynamic_pointer_cast<ForNode>(_sta), env);
	}
	if (_sta->type == Node::Type::While) {
		return evaluate_while(std::dynamic_pointer_cast<WhileNode>(_sta), env);
	}
	data_type_error("Unknown statement type.");
	return make_error();
}

std::shared_ptr<Object> Evaluator::evaluate_group(std::shared_ptr<GroupNode> gp, Environment* env) {
	return evaluate_value(gp->v, env);
}

void Evaluator::bind_args(std::vector<std::shared_ptr<Object>> v, std::vector<std::string> name, std::string more, std::shared_ptr<Environment> env) {
    for (size_t i = 0; i < name.size(); i++) {
        env->set(name[i], v[i]->copy());
		env->get(name[i])->markMutable(true);
    }
    if (more != "__null__") {
        auto arr = std::make_shared<Array>();
        for (size_t i = name.size(); i < v.size(); i++) {
            arr->elements.push_back(v[i]);
        }
        env->set(more, arr);
		env->get(more)->markMutable(true);
    }
}

void Evaluator::bind_args(std::vector<std::shared_ptr<Object>> v, std::vector<std::string> name, std::string more, Environment* env) {
    for (size_t i = 0; i < name.size(); i++) {
        env->set(name[i], v[i]->copy());
		env->get(name[i])->markMutable(true);
    }
    if (more != "__null__") {
        auto arr = std::make_shared<Array>();
        for (size_t i = name.size(); i < v.size(); i++) {
            arr->elements.push_back(v[i]);
        }
        env->set(more, arr);
		env->get(more)->markMutable(true);
    }
}

bool Evaluator::check_class_relationship(std::shared_ptr<Object> a, std::shared_ptr<Object> b) {
	if (a->type != Object::Type::Array || b->type != Object::Type::Array) {
		lvl_type_error();
		return false;
	}
	auto ac = std::dynamic_pointer_cast<Array>(a), bc = std::dynamic_pointer_cast<Array>(b);
	if (ac->elements.size() > bc->elements.size()) {
		return false;
	}
	auto ae = ac->elements[ac->elements.size() - 1], be = bc->elements[ac->elements.size() - 1];
	if (ae->type != Object::Type::Integer || be->type != Object::Type::Integer) {
		lvl_type_error();
		return false;
	}
	return std::dynamic_pointer_cast<Integer>(ae)->value == std::dynamic_pointer_cast<Integer>(be)->value;
}

std::shared_ptr<Object> Evaluator::calcuate_infix(std::shared_ptr<Object> left, std::shared_ptr<Object> right, std::string op, Environment* env) {
	// Instances
	if (left->type == Object::Type::Instance) {
		auto func = std::dynamic_pointer_cast<Instance>(left)->inner->getThere("operator" +  op);
		if (func->type == Object::Type::Function) {
			auto _fc = std::dynamic_pointer_cast<Function>(func);
			if (_fc->args.size() != 1 || _fc->hasMore()) {
				args_size_error("Infix functions can contain 1 arg only and cannot use MoreArgument.");
				return make_error();
			}
			auto innerEnv = std::make_shared<Environment>(env);
			innerEnv->set(_fc->args[0], right);
			innerEnv->get(_fc->args[0])->markMutable(true);
			if (_fc->inner->type != Node::Type::Region) {
				unhandled_error("Function->inner should be RegionNode, but found somewhere not.");
				return make_error();
			}
			return Object::toCommon(evaluate_region(std::dynamic_pointer_cast<RegionNode>(_fc->inner), innerEnv.get()));
		}
		if (func->type == Object::Type::NativeFunction) {
			auto _fc = std::dynamic_pointer_cast<NativeFunction>(func);
            auto res = _fc->_func(NativeFunction::arglist{right}, env);
            if (res.first == NativeFunction::Result::FORMAT_ERR) {
                args_size_error("Infix function(native) reported FORMAT_ERROR.");
                return make_error();
            }
            if (res.first == NativeFunction::Result::DATA_ERR) {
                arg_data_error();
                return make_error();
            }
            if (res.first == NativeFunction::Result::UNHANDLED_ERR) {
                unhandled_error("NativeFunction reported UNHANDLED_ERROR.");
                return make_error();
            }
            return res.second;
		}
		data_type_error("Not a function/native-function - Instance->inner->getThere(\"operator" + op + "\").");
		return make_error();
	}
	if (right->type == Object::Type::Instance) {
		auto func = std::dynamic_pointer_cast<Instance>(right)->inner->getThere("operator" + op);
		if (func->type == Object::Type::Function) {
			auto _fc = std::dynamic_pointer_cast<Function>(func);
			if (_fc->args.size() != 1 || _fc->hasMore()) {
				args_size_error("Infix functions can contain 1 arg only and cannot use MoreArgument.");
				return make_error();
			}
			auto innerEnv = std::make_shared<Environment>(env);
			innerEnv->set(_fc->args[0], left);
			innerEnv->get(_fc->args[0])->markMutable(true);
			if (_fc->inner->type != Node::Type::Region) {
				unhandled_error("Function->inner should be RegionNode, but found somewhere not.");
				return make_error();
			}
			return Object::toCommon(evaluate_region(std::dynamic_pointer_cast<RegionNode>(_fc->inner), innerEnv.get()));
		}
		if (func->type == Object::Type::NativeFunction) {
			auto _fc = std::dynamic_pointer_cast<NativeFunction>(func);
            auto res = _fc->_func(NativeFunction::arglist{left}, env);
            if (res.first == NativeFunction::Result::FORMAT_ERR) {
                args_size_error("Infix function(native) reported FORMAT_ERROR.");
                return make_error();
            }
            if (res.first == NativeFunction::Result::DATA_ERR) {
                arg_data_error();
                return make_error();
            }
            if (res.first == NativeFunction::Result::UNHANDLED_ERR) {
                unhandled_error("NativeFunction reported UNHANDLED_ERROR.");
                return make_error();
            }
            return res.second;
		}
		data_type_error("Not a function/native-function - Instance->inner->getThere(\"operator" + op + "\").");
		return make_error();
	}
	// Equations
	if (op == ">" || op == "<" || op == "==" || op == "!=" || op == ">=" || op == "<=") {
		return calcuate_infix_compare(left, right, op, env);
	}
	// Logical
	if (op == "||" || op == "&&") {
		return calcuate_infix_boolean(std::make_shared<Boolean>(isTrue(left)), std::make_shared<Boolean>(isTrue(right)), op);
	}
	// String
	if (left->type == Object::Type::String) {
		if (right->type == Object::Type::String) {
			return calcuate_infix_string(std::dynamic_pointer_cast<String>(left), std::dynamic_pointer_cast<String>(right), op);
		}
		return calcuate_infix_string(std::dynamic_pointer_cast<String>(left), std::make_shared<String>(right->toString()), op);
	}
	if (right->type == Object::Type::String) {
		return calcuate_infix_string(std::make_shared<String>(left->toString()), std::dynamic_pointer_cast<String>(right), op);
	}
	// Array
	if (left->type == Object::Type::Array && right->type == Object::Type::Array) {
		return calcuate_infix_array(std::dynamic_pointer_cast<Array>(left), std::dynamic_pointer_cast<Array>(right), op);
	}
	// Common Check
	if (left->type != Object::Type::Boolean && left->type != Object::Type::Integer && left->type != Object::Type::Float) {
		wrong_infix_error(left->typeOf(), right->typeOf(), op);
	}
	if (right->type != Object::Type::Boolean && right->type != Object::Type::Integer && right->type != Object::Type::Float) {
		wrong_infix_error(left->typeOf(), right->typeOf(), op);
	}
	// Common (Boolean, Integer, Float)
	if (left->type == Object::Type::Boolean) {
		if (right->type == Object::Type::Boolean) /* true + true = 2 */ {
			return calcuate_infix_integer(std::make_shared<Integer>(isTrue(left)), std::make_shared<Integer>(isTrue(right)), op);
		}
		if (right->type == Object::Type::Integer) {
			return calcuate_infix_integer(std::make_shared<Integer>(isTrue(left)), std::dynamic_pointer_cast<Integer>(right), op);
		}
		if (right->type == Object::Type::Float) {
			return calcuate_infix_float(std::make_shared<Float>(isTrue(left)), std::dynamic_pointer_cast<Float>(right), op);
		}
	}
	if (left->type == Object::Type::Integer) {
		if (right->type == Object::Type::Boolean) {
			return calcuate_infix_integer(std::dynamic_pointer_cast<Integer>(left), std::make_shared<Integer>(isTrue(right)), op);
		}
		if (right->type == Object::Type::Integer) {
			return calcuate_infix_integer(std::dynamic_pointer_cast<Integer>(left), std::dynamic_pointer_cast<Integer>(right), op);
		}
		if (right->type == Object::Type::Float) {
			return calcuate_infix_float(std::make_shared<Float>(std::dynamic_pointer_cast<Integer>(left)->value), std::dynamic_pointer_cast<Float>(right), op);
		}
	}
	if (left->type == Object::Type::Float) {
		if (right->type == Object::Type::Boolean) {
			return calcuate_infix_float(std::dynamic_pointer_cast<Float>(left), std::make_shared<Float>(isTrue(right)), op);
		}
		if (right->type == Object::Type::Integer) {
			return calcuate_infix_float(std::dynamic_pointer_cast<Float>(left), std::make_shared<Float>(std::dynamic_pointer_cast<Integer>(right)->value), op);
		}
		if (right->type == Object::Type::Float) {
			return calcuate_infix_float(std::dynamic_pointer_cast<Float>(left), std::dynamic_pointer_cast<Float>(right), op);
		}
	}
	return make_error();
}

std::shared_ptr<Object> Evaluator::calcuate_prefix(std::shared_ptr<Object> body, std::string op) {
	if (op == "!") {
		return std::make_shared<Boolean>(!isTrue(body));
	}
	if (op == "-") {
		if (body->type == Object::Type::Float) {
			return std::make_shared<Float>(-std::dynamic_pointer_cast<Float>(body)->value);
		}
		if (body->type == Object::Type::Integer) {
			return std::make_shared<Integer>(-std::dynamic_pointer_cast<Integer>(body)->value);
		}
		if (body->type == Object::Type::Boolean) {
			return std::make_shared<Integer>(-(long long) isTrue(body));
		}
		wrong_prefix_error(body->typeOf(), op);
		return make_error();
	}
	if (op == "+") {
		if (body->type == Object::Type::Float) {
			return body->copy();
		}
		if (body->type == Object::Type::Integer) {
			return body->copy();
		}
		if (body->type == Object::Type::Boolean) {
			return std::make_shared<Integer>(isTrue(body));
		}
		wrong_prefix_error(body->typeOf(), op);
		return make_error();
	}
	if (op == "~") {
		if (body->type == Object::Type::Integer) {
			return std::make_shared<Integer>(~std::dynamic_pointer_cast<Integer>(body)->value);
		}
		if (body->type == Object::Type::Boolean) {
			return std::make_shared<Integer>(~(long long) isTrue(body));
		}
		wrong_prefix_error(body->typeOf(), op);
		return make_error();
	}
	wrong_prefix_error(body->typeOf(), op);
	return make_error();
}

std::shared_ptr<Object> Evaluator::calcuate_infix_boolean(std::shared_ptr<Boolean> left, std::shared_ptr<Boolean> right, std::string op) {
	if (op == "||") {
		return std::make_shared<Boolean>(isTrue(left) || isTrue(right));
	}
	if (op == "&&") {
		return std::make_shared<Boolean>(isTrue(left) && isTrue(right));
	}
	wrong_infix_error("Boolean", "Boolean", "Boolean::operator" + op);
	return make_error();
}

std::shared_ptr<Object> Evaluator::calcuate_infix_integer(std::shared_ptr<Integer> left, std::shared_ptr<Integer> right, std::string op) {
	auto a = left->value, b = right->value;
	if (op == "+") {
		return std::make_shared<Integer>(a + b);
	}
	if (op == "-") {
		return std::make_shared<Integer>(a - b);
	}
	if (op == "*") {
		return std::make_shared<Integer>(a * b);
	}
	if (op == "/") {
		return std::make_shared<Integer>(a / b);
	}
	if (op == "%") {
		return std::make_shared<Integer>(a % b);
	}
	if (op == "&") {
		return std::make_shared<Integer>(a & b);
	}
	if (op == "|") {
		return std::make_shared<Integer>(a | b);
	}
	if (op == "^") {
		return std::make_shared<Integer>(a ^ b);
	}
	if (op == "<<") {
		return std::make_shared<Integer>(a << b);
	}
	if (op == ">>") {
		return std::make_shared<Integer>(a >> b);
	}
	wrong_infix_error("Integer", "Integer", "Integer::operator" + op);
	return make_error();
}

std::shared_ptr<Object> Evaluator::calcuate_infix_float(std::shared_ptr<Float> left, std::shared_ptr<Float> right, std::string op) {
	auto a = left->value, b = right->value;
	if (op == "+") {
		return std::make_shared<Float>(a + b);
	}
	if (op == "-") {
		return std::make_shared<Float>(a - b);
	}
	if (op == "*") {
		return std::make_shared<Float>(a * b);
	}
	if (op == "/") {
		return std::make_shared<Float>(a / b);
	}
	if (op == "%") {
		return std::make_shared<Float>(fmod(a, b));
	}
	wrong_infix_error("Float", "Float", "Float::operator" + op);
	return make_error();
}

std::shared_ptr<Object> Evaluator::calcuate_infix_string(std::shared_ptr<String> left, std::shared_ptr<String> right, std::string op) {
	if (op != "+") {
		wrong_infix_error("String", "String", "String::operator" + op);
		return make_error();
	}
	return std::make_shared<String>(left->value + right->value);
}

std::shared_ptr<Object> Evaluator::calcuate_infix_array(std::shared_ptr<Array> left, std::shared_ptr<Array> right, std::string op) {
	if (op != "+") {
		wrong_infix_error("Array", "Array", "Array::operator" + op);
		return make_error();
	}
	auto ac = std::dynamic_pointer_cast<Array>(left->copy()), bc = std::dynamic_pointer_cast<Array>(right->copy()), res = std::make_shared<Array>();
	res->elements.insert(res->elements.end(), ac->elements.begin(), ac->elements.end());
	res->elements.insert(res->elements.end(), bc->elements.begin(), bc->elements.end());
	return res;
}

std::shared_ptr<Object> Evaluator::calcuate_infix_compare(std::shared_ptr<Object> left, std::shared_ptr<Object> right, std::string op, Environment* env) {
	if (left->type == Object::Type::String && right->type != Object::Type::String) {
		right = std::make_shared<String>(right->toString());
	}
	else if (right->type == Object::Type::String && left->type != Object::Type::String) {
		left = std::make_shared<String>(left->toString());
	}
	else if (left->type == Object::Type::Float) {
		if (right->type == Object::Type::Integer) {
			right = std::make_shared<Float>(std::dynamic_pointer_cast<Integer>(right)->value);
		}
		else if (right->type == Object::Type::Boolean) {
			right = std::make_shared<Float>(isTrue(right));
		}
		else if (right->type != Object::Type::Float) {
			return std::make_shared<Boolean>(false);
		}
	}
	else if (right->type == Object::Type::Float) {
		if (left->type == Object::Type::Integer) {
			left = std::make_shared<Float>(std::dynamic_pointer_cast<Integer>(left)->value);
		}
		else if (left->type == Object::Type::Boolean) {
			left = std::make_shared<Float>(isTrue(left));
		}
		else if (right->type != Object::Type::Float) {
			return std::make_shared<Boolean>(false);
		}
	}
	else if (left->type == Object::Type::Integer) {
		if (right->type == Object::Type::Boolean) {
			right = std::make_shared<Integer>(isTrue(right));
		}
		else if (right->type != Object::Type::Integer) {
			return std::make_shared<Boolean>(false);
		}
	}
	else if (right->type == Object::Type::Integer) {
		if (left->type == Object::Type::Boolean) {
			left = std::make_shared<Integer>(isTrue(left));
		}
		else if (left->type != Object::Type::Integer) {
			return std::make_shared<Boolean>(false);
		}
	}
	if (left->type != right->type) {
		return std::make_shared<Boolean>(false);
	}
	if (op == "==") {
		if (left->type == Object::Type::Array) {
			auto ac = std::dynamic_pointer_cast<Array>(left), bc = std::dynamic_pointer_cast<Array>(right);
			if (ac->elements.size() != bc->elements.size()) {
				return std::make_shared<Boolean>(false);
			}
			for (size_t i = 0; i < ac->elements.size(); i++) {
				if (!isTrue(calcuate_infix_compare(ac->elements[i], bc->elements[i], "==", env))) {
					return std::make_shared<Boolean>(false);
				}
			}
			return std::make_shared<Boolean>(true);
		}
		else if (left->type == Object::Type::String) {
			return std::make_shared<Boolean>(std::dynamic_pointer_cast<String>(left)->value == std::dynamic_pointer_cast<String>(right)->value);
		}
		else if (left->type == Object::Type::Integer) {
			return std::make_shared<Boolean>(std::dynamic_pointer_cast<Integer>(left)->value == std::dynamic_pointer_cast<Integer>(right)->value);
		}
		else if (left->type == Object::Type::Float) {
			return std::make_shared<Boolean>(std::dynamic_pointer_cast<Float>(left)->value == std::dynamic_pointer_cast<Float>(right)->value);
		}
		else if (left->type == Object::Type::Boolean) {
			return std::make_shared<Boolean>(isTrue(left) ^ isTrue(right));
		}
	}
	if (op == "!=") {
		if (left->type == Object::Type::Array) {
			auto ac = std::dynamic_pointer_cast<Array>(left), bc = std::dynamic_pointer_cast<Array>(right);
			if (ac->elements.size() != bc->elements.size()) {
				return std::make_shared<Boolean>(true);
			}
			for (size_t i = 0; i < ac->elements.size(); i++) {
				if (!isTrue(calcuate_infix_compare(ac->elements[i], bc->elements[i], "==", env))) {
					return std::make_shared<Boolean>(true);
				}
			}
			return std::make_shared<Boolean>(false);
		}
		else if (left->type == Object::Type::String) {
			return std::make_shared<Boolean>(std::dynamic_pointer_cast<String>(left)->value != std::dynamic_pointer_cast<String>(right)->value);
		}
		else if (left->type == Object::Type::Integer) {
			return std::make_shared<Boolean>(std::dynamic_pointer_cast<Integer>(left)->value != std::dynamic_pointer_cast<Integer>(right)->value);
		}
		else if (left->type == Object::Type::Float) {
			return std::make_shared<Boolean>(std::dynamic_pointer_cast<Float>(left)->value != std::dynamic_pointer_cast<Float>(right)->value);
		}
		else if (left->type == Object::Type::Boolean) {
			return std::make_shared<Boolean>(!(isTrue(left) ^ isTrue(right)));
		}
		else {
			return std::make_shared<Boolean>(false);
		}
	}
	if (op == ">") {
		if (left->type == Object::Type::Integer) {
			return std::make_shared<Boolean>(std::dynamic_pointer_cast<Integer>(left)->value > std::dynamic_pointer_cast<Integer>(right)->value);
		}
		else if (left->type == Object::Type::Float) {
			return std::make_shared<Boolean>(std::dynamic_pointer_cast<Float>(left)->value > std::dynamic_pointer_cast<Float>(right)->value);
		}
		else if (left->type == Object::Type::String) {
			return std::make_shared<Boolean>(std::dynamic_pointer_cast<String>(left)->value > std::dynamic_pointer_cast<String>(right)->value);
		}
		else {
			return std::make_shared<Boolean>(false);
		}
	}
	if (op == ">=") {
		if (left->type == Object::Type::Integer) {
			return std::make_shared<Boolean>(std::dynamic_pointer_cast<Integer>(left)->value >= std::dynamic_pointer_cast<Integer>(right)->value);
		}
		else if (left->type == Object::Type::Float) {
			return std::make_shared<Boolean>(std::dynamic_pointer_cast<Float>(left)->value >= std::dynamic_pointer_cast<Float>(right)->value);
		}
		else if (left->type == Object::Type::String) {
			return std::make_shared<Boolean>(std::dynamic_pointer_cast<String>(left)->value >= std::dynamic_pointer_cast<String>(right)->value);
		}
		else {
			return std::make_shared<Boolean>(false);
		}
	}
	if (op == "<") {
		if (left->type == Object::Type::Integer) {
			return std::make_shared<Boolean>(std::dynamic_pointer_cast<Integer>(left)->value < std::dynamic_pointer_cast<Integer>(right)->value);
		}
		else if (left->type == Object::Type::Float) {
			return std::make_shared<Boolean>(std::dynamic_pointer_cast<Float>(left)->value < std::dynamic_pointer_cast<Float>(right)->value);
		}
		else if (left->type == Object::Type::String) {
			return std::make_shared<Boolean>(std::dynamic_pointer_cast<String>(left)->value < std::dynamic_pointer_cast<String>(right)->value);
		}
		else {
			return std::make_shared<Boolean>(false);
		}
	}
	if (op == "<=") {
		if (left->type == Object::Type::Integer) {
			return std::make_shared<Boolean>(std::dynamic_pointer_cast<Integer>(left)->value <= std::dynamic_pointer_cast<Integer>(right)->value);
		}
		else if (left->type == Object::Type::Float) {
			return std::make_shared<Boolean>(std::dynamic_pointer_cast<Float>(left)->value <= std::dynamic_pointer_cast<Float>(right)->value);
		}
		else if (left->type == Object::Type::String) {
			return std::make_shared<Boolean>(std::dynamic_pointer_cast<String>(left)->value <= std::dynamic_pointer_cast<String>(right)->value);
		}
		else {
			return std::make_shared<Boolean>(false);
		}
	}
	return  std::make_shared<Boolean>(false);
}

bool Evaluator::isTrue(std::shared_ptr<Object> obj) {
	if (obj->type == Object::Type::Boolean) {
		return std::dynamic_pointer_cast<Boolean>(obj)->_case;
	}
	if (obj->type == Object::Type::Integer) {
		return std::dynamic_pointer_cast<Integer>(obj)->value != 0;
	}
	if (obj->type == Object::Type::Float) {
		auto f = std::dynamic_pointer_cast<Float>(obj)->value;
		return f != 0.0 && !std::isnan(f);
	}
	if (obj->type == Object::Type::Null || obj->type == Object::Type::Error) {
		return false;
	}
	if (obj->type == Object::Type::String) {
		return obj->toString().length() > 0;
	}
	return true;
}

void arg_data_error() {
	err_begin(true);
	std::cout << "Argument Data Error.";
	err_end();
}

void unhandled_error(std::string reason) {
	err_begin(true);
	std::cout << "Unhandled Error by " << reason;
	err_end();
}

void program_error() {
	err_begin(true);
	std::cout << "Program Init Error.";
	err_end();
}

void node_type_error() {
	err_begin(true);
	std::cout << "Unexpected Node Type.";
	err_end();
}

void args_size_erorr(std::string msg) {
	err_begin(true);
	std::cout << "Argument Size Error: " << msg;
	err_end();
}

void invalid_constructor_error() {
	err_begin(true);
	std::cout << "Invalid Constructor.";
	err_end();
}

void data_type_error(std::string msg) {
	err_begin(true);
	std::cout << "Data Type Error: " << msg;
	err_end();
}

void data_value_error() {
	err_begin(true);
	std::cout << "Unsupported Data Value.";
	err_end();
}

void not_found_error(std::string name) {
	err_begin(true);
	std::cout << name << ": Not Found.";
	err_end();
}

void more_type_error(std::string msg) {
	err_begin(true);
	std::cout << "Illegal type of More Argument: " << msg;
	err_end();
}

void not_mutable_error() {
	err_begin(true);
	std::cout << "Cannot Assign a Constant!";
	err_end();
}

void type_different_error(std::string l, std::string r) {
	err_begin(true);
	std::cout << "Cannot assign type " << l << " into type " << r;
	err_end();
}

void extand_nothing_error(std::string name) {
	err_begin(true);
	std::cout << "Trying to inheritance nothing.";
	err_end();
}

void lvl_type_error() {
	err_begin(true);
	std::cout << "Incorrect \"Instance.__level__\".";
	err_end();
}

void conde_type_error() {
	err_begin(true);
	std::cout << "Unsupported type for Increment/Decrement.";
	err_end();
}

void already_valid_error(std::string name) {
	err_begin(true);
	std::cout << "Variable \"" << name << "\" already exists.";
	err_end();
}

void for_elem_error(std::string name) {
	err_begin(true);
	std::cout << "FOR Element Error for \"" << name << "\"";
	err_end();
}

void idc_error(std::string name) {
	err_begin(true);
	std::cout << "Increment/Decrement Error: " << name;
	err_end();
}

void out_of_range_error(std::string text) {
	err_begin(true);
	std::cout << "Out of Range: " << text;
	err_end();
}

void invalid_error(std::string name) {
	// Deprecated
	err_begin(true);
	std::cout << "Deprecated/Invalid Error: " << name;
	err_end();
}

void sub_error(std::string name) {
	err_begin(true);
	std::cout << "Object Index Error: " << name;
	err_end();
}

void wrong_infix_error(std::string lt, std::string rt, std::string op) {
	err_begin(true);
	std::cout << "Invalid Index : \"" << op << "\" between " << lt << " and " << rt;
	err_end();
}

void wrong_prefix_error(std::string bt, std::string op) {
	err_begin(true);
	std::cout << "Invalid Prefix: \"" << op << "\" for " << bt;
	err_end();
}