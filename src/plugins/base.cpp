#include "plugins/plugin.hpp"
#include "vm_error.hpp"
#include "object/iterator.hpp"
#include <sstream>
#include "object/string.hpp"
#include "object/executable.hpp"
#include "object/array.hpp"
#include "object/integer.hpp"
#include "vm/vm.hpp"
#include <cstdlib>
#include "util.hpp"
#include "vm/gct.hpp"
#include "object/instance.hpp"
#include "object/nativeobject.hpp"
#include "object/reference.hpp"

Plugins::Base::Base() {}

std::shared_ptr<Object> Array_Join(Args args) {
    plain(args);
    if (args.size() == 1) {
        args.push_back(std::make_shared<String>(""));
    }
    if (args.size() != 2 || args[1]->type != Object::Type::String) {
        throw VMError("(Base)Array_Join", "Incorrect Format");
    }
    if (args[0]->type == Object::Type::Iterator) {
        args[0] = std::dynamic_pointer_cast<Iterator>(args[0])->toArray();
    }
    if (args[0]->type != Object::Type::Array) {
        throw VMError("(Base)Array_Join", "Incorrect Format");
    }
    std::string str = std::dynamic_pointer_cast<String>(args[1])->value;
    auto arr = std::dynamic_pointer_cast<Array>(args[0]);
    std::stringstream ss;
    bool isFirst = true;
    for (auto& e : arr->value) {
        if (isFirst) isFirst = false;
        else ss << str;
        ss << e->toString();
    }
    return std::make_shared<String>(ss.str());
}

std::shared_ptr<Object> Array_Map(Args args) {
    plain(args);
    if (args.size() != 2 || args[1]->type != Object::Type::Executable) {
        throw VMError("(Base)Array_Map", "Incorrect Format");
    }
    if (args[0]->type == Object::Type::Iterator) {
        args[0] = std::dynamic_pointer_cast<Iterator>(args[0])->toArray();
    }
    if (args[0]->type != Object::Type::Array) {
        throw VMError("(Base)Array_Map", "Incorrect Format");
    }
    auto exec = std::dynamic_pointer_cast<Executable>(args[1]);
    auto arr = std::dynamic_pointer_cast<Array>(args[0]);
    auto res = std::make_shared<Array>();
    res->value.reserve(arr->value.size());
    for (auto& e : arr->value) {
        res->value.push_back(exec->call({e}));
    }
    return res;
}

std::shared_ptr<Object> Array_FlatMap(Args args) {
    plain(args);
    if (args.size() != 2 || args[1]->type != Object::Type::Executable) {
        throw VMError("(Base)Array_Map", "Incorrect Format");
    }
    if (args[0]->type == Object::Type::Iterator) {
        args[0] = std::dynamic_pointer_cast<Iterator>(args[0])->toArray();
    }
    if (args[0]->type != Object::Type::Array) {
        throw VMError("(Base)Array_Map", "Incorrect Format");
    }
    auto exec = std::dynamic_pointer_cast<Executable>(args[1]);
    auto arr = std::dynamic_pointer_cast<Array>(args[0]);
    auto res = std::make_shared<Array>();
    res->value.reserve(arr->value.size());
    for (auto& e : arr->value) {
        auto v = exec->call({e});
        if (v->type == Object::Type::Iterator) {
            v = std::dynamic_pointer_cast<Iterator>(v)->toArray();
        }
        if (v->type == Object::Type::Array) {
            auto cas = std::dynamic_pointer_cast<Array>(v);
            for (auto& i : cas->value) {
                res->value.push_back(i);
            }
        }
        else res->value.push_back(v);
    }
    return res;
}

std::shared_ptr<Object> Array_Foreach(Args args) {
    plain(args);
    if (args.size() != 2 || args[1]->type != Object::Type::Executable) {
        throw VMError("(Base)Array_Foreach", "Incorrect Format");
    }
    if (args[0]->type == Object::Type::Iterator) {
        args[0] = std::dynamic_pointer_cast<Iterator>(args[0])->toArray();
    }
    if (args[0]->type != Object::Type::Array) {
        throw VMError("(Base)Array_Foreach", "Incorrect Format");
    }
    auto exec = std::dynamic_pointer_cast<Executable>(args[1]);
    auto arr = std::dynamic_pointer_cast<Array>(args[0]);
    for (auto& e : arr->value) {
        exec->call({e});
    }
    return gVM->VNull;
}

