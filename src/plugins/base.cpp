#include "plugins/plugin.hpp"
#include "vm_error.hpp"
#include "object/iterator.hpp"
#include <sstream>
#include "object/string.hpp"
#include "object/executable.hpp"
#include "vm/vm.hpp"

Plugins::Base::Base() {}

std::shared_ptr<Object> Array_Join(Args args) {
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
    for (auto& e : arr->value) {
        res->value.push_back(exec->call({e}));
    }
    return res;
}

std::shared_ptr<Object> Array_Foreach(Args args) {
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

void Plugins::Base::enable() {
    regist("join", Array_Join);
    regist("map", Array_Map);
    regist("foreach", Array_Foreach);
}