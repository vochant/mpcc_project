#pragma once

#include "evaluator/evaluator.hpp"

std::shared_ptr<Object> Evaluator::evaluate_program(std::shared_ptr<ProgramNode> _prog, std::shared_ptr<Environment> env) {
    auto _inner = _prog->mainRegion;
    if (_inner->type != Node::Type::Region) {
        program_error();
        return make_error();
    }
    auto _cast = std::dynamic_pointer_cast<RegionNode>(_inner);
    return evaluate_region(_cast, env);
}

std::shared_ptr<Object> Evaluator::evaluate_constant(std::shared_ptr<ConstantNode> _constant) {
    auto obj = _constant->obj;
    obj->markMutable(false);
}

std::shared_ptr<Object> Evaluator::evaluate_region(std::shared_ptr<RegionNode> _region, std::shared_ptr<Environment> env) {
    std::shared_ptr<Object> res = std::make_shared<Null>();
    for (auto i : _region->statements) {
        res = evaluate_one(i, env);
        if (res->isReturn || res->type == Object::Type::Error) {
            return res;
        }
    }
    return res;
}

std::shared_ptr<Object> Evaluator::evaluate_function(std::shared_ptr<FunctionNode> _func, std::shared_ptr<Environment> env) {
    return std::make_shared<Function>(_func, env);
}

std::shared_ptr<Object> Evaluator::evaluate_call(std::shared_ptr<CallNode> _call, std::shared_ptr<Environment> env) {
    auto body = evaluate_value(_call->to_run, env);
    std::vector<std::shared_ptr<Object>> args;
    for (auto i : _call->args) {
        args.push_back(evaluate_value(i, env));
    }
    if (_call->more) {
        auto _more = evaluate_value(_call->more, env);
        if (_more->type != Object::Type::Array) {
            more_type_error();
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
            args_size_error();
            return make_error();
        }
        auto innerEnv = std::make_shared<Environment>(_funcCast->isLambda ? env : _funcCast->outerEnv);
        bind_args(args, _funcCast->args, _funcCast->moreName, innerEnv);
        if (_funcCast->inner->type != Node::Type::Region) {
            node_type_error();
            return make_error();
        }
        return evaluate_region(std::dynamic_pointer_cast<RegionNode>(_funcCast->inner), innerEnv);
    }
    else if (body->type == Object::Type::Class) {
        auto _classCast = std::dynamic_pointer_cast<Class>(body);
        auto _inst = std::make_shared<Instance>(_classCast);
        auto _cobj = _inst->inner->getThere("__spec_constructor" + _inst->inner->getThere("__level__")->toString() + "_" + std::to_string(args.size()));
        if (_cobj->type == Object::Type::Null) {
            invalid_constructor_error();
            return make_error();
        }
        if (_cobj->type != Object::Type::Function) {
            data_type_error();
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
        evaluate_region(std::dynamic_pointer_cast<RegionNode>(_func->inner), _innerEnv);
        return _inst;
    }
    else if (body->type == Object::Type::Instance) {
        auto _instCast = std::make_shared<Instance>(body);
        auto _cobj = _instCast->inner->getThere("operator()");
        if (_cobj->type == Object::Type::Null) {
            not_found_error();
            return make_error();
        }
        if (_cobj->type == Object::Type::Function) {
            auto _func = std::dynamic_pointer_cast<Function>(_cobj);
            if (_func->args.size() > args.size() && (_func->hasMore() || _func->args.size() == args.size())) {
                args_size_error();
                return make_error();
            }
            if (_func->isLambda) {
                data_value_error();
                return make_error();
            }
            auto _innerEnv = std::make_shared<Environment>(_instCast->inner);
            bind_args(args, _func->args, _func->moreName, _innerEnv);
            if (_func->inner->type != Node::Type::Region) {
                node_type_error();
                return make_error();
            }
            return evaluate_region(std::dynamic_pointer_cast<RegionNode>(_func->inner), _innerEnv);
        }
        else if (_cobj->type == Object::Type::NativeFunction) {
            auto _func = std::dynamic_pointer_cast<NativeFunction>(_cobj);
            auto[_stat, _rval] = _func->func(args);
            if (_stat == NativeFunction::Result::FORMAT_ERR) {
                args_size_error();
                return make_error();
            }
            else if (_stat == NativeFunction::Result::DATA_ERR) {
                arg_data_error();
                return make_error();
            }
            else if (_stat == NativeFunction::Result::UNHANDLED_ERR) {
                unhandled_error();
                return make_error();
            }
            else {
                return _rval;
            }
        }
    }
    else if (body->type == Object::Type::NativeFunction) {
        auto _funcCast = std::dynamic_pointer_cast<NativeFunction>(body);
        auto[_stat, _rval] = _funcCast->func(args);
        if (_stat == NativeFunction::Result::FORMAT_ERR) {
            args_size_error();
            return make_error();
        }
        else if (_stat == NativeFunction::Result::DATA_ERR) {
            arg_data_error();
            return make_error();
        }
        else if (_stat == NativeFunction::Result::UNHANDLED_ERR) {
            unhandled_error();
            return make_error();
        }
        else {
            return _rval;
        }
    }
    else {
        data_type_error();
        return make_error();
    }
}

std::shared_ptr<Object> Evaluator::evaluate_array(std::shared_ptr<ArrayNode> _arr, std::shared_ptr<Environment> env) {
    auto arr = std::make_shared<Array>();
    for (auto i : _arr->elements) {
        arr->elements.push_back(evaluate_value(i, env));
    }
    return arr;
}

std::shared_ptr<Object> Evaluator::evaluate_assign(std::shared_ptr<AssignNode> _assign, std::shared_ptr<Environment> env) {
    auto w = evaluate_value(_assign->left, env);
    if (!w->isMutable) {
        not_mutable_error();
        return make_error();
    }
    auto v = evaluate_value(_assign->right, env);
    if (_assign->_op != "=") {
        auto bef = _assign->_op.substr(0, _assign->_op.length() - 1);
        v = calcuate_infix(w, v, bef);
    }
    if (w->type != v->type) {
        type_different_error(w->typeOf(), v->typeOf());
        return make_error();
    }
    w->assign(v);
}

std::shared_ptr<Object> Evaluator::evaluate_class(std::shared_ptr<ClassNode> _class, std::shared_ptr<Environment> env) {
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
    }
    else {
        cls = std::make_shared<Class>(env);
        cls->inner->set("__level__", std::make_shared<Integer>(0));
    }
    if (_class->inner->type != Node::Type::Region) {
        unhandled_error();
        return make_error();
    }
    evaluate_region(std::dynamic_pointer_cast<RegionNode>(_class->inner), cls->inner);
    return cls;
}

