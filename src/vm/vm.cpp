#include "vm/vm.hpp"

#include "vm_error.hpp"

#include "object/array.hpp"
#include "object/boolean.hpp"
#include "object/byte.hpp"
#include "object/bytearray.hpp"
#include "object/conproxy.hpp"
#include "object/function.hpp"
#include "object/float.hpp"
#include "object/instance.hpp"
#include "object/integer.hpp"
#include "object/lowref.hpp"
#include "object/memberf.hpp"
#include "object/nativefunction.hpp"
#include "object/null.hpp"
#include "object/reference.hpp"
#include "object/string.hpp"
#include "object/file.hpp"
#include "object/mark.hpp"
#include "object/abit.hpp"

// temp
#include "object/commonobject.hpp"

#include "env/common.hpp"

#include <sstream>

template<typename _Tp>
std::shared_ptr<_Tp> As(std::shared_ptr<Node> obj) {
    return std::dynamic_pointer_cast<_Tp>(obj);
}

template<typename _Tp>
std::shared_ptr<_Tp> As(std::shared_ptr<Object> obj) {
    return std::dynamic_pointer_cast<_Tp>(obj);
}

int VirtualMachine::Execute(std::shared_ptr<ProgramNode> program, std::shared_ptr<Environment> env) {
    if (program->mainScope->type != Node::Type::Scope) {
        throw VMError("VM:Execute", "The main scope of the program must be a scope");
    }
    auto v = ExecuteScope(As<ScopeNode>(program->mainScope), env);
    if (v->type == Object::Type::Reference || v->type == Object::Type::LowReference) {
        v = v->make_copy();
    }
    if (v->type == Object::Type::Integer) {
        return As<Integer>(v)->value;
    }
    return 0;
}

std::shared_ptr<Object> VirtualMachine::ExecuteArray(std::shared_ptr<ArrayNode> arr, std::shared_ptr<Environment> env) {
    auto res = std::make_shared<Array>();
    for (auto& i : arr->elements) {
        auto v = ExecuteCommon(i, env);
        if (v->type == Object::Type::Reference) {
            v = v->make_copy();
        }
        res->value.push_back(v);
    }
    return res;
}

std::shared_ptr<Object> VirtualMachine::ExecuteAssign(std::shared_ptr<AssignNode> assign, std::shared_ptr<Environment> env) {
    auto l = ExecuteValue(assign->left, env), r = ExecuteCommon(assign->right, env);
    if (assign->_op != "=") {
        r = CalculateInfix(assign->_op.substr(0, assign->_op.length() - 1), l, r, env);
    }
    if (l->type != Object::Type::Reference) {
        throw VMError("VM:ExecuteAssign", "Assign to constant");
    }
    *(As<Reference>(l)->ptr) = r;
    return r;
}

std::shared_ptr<Object> VirtualMachine::ExecuteBoolean(std::shared_ptr<BooleanNode> b, std::shared_ptr<Environment> env) {
    return b->value ? True : False;
}

std::shared_ptr<Object> VirtualMachine::ExecuteBreakContinue(std::shared_ptr<BreakContinueNode> bc, std::shared_ptr<Environment> env) {
    if (bc->isContinue) state = State::CONTINUE;
    else state = State::BREAK;
    return VNull;
}

std::shared_ptr<Object> VirtualMachine::ExecuteCall(std::shared_ptr<CallNode> call, std::shared_ptr<Environment> env) {
    std::vector<std::shared_ptr<Object>> args;
    size_t ix = 0;
    for (size_t i = 0; i < call->args.size(); i++) {
        auto obj = ExecuteValue(call->args[i], env);
        if (ix < call->expands.size() && call->expands[ix] == i) {
            if (obj->type == Object::Type::Array) {
                auto cast = As<Array>(obj);
                for (auto& j : cast->value) {
                    args.push_back(j);
                }
            }
            else if (obj->type == Object::Type::Iterator) {
                auto cast = As<Iterator>(obj)->toArray();
                for (auto& j : cast->value) {
                    args.push_back(j);
                }
            }
            else {
                throw VMError("VM:ExecuteCall", "Expanding argument must be array or iterator");
            }
            ix++;
            continue;
        }
        args.push_back(obj);
    }
    auto callable = ExecuteCommon(call->to_run, env);
    if (callable->type != Object::Type::Executable) {
        throw VMError("VM:ExecuteCall", "Not Executable");
    }
    return As<Executable>(callable)->call(args);
}

std::shared_ptr<Object> VirtualMachine::ExecuteCFor(std::shared_ptr<CForNode> r, std::shared_ptr<Environment> env) {
    auto inner = std::make_shared<CommonEnvironment>(env);
    auto res = ExecuteStatement(r->_init, inner);
    if (state == State::RETURN) {
        return res;
    }
    if (state == State::BREAK) {
        state = State::COMMON;
        return VNull;
    }
    while (true) {
        if (r->_cond->type != Node::Type::Expr) {
            throw VMError("VM:ExecuteCFor", "Condition must be an expression");
        }
        auto v = isTrue(ExecuteExpr(As<ExprNode>(r->_cond), inner));
        if (!v) break;
        auto v2 = ExecuteStatement(r->_body, inner);
        if (state == State::RETURN) {
            return v2;
        }
        if (state == State::BREAK) {
            state = State::COMMON;
            break;
        }
        state = State::COMMON;
        auto v3 = ExecuteStatement(r->_next, inner);
        if (state == State::RETURN) {
            return v3;
        }
        if (state == State::BREAK) {
            state = State::COMMON;
            return VNull;
        }
    }
    return VNull;
}

