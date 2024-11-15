#include "object/array.hpp"
#include "object/boolean.hpp"
#include "object/byte.hpp"
#include "object/float.hpp"
#include "object/function.hpp"
#include "object/integer.hpp"
#include "object/nativefunction.hpp"
#include "object/null.hpp"
#include "object/string.hpp"
#include "object/reference.hpp"
#include "object/lowref.hpp"
#include "object/conproxy.hpp"

#include "util.hpp"

#include <sstream>

// 程序代码分布：
// 代码段，以 ASM 序列存储在 VM.code 中，加载二进制得到的代码
// 远代码段，以 ASM 序列存储在其他的位置，后期创造
// 本地段，以本地二进制存储在其他位置，初始化生成或后期创造

Object::Object(Type type) : type(type) {}
Array::Array() : Object(Type::Array) {}
Boolean::Boolean(bool value) : Object(Type::Boolean), value(value) {}
Byte::Byte(unsigned char value) : Object(Type::Byte), value(value) {}
Executable::Executable(ExecType etype) : Object(Type::Executable), etype(etype) {}
Float::Float(double value) : Object(Type::Float), value(value) {}
Function::Function(std::shared_ptr<Node> inner, std::shared_ptr<Environment> env) : Executable(ExecType::Function), inner(inner), env(env) {}
Integer::Integer(long long value) : Object(Type::Integer), value(value) {}
NativeFunction::NativeFunction(NFunc func) : Executable(ExecType::NativeFunction), func(func) {}
Null::Null() : Object(Type::Null) {}
String::String(std::string value) : Object(Type::String), value(value), hash(genHash(value)) {}
String::String(std::string value, StringHash hash) : Object(Type::String), value(value), hash(hash) {}
Reference::Reference(std::shared_ptr<Object>* ptr) : Object(Type::Reference), ptr(ptr) {}
LowReference::LowReference(Object* ptr) : Object(Type::LowReference), ptr(ptr) {}
ConstructorProxy::ConstructorProxy(MpcClass* cls, Instance* inst) : Executable(ExecType::Placeholder), cls(cls), inst(inst) {}

std::shared_ptr<Object> Array::make_copy() {
    auto arr = std::make_shared<Array>();
    for (auto i : value) {
        arr->value.push_back(i->make_copy());
    }
    return arr;
}

std::shared_ptr<Object> Boolean::make_copy() {
    return std::make_shared<Boolean>(value);
}

std::shared_ptr<Object> Byte::make_copy() {
    return std::make_shared<Byte>(value);
}

std::shared_ptr<Object> Float::make_copy() {
    return std::make_shared<Float>(value);
}

std::shared_ptr<Object> Function::make_copy() {
    return std::make_shared<Function>(inner, env);
}

std::shared_ptr<Object> Integer::make_copy() {
    return std::make_shared<Integer>(value);
}

std::shared_ptr<Object> NativeFunction::make_copy() {
    return std::make_shared<NativeFunction>(func);
}

std::shared_ptr<Object> String::make_copy() {
    return std::make_shared<String>(value, hash);
}

std::shared_ptr<Object> Reference::make_copy() {
    return (*ptr)->make_copy();
}

std::shared_ptr<Object> LowReference::make_copy() {
    return ptr->make_copy();
}

std::shared_ptr<Object> ConstructorProxy::make_copy() {
    return std::make_shared<ConstructorProxy>(cls, inst);
}

std::string Array::toString() {
    std::stringstream ss;
    bool isFirst = true;
    for (auto i : value) {
        if (isFirst) {
            ss << '[';
            isFirst = false;
        }
        else {
            ss << ", ";
        }
        if (i->type != Type::String) ss << i->toString();
        else ss << escape(i->toString());
    }
    ss << ']';
    return ss.str();
}

std::string Boolean::toString() {
    return value ? "true" : "false";
}

std::string Byte::toString() {
    auto toHex = [](unsigned char ch) {
        return (ch < 10) ? (ch + '0') : (ch - 10 + 'A');
    };
    char str[3];
    str[0] = toHex(value >> 4);
    str[1] = toHex(value & 15);
    str[2] = 0;
    return str;
}

std::string Float::toString() {
    return std::to_string(value);
}
std::string Function::toString() {
    return "func[ast]";
}

std::string Integer::toString() {
    return std::to_string(value);
}

std::string NativeFunction::toString() {
    return "func[native]";
}

std::string Null::toString() {
    return "null";
}

std::string String::toString() {
    return value;
}

std::string Reference::toString() {
    return (*ptr)->toString();
}

std::string LowReference::toString() {
    return ptr->toString();
}

std::string ConstructorProxy::toString() {
    return "func[proxy]";
}

#include "vm_error.hpp"

std::shared_ptr<Object> ConstructorProxy::call(std::vector<std::shared_ptr<Object>> args) {
    if (!cls->constructors.count(args.size())) {
        throw VMError("ConProxy.call", "Unable to find constructor with " + std::to_string(args.size()) + " args");
    }
    return cls->runConstruct(inst->innerBinder, args);
}