std::shared_ptr<Object> Array_Find(Args args) {
    plain(args);
    if (args.size() != 2 || args[1]->type != Object::Type::Executable) {
        throw VMError("(Base)Array_Find", "Incorrect Format");
    }
    if (args[0]->type == Object::Type::Iterator) {
        args[0] = std::dynamic_pointer_cast<Iterator>(args[0])->toArray();
    }
    if (args[0]->type != Object::Type::Array) {
        throw VMError("(Base)Array_Find", "Incorrect Format");
    }
    auto arr = std::dynamic_pointer_cast<Array>(args[0]);
    auto exec = std::dynamic_pointer_cast<Executable>(args[1]);
    long long counter = 0;
    for (auto& e : arr->value) {
        if (gVM->isTrue(exec->call({e, std::make_shared<Integer>(counter)}))) return e;
        counter++;
    }
    return gVM->VNull;
}

std::shared_ptr<Object> Array_FindAt(Args args) {
    plain(args);
    if (args.size() != 2 || args[1]->type != Object::Type::Executable) {
        throw VMError("(Base)Array_FindAt", "Incorrect Format");
    }
    if (args[0]->type == Object::Type::Iterator) {
        args[0] = std::dynamic_pointer_cast<Iterator>(args[0])->toArray();
    }
    if (args[0]->type != Object::Type::Array) {
        throw VMError("(Base)Array_FindAt", "Incorrect Format");
    }
    auto arr = std::dynamic_pointer_cast<Array>(args[0]);
    auto exec = std::dynamic_pointer_cast<Executable>(args[1]);
    long long counter = 0;
    for (auto& e : arr->value) {
        if (gVM->isTrue(exec->call({e, std::make_shared<Integer>(counter)}))) return std::make_shared<Integer>(counter);
        counter++;
    }
    return gVM->VNull;
}

std::shared_ptr<Object> ArrStr_Reverse(Args args) {
    plain(args);
    if (args.size() != 1) {
        throw VMError("(Base)ArrStr_Reverse", "Incorrect Format");
    }
    if (args[0]->type == Object::Type::Iterator) {
        args[0] = std::dynamic_pointer_cast<Iterator>(args[0])->toArray();
    }
    if (args[0]->type == Object::Type::Array) {
        auto arr = std::dynamic_pointer_cast<Array>(args[0]);
        auto res = std::make_shared<Array>();
        res->value.reserve(arr->value.size());
        for (auto it = arr->value.rbegin(); it != arr->value.rend(); it++) {
            res->value.push_back(*it);
        }
        return res;
    }
    else if (args[0]->type == Object::Type::String) {
        auto str = std::dynamic_pointer_cast<String>(args[0])->value;
        std::stringstream ss;
        for (size_t i = str.length() - 1; i >= 0; i--) ss << str.at(i);
        return std::make_shared<String>(ss.str());
    }
    throw VMError("(Base)ArrStr_Reverse", "Incorrect Format");
}

std::shared_ptr<Object> String_Split(Args args) {
    plain(args);
    if (args.size() < 1 || args.size() > 2 || args[0]->type != Object::Type::String) {
        throw VMError("(Base)String_Split", "Incorrect Format");
    }
    if (args.size() == 2) {
        if (args[1]->type != Object::Type::String) {
            throw VMError("(Base)String_Split", "Incorrect Format");
        }
        if (std::dynamic_pointer_cast<String>(args[2])->value == "") args.pop_back();
    }
    auto& str = std::dynamic_pointer_cast<String>(args[0])->value;
    if (args.size() == 1) {
        // 分隔符是空字符串，每个字符一项
        auto res = std::make_shared<Array>();
        res->value.reserve(str.length());
        for (size_t i = 0; i < str.length(); i++) {
            res->value.push_back(std::make_shared<String>(str.at(i)));
        }
        return res;
    }
    else {
        // 使用 KMP 算法进行 split 操作
        auto& mode = std::dynamic_pointer_cast<String>(args[1])->value;
        auto res = std::make_shared<Array>();
        std::vector<size_t> fail;
        fail.reserve(str.length());
        fail.push_back(0);
        size_t mode_length = mode.length();
        for (size_t i = 1; i < mode_length; i++) {
            size_t p = fail[i - 1];
            while (p && mode[p] != mode[i]) p = fail[p - 1];
            fail.push_back(p + (mode[i] == mode[p]));
        }
        size_t e = 0, begin = 0;
        for (size_t i = 0; i < str.length(); i++) {
            while (e && mode[e] != str[i]) e = fail[e - 1];
            if (mode[e] == str[i]) e++;
            if (e == mode_length) {
                res->value.push_back(std::make_shared<String>(str.substr(begin, i + 1 - mode_length - begin)));
                begin = i + 1;
                e = 0;
            }
        }
        if (begin == str.length()) res->value.push_back(std::make_shared<String>(""));
        else res->value.push_back(std::make_shared<String>(str.substr(begin)));
        return res;
    }
}