std::shared_ptr<Object> VirtualMachine::ExecuteClass(std::shared_ptr<ClassNode> cls, std::shared_ptr<Environment> env) {
    auto _class = std::make_shared<MpcClass>();
    inner->set(cls->_name, std::make_shared<Mark>(false, cls->_name));
    _class->name = cls->_name;
    if (cls->_ext != ":") {
        _class->parentName = cls->_ext;
        if (!GCT.count(cls->_ext)) {
            throw VMError("VM:ExecuteClass", "Parent Class Not Found: " + cls->_ext);
        }
        _class->parent = GCT[cls->_ext];
        _class->ids = _class->parent->ids;
    }
    _class->ids.insert(_class->id);
    for (auto& i : cls->inner) {
        if (i->type == Node::Type::Constructor) {
            auto c = As<ConstructorNode>(i);
            _class->constructors.insert({c->argcount, As<MemberFunction>(ExecuteConstructor(c, env))});
        }
        else if (i->type == Node::Type::Destructor) {
            auto d = As<DestructorNode>(i);
            _class->destructor = As<MemberFunction>(ExecuteDestructor(d, env));
        }
        else if (i->type == Node::Type::ClassMember) {
            auto cm = As<ClassMember>(i);
            auto v = ExecuteCommon(cm->inner, env);
            auto al = _class->getAL(cm->name);
            if (cm->ctype == ClassMember::METHOD) {
                if (v->type != Object::Type::Executable) {
                    throw VMError("VM:ExecuteClass", "Unable to create method: Not Executable");
                }
                auto e = std::make_shared<MemberFunction>(As<Executable>(v));
                if (al != -3) {
                    if (al >= 0) {
                        throw VMError("VM:ExecuteClass", "Unable to overwrite private: " + cm->name + " in class " + cls->_name);
                    }
                    if ((-1 - al) != (cm->ac == ClassMember::PROTECTED)) {
                        throw VMError("VM:ExecuteClass", "Accessibility mismatch: " + cm->name + " in class " + cls->_name);
                    }
                    if (!_class->ism.count(cm->name)) {
                        throw VMError("VM:ExecuteClass", "Unable to create member " + cm->name + " for class " + cls->_name);
                    }
                    _class->methods[cm->name] = e;
                }
                else {
                    _class->accessLookup.insert({cm->name, (cm->ac == ClassMember::PRIVATE) ? _class->id : (-1 - (cm->ac == ClassMember::PROTECTED))});
                    _class->ism.insert(cm->name);
                    _class->methods.insert({cm->name, e});
                }
                if (cm->isFinal) _class->ism.erase(cm->name);
            }
            else if (cm->ctype == ClassMember::STATIC) {
                if (al != -3) {
                    throw VMError("VM:ExecuteClass", "Unable to create member " + cm->name + " for class " + cls->_name);
                }
                _class->accessLookup.insert({cm->name, (cm->ac == ClassMember::PRIVATE) ? _class->id : (-1 - (cm->ac == ClassMember::PROTECTED))});
                _class->statics.insert({cm->name, v});
            }
            else if (cm->ctype == ClassMember::VALUE) {
                if (al != -3) {
                    throw VMError("VM:ExecuteClass", "Unable to create member " + cm->name + " for class " + cls->_name);
                }
                _class->accessLookup.insert({cm->name, (cm->ac == ClassMember::PRIVATE) ? _class->id : (-1 - (cm->ac == ClassMember::PROTECTED))});
                _class->values.insert({cm->name, v});
            }
            else {
                throw VMError("VM:ExecuteClass", "Unable to create member " + cm->name + " for class " + cls->_name + " because of unknown type");
            }
        }
        else {
            throw VMError("VM:ExecuteClass", "Unknown node under class " + cls->_name);
        }
    }
    GCT.insert({cls->_name, _class});
    return VNull;
}

std::shared_ptr<Object> VirtualMachine::ExecuteConstructor(std::shared_ptr<ConstructorNode> cons, std::shared_ptr<Environment> env) {
    auto f = std::make_shared<Function>(cons->inner, nullptr);
    for (auto& i : cons->args) {
        f->args.push_back(i);
    }
    for (auto& i : cons->typechecks) {
        f->checks.insert(i);
    }
    return std::make_shared<MemberFunction>(f);
}

std::shared_ptr<Object> VirtualMachine::ExecuteCreation(std::shared_ptr<CreationNode> cr, std::shared_ptr<Environment> env) {
    auto cenv = std::dynamic_pointer_cast<CommonEnvironment>(cr->isGlobal ? gVM->inner : env);
    for (auto&[k, v] : cr->creations) {
        if (!cr->allowOverwrite && cenv->has(k)) {
            throw VMError("VM:ExecuteCreation", "Unable to overwrite variable " + k);
        }
        auto rv = ExecuteCommon(v, env);
        cenv->set(k, rv);
        if (cr->isConst) {
            cenv->makeConst(k);
        }
    }
    return VNull;
}

std::shared_ptr<Object> VirtualMachine::ExecuteDecorate(std::shared_ptr<DecorateNode> dec, std::shared_ptr<Environment> env) {
    auto d = ExecuteCommon(dec->decorator, env), v = ExecuteCommon(dec->inner, env);
    if (d->type != Object::Type::Executable) {
        throw VMError("VM:ExecuteDecorate", "Decorator must be executable");
    }
    auto de = As<Executable>(d);
    return de->call({v});
}

std::shared_ptr<Object> VirtualMachine::ExecuteDestructor(std::shared_ptr<DestructorNode> des, std::shared_ptr<Environment> env) {
    return std::make_shared<MemberFunction>(std::make_shared<Function>(des->inner, nullptr));
}

std::shared_ptr<Object> VirtualMachine::ExecuteEnum(std::shared_ptr<EnumerateNode> e, std::shared_ptr<Environment> env) {
    inner->set(e->_name, std::make_shared<Mark>(false, e->_name));
    auto _enum = std::make_shared<MpcEnum>();
    size_t index = 0;
    for (auto& i : e->items) {
        _enum->entries.insert({i, index++});
    }
    GENT.insert({e->_name, _enum});
    return VNull;
}