std::shared_ptr<Object> Evaluator::evaluate_if(std::shared_ptr<IfNode> _if, std::shared_ptr<Environment> env) {
    auto _cond = evaluate_value(_if->_cond, env);
    std::shared_ptr<Object> _res;
    if (isTrue(_cond)) {
        auto innerEnv = std::make_shared<Environment>(env);
        _res = evaluate_one(_if->_then, innerEnv);
    }
    else if (_if->_else) {
        auto innerEnv = std::make_shared<Environment>(env);
        _res = evaluate_one(_if->_else, innerEnv);
    }
    if (_res->isReturn) {
        return _res;
    }
    return std::shared_ptr<Null>();
}

std::shared_ptr<Object> Evaluator::evaluate_creation(std::shared_ptr<CreationNode> _cr, std::shared_ptr<Environment> env) {
    while (env->_parent) {
        env = env->_parent;
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

std::shared_ptr<Object> Evaluator::evaluate_enumerate(std::shared_ptr<EnumerateNode> _enum, std::shared_ptr<Environment> env) {
    std::vector<std::pair<std::string, long long>> v;
    long long _idx = 0;
    for (auto i : _enum->items) {
        v.push_back(std::make_pair(i, _idx++));
    }
    auto e = std::make_shared<Enumerate>(v);
    return e;
}

std::shared_ptr<Object> Evaluator::evaluate_expr(std::shared_ptr<ExprNode> _expr, std::shared_ptr<Environment> env) {
    return evaluate_value(_expr->inner, env);
}

std::shared_ptr<Object> Evaluator::evaluate_for(std::shared_ptr<ForNode> _for, std::shared_ptr<Environment> env) {
    std::shared_ptr<Object> _res;
    auto innerEnv = std::make_shared<Environment>(env);
    auto elems = evaluate_value(_for->_elem, env);
    if (elems->type != Object::Type::Array) {
        for_elem_error(elems->typeOf());
        return make_error();
    }
    auto arr = std::dynamic_pointer_cast<Array>(elems);
    for (auto i : arr->elements) {
        innerEnv->set(_for->_var->toString(), i);
        _res = evaluate_one(_for->_body, innerEnv);
        if (_res->isReturn) {
            return _res;
        }
    }
    return std::make_shared<Null>();
}

std::shared_ptr<Object> Evaluator::evaluate_id(std::shared_ptr<IdentifierNode> _id, std::shared_ptr<Environment> env) {
    return env->get(_id->toString());
}

void Evaluator::bind_args(std::vector<std::shared_ptr<Object>> v, std::vector<std::string> name, std::string more, std::shared_ptr<Environment> env) {
    for (size_t i = 0; i < name.size(); i++) {
        env->set(name[i], v[i]);
    }
    if (more != "__null__") {
        auto arr = std::make_shared<Array>();
        for (size_t i = name.size(); i < v.size(); i++) {
            arr->elements.push_back(v[i]);
        }
        env->set(more, arr);
    }
}