std::shared_ptr<Object> String_Escape(Args args) {
    plain(args);
    if (args.size() != 1 || args[0]->type != Object::Type::String) {
        throw VMError("(Base)String_Escape", "Incorrect Format");
    }
    std::string common_esc = escape(args[0]->toString());
    return std::make_shared<String>(common_esc.substr(1, common_esc.length() - 2));
}

std::shared_ptr<Object> String_Unescape(Args args) {
    plain(args);
    if (args.size() != 1 || args[0]->type != Object::Type::String) {
        throw VMError("(Base)String_Unscape", "Incorrect Format");
    }
    std::string common_unesc = escape("'" + args[0]->toString() + "'");
    return std::make_shared<String>(common_unesc);
}

std::shared_ptr<Object> System_Exec(Args args) {
    if (!args.size()) {
        throw VMError("(Base)System_Exec", "Too few arguments");
    }
    std::stringstream ss;
    bool isFirst = true;
    for (auto& e : args) {
        if (isFirst) isFirst = false;
        else ss << ' ';
        ss << e->toString();
    }
    int retval = system(ss.str().c_str());
    return std::make_shared<Integer>(retval);
}

std::shared_ptr<Object> Typestr(Args args) {
    if (args.size() != 1) {
        throw VMError("(Base)Typestr", "Incorrect Format");
    }
    return std::make_shared<String>(gVM->getTypeString(args[0]));
}

std::shared_ptr<Object> Assert(Args args) {
    if (args.size() != 1) {
        throw VMError("(Base)Assert", "Incorrect Format");
    }
    bool a = gVM->isTrue(args[0]);
    if (!a) {
        throw VMError("(Base)Assert", "Assertion Failed");
    }
    return gVM->VNull;
}

std::shared_ptr<Object> BelongTo(Args args) {
    if (args.size() != 2 || args[1]->type != Object::Type::String) {
        throw VMError("(Base)Typestr", "Incorrect Format");
    }
    auto t = gVM->getTypeString(args[0]), et = args[1]->toString();
    if (t != et) {
        if (args[0]->type == Object::Type::Instance || GCT.count(et)) {
            auto id = GCT[et]->id;
            if (std::dynamic_pointer_cast<Instance>(args[0])->belong->ids.count(id)) {
                return gVM->True;
            }
        }
        return gVM->False;
    }
    return gVM->True;
}

std::shared_ptr<Object> Max(Args args) {
    if (!args.size()) {
        return gVM->VNull;
    }
    if (args.size() == 1) return args[1];
    auto v = gVM->isTrue(gVM->CalculateInfix(">", args[1], args[0], gVM->inner)) ? args[1] : args[0];
    for (size_t i = 2; i < args.size(); i++) {
        if (gVM->isTrue(gVM->CalculateInfix(">", args[i], v, gVM->inner))) {
            v = args[i];
        }
    }
    return v;
}

std::shared_ptr<Object> Min(Args args) {
    if (!args.size()) {
        return gVM->VNull;
    }
    if (args.size() == 1) return args[1];
    auto v = gVM->isTrue(gVM->CalculateInfix("<", args[1], args[0], gVM->inner)) ? args[1] : args[0];
    for (size_t i = 2; i < args.size(); i++) {
        if (gVM->isTrue(gVM->CalculateInfix("<", args[i], v, gVM->inner))) {
            v = args[i];
        }
    }
    return v;
}

std::shared_ptr<Object> String_Digit(Args args) {
    plain(args);
    if (args.size() != 1 || args[0]->type != Object::Type::String) {
        throw VMError("(Base)String_Digit", "Incorrect Format");
    }
    auto str = std::dynamic_pointer_cast<String>(args[0])->value;
    if (str.length() == 1) {
        return std::make_shared<Integer>(str[0]);
    }
    else {
        auto res = std::make_shared<Array>();
        res->value.reserve(str.length());
        for (size_t i = 0; i < str.length(); i++) {
            res->value.push_back(std::make_shared<Integer>(str[i]));
        }
        return res;
    }
}