std::shared_ptr<Object> VirtualMachine::ExecuteError(std::shared_ptr<ErrorNode> err, std::shared_ptr<Environment> env) {
    throw VMError("VM:ExecuteError", "Hit user defined error node");
}

std::shared_ptr<Object> VirtualMachine::ExecuteExpr(std::shared_ptr<ExprNode> expr, std::shared_ptr<Environment> env) {
    switch (expr->inner->type) {
    case Node::Type::Assign:
        return ExecuteAssign(As<AssignNode>(expr->inner), env);
    case Node::Type::Infix:
        return ExecuteInfix(As<InfixNode>(expr->inner), env);
    case Node::Type::Call:
        return ExecuteCall(As<CallNode>(expr->inner), env);
    case Node::Type::Index:
        return ExecuteIndex(As<IndexNode>(expr->inner), env);
    case Node::Type::InDecrement:
        return ExecuteInDecrement(As<InDecrementNode>(expr->inner), env);
    case Node::Type::Ternary:
        return ExecuteTernary(As<TernaryNode>(expr->inner), env);
    case Node::Type::Object:
        return ExecuteObject(As<ObjectNode>(expr->inner), env);
    case Node::Type::Integer:
        return ExecuteInteger(As<IntegerNode>(expr->inner), env);
    case Node::Type::Float:
        return ExecuteFloat(As<FloatNode>(expr->inner), env);
    case Node::Type::Boolean:
        return ExecuteBoolean(As<BooleanNode>(expr->inner), env);
    case Node::Type::Identifier:
        return ExecuteIdentifier(As<IdentifierNode>(expr->inner), env);
    case Node::Type::Function:
        return ExecuteFunction(As<FunctionNode>(expr->inner), env);
    case Node::Type::Array:
        return ExecuteArray(As<ArrayNode>(expr->inner), env);
    case Node::Type::Null:
        return ExecuteNull(As<NullNode>(expr->inner), env);
    case Node::Type::String:
        return ExecuteString(As<StringNode>(expr->inner), env);
    case Node::Type::Group:
        return ExecuteGroup(As<GroupNode>(expr->inner), env);
    case Node::Type::Prefix:
        return ExecutePrefix(As<PrefixNode>(expr->inner), env);
    case Node::Type::Decorate:
        return ExecuteDecorate(As<DecorateNode>(expr->inner), env);
    case Node::Type::New:
        return ExecuteNew(As<NewNode>(expr->inner), env);
    case Node::Type::Expr:
        return ExecuteExpr(As<ExprNode>(expr->inner), env);
    default:
        throw VMError("VM:ExecuteExpr", "Unable to get value - unknown node type");
    }
}

std::shared_ptr<Object> VirtualMachine::ExecuteFloat(std::shared_ptr<FloatNode> fl, std::shared_ptr<Environment> env) {
    return std::make_shared<Float>(fl->value);
}

std::shared_ptr<Object> VirtualMachine::ExecuteFor(std::shared_ptr<ForNode> f, std::shared_ptr<Environment> env) {
    auto r = ExecuteCommon(f->_elem, env);
    if (r->type == Object::Type::Array) r = Array2Iterator(As<Array>(r));
    if (r->type != Object::Type::Iterator) {
        throw VMError("VM:ExecuteFor", "Element is not an iterator");
    }
    auto it = As<Iterator>(r);
    auto inner = std::make_shared<CommonEnvironment>(env);
    while (it->hasNext()) {
        inner->set(f->_var, it->next());
        auto v = ExecuteStatement(f->_body, inner);
        if (state == State::BREAK) {
            state = State::COMMON;
            return VNull;
        }
        if (state == State::RETURN) {
            return v;
        }
        state = State::COMMON;
    }
    return VNull;
}

std::shared_ptr<Object> VirtualMachine::ExecuteFunction(std::shared_ptr<FunctionNode> f, std::shared_ptr<Environment> env) {
    auto func = std::make_shared<Function>(f->inner, env);
    func->earg = f->moreName;
    for (auto& i : f->args) {
        func->args.push_back(i);
    }
    for (auto& i : f->typechecks) {
        func->checks.insert(i);
    }
    return func;
}

std::shared_ptr<Object> VirtualMachine::ExecuteGroup(std::shared_ptr<GroupNode> group, std::shared_ptr<Environment> env) {
    return ExecuteCommon(group->v, env);
}

std::shared_ptr<Object> VirtualMachine::ExecuteIdentifier(std::shared_ptr<IdentifierNode> id, std::shared_ptr<Environment> env) {
    return env->get(id->id);
}

std::shared_ptr<Object> VirtualMachine::ExecuteIf(std::shared_ptr<IfNode> r, std::shared_ptr<Environment> env) {
    bool cond = isTrue(ExecuteCommon(r->_cond, env));
    if (cond) {
        auto v = ExecuteStatement(r->_then, env);
        if (state == State::RETURN) {
            return v;
        }
    }
    else if (r->_else) {
        auto v = ExecuteStatement(r->_else, env);
        if (state == State::RETURN) {
            return v;
        }
    }
    return VNull;
}

std::shared_ptr<Object> VirtualMachine::ExecuteInDecrement(std::shared_ptr<InDecrementNode> idc, std::shared_ptr<Environment> env) {
    auto a = ExecuteValue(idc->body, env);
    if (a->type != Object::Type::Reference) {
        throw VMError("VM:ExecuteXcrement", "Cannot use increment/decrement on rval");
    }
    auto rv = *(As<Reference>(a)->ptr);
    if (rv->type == Object::Type::Instance) {
        auto name = std::string(idc->isAfter ? "operator" : "prefix") + (idc->isDecrement ? "--" : "++");
        auto f = As<Instance>(rv)->innerBinder->getUnder(name, getIdent(env));
        if (f->type != Object::Type::Executable) {
            throw VMError("VM:ExecuteXcrement", "Member function " + name + " not found in class " + As<Instance>(rv)->belong->name);
        }
        return As<Executable>(f)->call({});
    }
    if (rv->type == Object::Type::Integer) {
        if (idc->isAfter) {
            auto cpy = rv->make_copy();
            As<Integer>(rv)->value += idc->isDecrement ? -1 : 1;
            return cpy;
        }
        else {
            As<Integer>(rv)->value += idc->isDecrement ? -1 : 1;
            return rv->make_copy();
        }
    }
    else if (rv->type == Object::Type::Float) {
        if (idc->isAfter) {
            auto cpy = rv->make_copy();
            As<Float>(rv)->value += idc->isDecrement ? -1 : 1;
            return cpy;
        }
        else {
            As<Float>(rv)->value += idc->isDecrement ? -1 : 1;
            return rv->make_copy();
        }
    }
    else if (rv->type == Object::Type::Iterator) {
        if (idc->isDecrement) {
            throw VMError("VM:ExecuteXcrement", "Common Iterator does not support decreament");
        }
        if (idc->isAfter) {
            auto v = As<Iterator>(rv)->next();
            As<Iterator>(rv)->go();
            return v;
        }
        else {
            As<Iterator>(rv)->go();
            return As<Iterator>(rv)->next();
        }
    }
    throw VMError("VM:ExecuteXcrement", "Unknown object type");
}

std::shared_ptr<Object> VirtualMachine::ExecuteIndex(std::shared_ptr<IndexNode> ix, std::shared_ptr<Environment> env) {
    auto l = ExecuteValue(ix->left, env), inx = ExecuteCommon(ix->index, env);
    bool isr;
    if (isr = (l->type == Object::Type::Reference)) {
        l = *(As<Reference>(l)->ptr);
    }
    if (l->type == Object::Type::Instance) {
        auto f = As<Instance>(l)->innerBinder->getUnder("opreator[]", getIdent(env));
        if (f->type != Object::Type::Executable) {
            throw VMError("VM:ExecuteIndex", "Member function operator[] not found in class " + As<Instance>(l)->belong->name);
        }
        return As<Executable>(f)->call({inx});
    }
    if (inx->type == Object::Type::CommonObject) {
        return As<CommonObject>(inx)->get(inx->toString());
    }
    if (inx->type != Object::Type::Integer && inx->type != Object::Type::Array) {
        throw VMError("VM:ExecuteIndex", "Unsupported index type");
    }
    if (l->type == Object::Type::String) {
        if (inx->type == Object::Type::Integer) {
            auto sl = As<String>(l)->value.length();
            return std::make_shared<String>(As<String>(l)->value.at((As<Integer>(inx)->value % sl + sl) % sl));
        }
        else {
            std::string rstr = "";
            auto arr = As<Array>(inx);
            auto& str = As<String>(l)->value;
            auto sl = str.length();
            for (auto& e : arr->value) {
                if (e->type != Object::Type::Integer) {
                    throw VMError("VM:ExecuteIndex", "Unsupported index type");
                }
                rstr += str.at((As<Integer>(e)->value % sl + sl) % sl);
            }
            return std::make_shared<String>(rstr);
        }
    }
    else if (l->type == Object::Type::Array) {
        if (inx->type == Object::Type::Integer) {
            auto al = As<Array>(l)->value.size();
            if (isr) {
                return std::make_shared<Reference>(&(As<Array>(l)->value[(As<Integer>(l)->value % al + al) % al]));
            }
            else {
                return As<Array>(l)->value[(As<Integer>(l)->value % al + al) % al];
            }
        }
        else {
            auto res = std::make_shared<Array>();
            auto& arr = As<Array>(l)->value;
            auto idx = As<Array>(inx);
            auto al = arr.size();
            for (auto& e : idx->value) {
                if (e->type != Object::Type::Integer) {
                    throw VMError("VM:ExecuteIndex", "Unsupported index type");
                }
                res->value.push_back(arr[(As<Integer>(e)->value % al + al) % al]);
            }
            return res;
        }
    }
    throw VMError("VM:ExecuteIndex", "Unsupported element type");
}

std::shared_ptr<Object> VirtualMachine::ExecuteInfix(std::shared_ptr<InfixNode> calc, std::shared_ptr<Environment> env) {
    if (calc->_op == "||") {
        if (isTrue(ExecuteCommon(calc->left, env))) return True;
        if (isTrue(ExecuteCommon(calc->right, env))) return True;
        return False;
    }
    if (calc->_op == "&&") {
        if (!isTrue(ExecuteCommon(calc->left, env))) return False;
        if (!isTrue(ExecuteCommon(calc->right, env))) return False;
        return True;
    }
    if (calc->_op == "." || calc->_op == "::") {
        if (calc->right->type != Node::Type::Identifier) {
            throw VMError("VM:ExecuteInfix", "Getter right must be an identifier");
        }
        return CalculateGetter(ExecuteCommon(calc->left, env), As<IdentifierNode>(calc->right)->id, env, calc->_op == "::");
    }
    auto le = ExecuteCommon(calc->left, env), ri = ExecuteCommon(calc->right, env);
    return CalculateInfix(calc->_op, le, ri, env);
}

std::shared_ptr<Object> VirtualMachine::ExecuteInteger(std::shared_ptr<IntegerNode> iv, std::shared_ptr<Environment> env) {
    if (-32 <= iv->value && iv->value <= 512) {
        return IntegerConstants[iv->value + 32];
    }
    return std::make_shared<Integer>(iv->value);
}