std::shared_ptr<Object> String_Char(Args args) {
    plain(args);
    if (args.size() != 1) {
        throw VMError("(Base)String_Char", "Incorrect Format");
    }
    if (args[0]->type == Object::Type::Integer) {
        return std::make_shared<String>((char)(std::dynamic_pointer_cast<Integer>(args[0])->value));
    }
    if (args[0]->type != Object::Type::Iterator) {
        args[0] = std::dynamic_pointer_cast<Iterator>(args[0])->toArray();
    }
    if (args[0]->type != Object::Type::Array) {
        throw VMError("(Base)String_Char", "Incorrect Format");
    }
    auto& arr = std::dynamic_pointer_cast<Array>(args[0])->value;
    std::stringstream ss;
    for (auto& e : arr) {
        if (e->type != Object::Type::Integer) {
            throw VMError("(Base)String_Char", "Incorrect Format");
        }
        ss << (char)(std::dynamic_pointer_cast<Integer>(e)->value);
    }
    return std::make_shared<String>(ss.str());
}

std::shared_ptr<Object> To_String(Args args) {
    if (args.size() != 1) {
        throw VMError("(Base)To_String", "Incorrect Format");
    }
    return std::make_shared<String>(args[0]->toString());
}

std::shared_ptr<Object> ArrStr_Length(Args args) {
    if (args.size() != 1) {
        throw VMError("(Base)ArrStr_Length", "Incorrect Format");
    }
    plain(args);
    if (args[0]->type == Object::Type::String) {
        return std::make_shared<Integer>(std::dynamic_pointer_cast<String>(args[0])->value.length());
    }
    if (args[0]->type == Object::Type::Iterator) {
        args[0] = std::dynamic_pointer_cast<Iterator>(args[0])->toArray();
    }
    if (args[0]->type == Object::Type::Array) {
        return std::make_shared<Integer>(std::dynamic_pointer_cast<Array>(args[0])->value.size());
    }
    throw VMError("(Base)ArrStr_Length", "Incorrect Format");
}

std::shared_ptr<Object> Array_Push(Args args) {
    if (args.size() < 2 || args[0]->type != Object::Type::Reference) {
        throw VMError("(Base)Array_Push", "Incorrect Format");
    }
    args[0] = *(std::dynamic_pointer_cast<Reference>(args[0])->ptr);
    plain(args);
    if (args[0]->type != Object::Type::Array) {
        throw VMError("(Base)Array_Push", "Incorrect Format");
    }
    auto& arr = std::dynamic_pointer_cast<Array>(args[0])->value;
    for (size_t i = 1; i < args.size(); i++) {
        arr.push_back(args[i]);
    }
    return gVM->VNull;
}

std::shared_ptr<Object> Array_Pop(Args args) {
    if (args.size() == 1) {
        args.push_back(std::make_shared<Integer>(1));
    }
    if (args.size() != 2 || args[0]->type != Object::Type::Reference) {
        throw VMError("(Base)Array_Pop", "Incorrect Format");
    }
    args[0] = *(std::dynamic_pointer_cast<Reference>(args[0])->ptr);
    plain(args);
    if (args[0]->type != Object::Type::Array || args[1]->type != Object::Type::Integer) {
        throw VMError("(Base)Array_Pop", "Incorrect Format");
    }
    if (std::dynamic_pointer_cast<Integer>(args[1])->value < 0) {
        throw VMError("(Base)Array_Pop", "Incorrect Format");
    }
    auto p_count = std::min<size_t>(std::dynamic_pointer_cast<Integer>(args[1])->value, std::dynamic_pointer_cast<Array>(args[0])->value.size());
    auto& arr = std::dynamic_pointer_cast<Array>(args[0])->value;
    while (p_count--) arr.pop_back();
    return gVM->VNull;
}

void Plugins::Base::enable() {
    regist("join", Array_Join);
    regist("map", Array_Map);
    regist("flatMap", Array_FlatMap);
    regist("foreach", Array_Foreach);
    regist("find", Array_Find);
    regist("findAt", Array_FindAt);
    regist("push", Array_Push);
    regist("pop", Array_Pop);
    regist("reverse", ArrStr_Reverse);
    regist("len", ArrStr_Length);
    regist("split", String_Split);
    regist("escape", String_Escape);
    regist("unescape", String_Unescape);
    regist("digit", String_Digit);
    regist("char", String_Char);
    regist("system", System_Exec);
    regist("typestr", Typestr);
    regist("assert", Assert);
    regist("belongTo", BelongTo);
    regist("max", Max);
    regist("min", Min);
    regist("toString", To_String);

    auto _MPCC = std::make_shared<NativeObject>();
    _MPCC->set("version", std::make_shared<String>("2.2"));
    _MPCC->set("arch", std::make_shared<String>("Arkscene"));
    _MPCC->set("major_version", std::make_shared<Integer>(2));
    regist("MPCC", _MPCC);
}