std::shared_ptr<Object> VirtualMachine::ExecuteNew(std::shared_ptr<NewNode> nw, std::shared_ptr<Environment> env) {
    if (!GCT.count(nw->classname)) {
        throw VMError("VM:ExecuteNew", "Class Not Found: " + nw->classname);
    }
    std::vector<std::shared_ptr<Object>> args;
    size_t ix = 0;
    for (size_t i = 0; i < nw->args.size(); i++) {
        auto obj = ExecuteValue(nw->args[i], env);
        if (ix < nw->expands.size() && nw->expands[ix] == i) {
            if (obj->type == Object::Type::Array) {
                auto cast = As<Array>(obj);
                for (auto& j : cast->value) {
                    args.push_back(j);
                }
            }
            else if (obj->type == Object::Type::Iterator) {
                auto cast = As<Iterator>(obj)->toArray();
                for (auto& j : cast->value) {
                    args.push_back(j);
                }
            }
            else {
                throw VMError("VM:ExecuteNew", "Expanding argument must be array or iterator");
            }
            ix++;
            continue;
        }
        args.push_back(obj);
    }
    return GCT[nw->classname]->makeInstance(args);
}

std::shared_ptr<Object> VirtualMachine::ExecuteNull(std::shared_ptr<NullNode> nl, std::shared_ptr<Environment> env) {
    return VNull;
}

std::shared_ptr<Object> VirtualMachine::ExecuteObject(std::shared_ptr<ObjectNode> obj, std::shared_ptr<Environment> env) {
    return obj->obj;
}

std::shared_ptr<Object> VirtualMachine::ExecutePrefix(std::shared_ptr<PrefixNode> calc, std::shared_ptr<Environment> env) {
    auto obj = ExecuteCommon(calc->right, env);
    if (obj->type == Object::Type::Instance) {
        std::string fn = "prefix" + calc->_op;
        auto f = As<Instance>(obj)->innerBinder->getUnder(fn, getIdent(env));
        if (f->type != Object::Type::Executable) {
            throw VMError("VM:ExecutePrefix", "Member function " + fn + " not found in class " + As<Instance>(obj)->belong->name);
        }
        return As<Executable>(f)->call({});
    }
    if (calc->_op == "-") {
        if (obj->type == Object::Type::Integer) {
            return std::make_shared<Integer>(-(As<Integer>(obj)->value));
        }
        else if (obj->type == Object::Type::Float) {
            return std::make_shared<Float>(-(As<Float>(obj)->value));
        }
        throw VMError("VM:ExecutePrefix", "Unsupported type for operator-");
    }
    else if (calc->_op == "+") {
        if (obj->type == Object::Type::Integer || obj->type == Object::Type::Float) {
            return obj;
        }
        throw VMError("VM:ExecutePrefix", "Unsupported type for operator+");
    }
    else if (calc->_op == "!") {
        bool v = isTrue(obj);
        return v ? False : True;
    }
    else if (calc->_op == "~") {
        if (obj->type == Object::Type::Integer) {
            return std::make_shared<Integer>(~(As<Integer>(obj)->value));
        }
        else if (obj->type == Object::Type::Byte) {
            return std::make_shared<Byte>(~(As<Byte>(obj)->value));
        }
        else if (obj->type == Object::Type::ByteArray) {
            auto res = std::make_shared<ByteArray>(), cast = As<ByteArray>(obj);
            for (auto& e : cast->value) {
                res->value.push_back(~e);
            }
            return res;
        }
        throw VMError("VM:ExecutePrefix", "Unsupported type for operator~");
    }
    throw VMError("VM:ExecutePrefix", "Unknown operator " + calc->_op);
}

std::shared_ptr<Object> VirtualMachine::ExecuteRemove(std::shared_ptr<RemoveNode> rmv, std::shared_ptr<Environment> env) {
    env->remove(rmv->toRemove);
    return VNull;
}

std::shared_ptr<Object> VirtualMachine::ExecuteReturn(std::shared_ptr<ReturnNode> ret, std::shared_ptr<Environment> env) {
    state = State::RETURN;
    if (ret->isReference) {
        auto v = ExecuteValue(ret->obj, env);
        if (v->type != Object::Type::Reference) {
            throw VMError("VM:ExecuteReturn", "Unable to return a non-reference");
        }
        return v;
    }
    else {
        return ExecuteCommon(ret->obj, env);
    }
}

std::shared_ptr<Object> VirtualMachine::ExecuteScope(std::shared_ptr<ScopeNode> scope, std::shared_ptr<Environment> env) {
    auto inner = std::make_shared<CommonEnvironment>(env);
    for (auto& s : scope->statements) {
        auto v = ExecuteStatement(s, inner);
        if (state == State::RETURN) {
            return v;
        }
        if (state != State::COMMON) {
            return VNull;
        }
        lastObject = v;
    }
    return VNull;
}

std::shared_ptr<Object> VirtualMachine::ExecuteString(std::shared_ptr<StringNode> str, std::shared_ptr<Environment> env) {
    return std::make_shared<String>(str->value);
}

std::shared_ptr<Object> VirtualMachine::ExecuteTernary(std::shared_ptr<TernaryNode> tern, std::shared_ptr<Environment> env) {
    bool cond = isTrue(ExecuteCommon(tern->_cond, env));
    if (cond) return ExecuteCommon(tern->_if, env);
    else return ExecuteCommon(tern->_else, env);
}

std::shared_ptr<Object> VirtualMachine::ExecuteWhile(std::shared_ptr<WhileNode> wh, std::shared_ptr<Environment> env) {
    auto inner = std::make_shared<CommonEnvironment>(env);
    if (wh->isDoWhile) {
        do {
            auto v = ExecuteStatement(wh->_body, inner);
            if (state == State::BREAK) {
                state = State::COMMON;
                return VNull;
            }
            if (state == State::RETURN) {
                return v;
            }
            state = State::COMMON;
        }
        while (isTrue(ExecuteCommon(wh->_cond, inner)));
    }
    else {
        while (isTrue(ExecuteCommon(wh->_cond, inner))) {
            auto v = ExecuteStatement(wh->_body, inner);
            if (state == State::BREAK) {
                state = State::COMMON;
                return VNull;
            }
            if (state == State::RETURN) {
                return v;
            }
            state = State::COMMON;
        }
    }
    return VNull;
}

std::shared_ptr<Object> VirtualMachine::ExecuteValue(std::shared_ptr<Node> v, std::shared_ptr<Environment> env) {
    if (v->type == Node::Type::Expr) return ExecuteExpr(As<ExprNode>(v), env);
    else return ExecuteExpr(std::make_shared<ExprNode>(v), env);
}

std::shared_ptr<Object> VirtualMachine::ExecuteCommon(std::shared_ptr<Node> v, std::shared_ptr<Environment> env) {
    auto r = ExecuteValue(v, env);
    if (r->type == Object::Type::Reference || r->type == Object::Type::LowReference) {
        return r->make_copy();
    }
    return r;
}

std::shared_ptr<Object> VirtualMachine::ExecuteStatement(std::shared_ptr<Node> v, std::shared_ptr<Environment> env) {
    switch (v->type) {
    case Node::Type::BreakContinue:
        return ExecuteBreakContinue(As<BreakContinueNode>(v), env);
    case Node::Type::CFor:
        return ExecuteCFor(As<CForNode>(v), env);
    case Node::Type::Class:
        return ExecuteClass(As<ClassNode>(v), env);
    case Node::Type::Scope:
        return ExecuteScope(As<ScopeNode>(v), env);
    case Node::Type::Creation:
        return ExecuteCreation(As<CreationNode>(v), env);
    case Node::Type::If:
        return ExecuteIf(As<IfNode>(v), env);
    case Node::Type::Return:
        return ExecuteReturn(As<ReturnNode>(v), env);
    case Node::Type::For:
        return ExecuteFor(As<ForNode>(v), env);
    case Node::Type::While:
        return ExecuteWhile(As<WhileNode>(v), env);
    case Node::Type::Enumerate:
        return ExecuteEnum(As<EnumerateNode>(v), env);
    case Node::Type::Remove:
        return ExecuteRemove(As<RemoveNode>(v), env);
    case Node::Type::Expr:
        return ExecuteExpr(As<ExprNode>(v), env);
    default:
        throw VMError("VM:ExecuteStat", "Unknown Statement Type");
    }
}

bool VirtualMachine::isTrue(std::shared_ptr<Object> obj) {
    if (obj->type == Object::Type::Reference || obj->type == Object::Type::LowReference) {
        obj = obj->make_copy();
    }
    if (obj->type == Object::Type::Boolean) {
        return As<Boolean>(obj)->value;
    }
    if (obj->type == Object::Type::Integer) {
        return As<Integer>(obj)->value;
    }
    if (obj->type == Object::Type::Float) {
        return As<Float>(obj)->value;
    }
    if (obj->type == Object::Type::File) {
        return !(As<File>(obj)->fs->good());
    }
    if (obj->type == Object::Type::Instance) {
        auto f = As<Instance>(obj)->innerBinder->get("_cond");
        if (f->type == Object::Type::Executable) {
            auto r = As<Executable>(f)->call({});
            if (r->type != Object::Type::Boolean) {
                throw VMError("VM:isTrue", "_cond function must return true or false");
            }
            return As<Boolean>(r)->value;
        }
        return true;
    }
    if (obj->type == Object::Type::Iterator) {
        return As<Iterator>(obj)->hasNext();
    }
    if (obj->type == Object::Type::Byte) {
        return As<Byte>(obj)->value;
    }
    return true;
}

std::shared_ptr<Object> VirtualMachine::CalculateInfix(std::string op, std::shared_ptr<Object> a, std::shared_ptr<Object> b, std::shared_ptr<Environment> env) {
    if (op == "===" && a->type != b->type) return False;
    if (op == "!==" && a->type == b->type) return False;
    if (a->type == Object::Type::Instance) {
        std::string fn = "operator" + op;
        auto f = As<Instance>(a)->innerBinder->getUnder(fn, getIdent(env));
        if (f->type == Object::Type::Executable) {
            return As<Executable>(f)->call({b});
        }
    }
    if (a->type <= Object::Type::Float && b->type <= Object::Type::Float) {
        if (a->type == Object::Type::Boolean) a = std::make_shared<Byte>(isTrue(a));
        if (b->type == Object::Type::Boolean) b = std::make_shared<Byte>(isTrue(b));
        if (a->type == Object::Type::Null) a = std::make_shared<Byte>(0);
        if (b->type == Object::Type::Null) b = std::make_shared<Byte>(0);
        if (a->type == Object::Type::Byte && b->type == Object::Type::Byte) {
            return CalculateInteger<Byte>(op, As<Byte>(a), As<Byte>(b));
        }
        if (a->type == Object::Type::Byte) a = std::make_shared<Integer>(As<Byte>(a)->value);
        if (b->type == Object::Type::Byte) b = std::make_shared<Integer>(As<Byte>(b)->value);
        if (a->type == Object::Type::Integer && b->type == Object::Type::Integer) {
            return CalculateInteger<Integer>(op, As<Integer>(a), As<Integer>(b));
        }
        if (a->type == Object::Type::Integer) a = std::make_shared<Float>(As<Integer>(a)->value);
        if (b->type == Object::Type::Integer) b = std::make_shared<Float>(As<Integer>(b)->value);
        return CalculateFloat(op, As<Float>(a), As<Float>(b));
    }
    if (op == "==" || op == "!=" || op == ">" || op == "<" || op == ">=" || op == "<=" || op == "===" || op == "!==") {
        return CalculateRelationship(op, a, b);
    }
    if (op == "*" && (b->type == Object::Type::Byte || b->type == Object::Type::Integer || b->type == Object::Type::Boolean)) {
        if (b->type == Object::Type::Boolean) b = std::make_shared<Integer>(isTrue(b));
        if (b->type == Object::Type::Byte) b = std::make_shared<Integer>(As<Byte>(b)->value);
        long long rep = As<Integer>(b)->value;
        if ((a->type == Object::Type::Array || a->type == Object::Type::Iterator) && (b->type == Object::Type::Byte || b->type == Object::Type::Integer || b->type == Object::Type::Boolean)) {
            if (a->type == Object::Type::Iterator) a = As<Iterator>(a)->toArray();
            if (rep <= 0) return std::make_shared<Array>();
            auto res = std::make_shared<Array>(), cas = As<Array>(a);
            while (rep--) {
                res->value.insert(res->value.end(), cas->value.begin(), cas->value.end());
            }
            return res;
        }
        else if (a->type == Object::Type::String) {
            if (rep <= 0) return std::make_shared<String>("");
            std::stringstream ss;
            auto cas = As<String>(a);
            StringHash hash = {0, 0};
            while (rep--) {
                ss << cas->value;
                hash = concatHash(hash, cas->hash, cas->value.length());
            }
            return std::make_shared<String>(ss.str(), hash);
        }
        else if (a->type == Object::Type::ByteArray) {
            if (rep <= 0) return std::make_shared<ByteArray>();
            auto res = std::make_shared<ByteArray>(), cas = As<ByteArray>(a);
            while (rep--) {
                res->value.insert(res->value.end(), cas->value.begin(), cas->value.end());
            }
            return res;
        }
    }
    if (op != "+") {
        throw VMError("VM:CalculateInfix", "No that operator " + op + " between these objects");
    }
    if ((a->type == Object::Type::Array || a->type == Object::Type::Iterator) && (b->type == Object::Type::Array || b->type == Object::Type::Iterator)) {
        if (a->type == Object::Type::Iterator) a = As<Iterator>(a)->toArray();
        if (b->type == Object::Type::Iterator) b = As<Iterator>(b)->toArray();
        return CalculateArrStrExt(a, b);
    }
    return CalculatePlusToString(a, b);
}

std::shared_ptr<Object> VirtualMachine::CalculatePlusToString(std::shared_ptr<Object> a, std::shared_ptr<Object> b) {
    return std::make_shared<String>(a->toString() + b->toString());
}

template<typename _Tp>
_Tp VM_fastPow(_Tp base, long long sup) {
    _Tp res = 1;
    while (sup) {
        if (sup & 1) res *= base;
        base *= base;
        sup >>= 1;
    }
    return res;
}

template<typename _Tp>
std::shared_ptr<Object> VirtualMachine::CalculateInteger(std::string op, std::shared_ptr<_Tp> a, std::shared_ptr<_Tp> b) {
    auto av = a->value, bv = b->value;
    if (op == "+") return std::make_shared<_Tp>(av + bv);
    if (op == "-") return std::make_shared<_Tp>(av - bv);
    if (op == "*") return std::make_shared<_Tp>(av * bv);
    if (op == "/") return std::make_shared<_Tp>(av / bv);
    if (op == "%") return std::make_shared<_Tp>(av % bv);
    if (op == "^") return std::make_shared<_Tp>(av ^ bv);
    if (op == "&") return std::make_shared<_Tp>(av & bv);
    if (op == "|") return std::make_shared<_Tp>(av | bv);
    if (op == ">") return av > bv ? True : False;
    if (op == ">=") return av >= bv ? True : False;
    if (op == "<") return av < bv ? True : False;
    if (op == "<") return av <= bv ? True : False;
    if (op == "==" || op == "===") return av == bv ? True : False;
    if (op == "!=" || op == "!==") return av != bv ? True : False;
    if (op == "**") return std::make_shared<_Tp>(VM_fastPow(av, bv));
    if (op == "...") {
        auto res = std::make_shared<Array>();
        if (av <= bv) {
            while (av <= bv) {
                res->value.push_back(std::make_shared<_Tp>(av));
                av++;
            }
        }
        else {
            while (av >= bv) {
                res->value.push_back(std::make_shared<_Tp>(av));
                av--;
            }
        }
        return res;
    }
    throw VMError("VM:CalcInteger", "Unknown operator " + op);
}

std::shared_ptr<Object> VirtualMachine::CalculateFloat(std::string op, std::shared_ptr<Float> a, std::shared_ptr<Float> b) {
    double av = a->value, bv = b->value;
    if (op == "+") return std::make_shared<Float>(av + bv);
    if (op == "-") return std::make_shared<Float>(av - bv);
    if (op == "*") return std::make_shared<Float>(av * bv);
    if (op == "/") return std::make_shared<Float>(av / bv);
    if (op == "%") return std::make_shared<Float>(fmod(av, bv));
    if (op == ">") return av > bv ? True : False;
    if (op == ">=") return av >= bv ? True : False;
    if (op == "<") return av < bv ? True : False;
    if (op == "<") return av <= bv ? True : False;
    if (op == "==" || op == "===") return av == bv ? True : False;
    if (op == "!=" || op == "!==") return av != bv ? True : False;
    if (op == "**") return std::make_shared<Float>(VM_fastPow(av, bv));
    throw VMError("VM:CalcFloat", "Unknown operator " + op);
}

std::shared_ptr<Object> VirtualMachine::CalculateArrStrExt(std::shared_ptr<Object> a, std::shared_ptr<Object> b) {
    auto aa = As<Array>(a), ab = As<Array>(b), ac = std::make_shared<Array>();
    ac->value.insert(ac->value.end(), aa->value.begin(), aa->value.end());
    ac->value.insert(ac->value.end(), ab->value.begin(), ab->value.end());
    return ac;
}

std::shared_ptr<Object> VirtualMachine::CalculateRelationship(std::string op, std::shared_ptr<Object> a, std::shared_ptr<Object> b) {
    if (a->type == Object::Type::Instance && b->type == Object::Type::Instance) {
        long long ai = As<Instance>(a)->belong->id;
        long long bi = As<Instance>(b)->belong->id;
        if (op == "===" && ai != bi) return False;
        if (op == "!==" && ai == bi) return False;
    }
    if (op == "===") {
        if (a->type == Object::Type::Null) return True;
        if (a->type == Object::Type::String) return (As<String>(a)->hash) == (As<String>(b)->hash) ? True : False;
        if (a->type == Object::Type::Array) {
            auto ac = As<Array>(a), bc = As<Array>(b);
            if (ac->value.size() != bc->value.size()) return False;
            for (auto ait = ac->value.begin(), bit = bc->value.begin(); ait != ac->value.end(); ait++, bit++) {
                if (!isTrue(CalculateRelationship("===", *ait, *bit))) return False;
            }
            return True;
        }
        return False;
    }
    if (op == "!==") {
        if (a->type == Object::Type::Null) return False;
        if (a->type == Object::Type::String) return (As<String>(a)->hash) == (As<String>(b)->hash) ? False : True;
        if (a->type == Object::Type::Array) {
            auto ac = As<Array>(a), bc = As<Array>(b);
            if (ac->value.size() != bc->value.size()) return True;
            for (auto ait = ac->value.begin(), bit = bc->value.begin(); ait != ac->value.end(); ait++, bit++) {
                if (!isTrue(CalculateRelationship("===", *ait, *bit))) return True;
            }
            return False;
        }
        return True;
    }
    if (a->type == Object::Type::String && b->type == Object::Type::String) {
        if (op == "==") return (As<String>(a)->hash == As<String>(b)->hash) ? True : False;
        if (op == "!=") return (As<String>(a)->hash == As<String>(b)->hash) ? False : True;
    }
    std::string as = a->toString(), bs = b->toString();
    if (op == "==") return as == bs ? True : False;
    if (op == "!=") return as != bs ? True : False;
    if (op == ">=") return as >= bs ? True : False;
    if (op == "<=") return as <= bs ? True : False;
    if (op == ">") return as > bs ? True : False;
    if (op == "<") return as < bs ? True : False;
    throw VMError("VM:CalcRels", "Unknown operator: " + op);
}

std::shared_ptr<Object> VirtualMachine::CalculateGetter(std::shared_ptr<Object> a, std::string b, std::shared_ptr<Environment> env, bool isForced) {
    long long ident = isForced ? -1 : getIdent(env);
    if (a->type == Object::Type::Mark) {
        auto mrk = As<Mark>(a);
        if (mrk->isEnum) {
            auto it = GENT.find(mrk->value);
            if (it == GENT.end()) {
                return VNull;
            }
            auto v = it->second->entries.find(b);
            if (v == it->second->entries.end()) {
                return VNull;
            }
            return std::make_shared<Integer>(v->second);
        }
        else {
            auto it = GCT.find(mrk->value);
            if (it == GCT.end()) {
                return VNull;
            }
            return it->second->getStatic(b, getIdent(env));
        }
    }
    if (a->type == Object::Type::Instance) {
        return As<Instance>(a)->innerBinder->getUnder(b, ident);
    }
    auto f = env->get(b);
    if (f->type != Object::Type::Executable) {
        return VNull;
    }
    auto scop = std::make_shared<ScopeNode>();
    auto call = std::make_shared<CallNode>(std::make_shared<ObjectNode>(f));
    call->args.push_back(std::make_shared<ObjectNode>(a));
    call->args.push_back(std::make_shared<IdentifierNode>("__args"));
    call->expands.push_back(1);
    auto re = std::make_shared<ReturnNode>(call);
    scop->statements.push_back(re);
    auto func = std::make_shared<Function>(scop, env);
    func->earg = "__args";
    return func;
}

std::shared_ptr<Object> VirtualMachine::Array2Iterator(std::shared_ptr<Array> arr) {
    return std::make_shared<ArrayBasedIterator>(arr);
}

long long VirtualMachine::getIdent(std::shared_ptr<Environment> env) {
    auto item = env->get("__index__");
    if (item->type == Object::Type::Integer) {
        return As<Integer>(item)->value;
    }
    return 0;
}

VirtualMachine::VirtualMachine(std::shared_ptr<Environment> outer) {
    outer = outer;
    inner = std::make_shared<CommonEnvironment>(outer);
    state = State::COMMON;
    for (int i = -32; i <= 512; i++) IntegerConstants[i + 32] = std::make_shared<Integer>(i);
    True = std::make_shared<Boolean>(true);
    False = std::make_shared<Boolean>(false);
    VNull = std::make_shared<Null>();
    lastObject = VNull;
}

std::string VirtualMachine::getTypeString(std::shared_ptr<Object> obj) {
    if (obj->type == Object::Type::Reference || obj->type == Object::Type::LowReference) {
        obj = obj->make_copy();
    }
    if (obj->type == Object::Type::Array) return "array";
    else if (obj->type == Object::Type::Boolean) return "boolean";
    else if (obj->type == Object::Type::Byte) return "byte";
    else if (obj->type == Object::Type::ByteArray) return "ByteArray";
    else if (obj->type == Object::Type::CommonObject) return "Object";
    else if (obj->type == Object::Type::Executable) return "executable";
    else if (obj->type == Object::Type::File) return "File";
    else if (obj->type == Object::Type::Float) return "float";
    else if (obj->type == Object::Type::Instance) return As<Instance>(obj)->belong->name;
    else if (obj->type == Object::Type::Integer) return "int";
    else if (obj->type == Object::Type::Iterator) return "iterator";
    else if (obj->type == Object::Type::Map) return "Map";
    else if (obj->type == Object::Type::Mark) return "BaseType";
    else if (obj->type == Object::Type::Native) return "native";
    else if (obj->type == Object::Type::Null) return "nulltype";
    else if (obj->type == Object::Type::Segment) return "segment";
    else if (obj->type == Object::Type::Set) return "set";
    else if (obj->type == Object::Type::String) return "string";
    else return "unknown";
}