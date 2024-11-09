#include "object/array.hpp"
#include "object/boolean.hpp"
#include "object/byte.hpp"
#include "object/farfunction.hpp"
#include "object/float.hpp"
#include "object/float32.hpp"
#include "object/function.hpp"
#include "object/integer.hpp"
#include "object/int8.hpp"
#include "object/int16.hpp"
#include "object/int32.hpp"
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
FarFunction::FarFunction(std::vector<std::shared_ptr<Asm>> code, std::shared_ptr<Environment> env) : Executable(ExecType::FarFunction), code(code), env(env) {}
Float::Float(double value) : Object(Type::Float), value(value) {}
Float32::Float32(float value) : Object(Type::Float32), value(value) {}
Function::Function(size_t position, std::shared_ptr<Environment> env) : Executable(ExecType::Function), position(position), env(env) {}
Integer::Integer(long long value) : Object(Type::Integer), value(value) {}
Int8::Int8(char value) : Object(Type::Int8), value(value) {}
Int16::Int16(short value) : Object(Type::Int16), value(value) {}
Int32::Int32(int value) : Object(Type::Int32), value(value) {}
NativeFunction::NativeFunction(NFunc func) : Executable(ExecType::NativeFunction), func(func) {}
Null::Null() : Object(Type::Null) {}
String::String(std::string value) : Object(Type::String) {}
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

std::shared_ptr<Object> FarFunction::make_copy() {
    return std::make_shared<FarFunction>(code, env);
}

std::shared_ptr<Object> Float::make_copy() {
    return std::make_shared<Float>(value);
}

std::shared_ptr<Object> Float32::make_copy() {
    return std::make_shared<Float32>(value);
}

std::shared_ptr<Object> Function::make_copy() {
    return std::make_shared<Function>(position, env);
}

std::shared_ptr<Object> Integer::make_copy() {
    return std::make_shared<Integer>(value);
}

std::shared_ptr<Object> Int8::make_copy() {
    return std::make_shared<Int8>(value);
}

std::shared_ptr<Object> Int16::make_copy() {
    return std::make_shared<Int16>(value);
}

std::shared_ptr<Object> Int32::make_copy() {
    return std::make_shared<Int32>(value);
}

std::shared_ptr<Object> NativeFunction::make_copy() {
    return std::make_shared<NativeFunction>(func);
}

std::shared_ptr<Object> String::make_copy() {
    return std::make_shared<String>(value);
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

std::string FarFunction::toString() {
    return "func[far asm]";
}

std::string Float::toString() {
    return std::to_string(value);
}

std::string Float32::toString() {
    return std::to_string(value);
}

std::string Function::toString() {
    return "func[local asm]";
}

std::string Int8::toString() {
    return std::to_string(short(value));
}

std::string Int16::toString() {
    return std::to_string(value);
}

std::string Int32::toString() {
    return std::to_string(value);
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

std::shared_ptr<Object> ConstructorProxy::call(std::vector<std::shared_ptr<Object>> args, VirtualMachine* vm) {
    if (!cls->constructors.count(args.size())) {
        throw VMError("ConProxy.call", "Unable to find constructor with " + std::to_string(args.size()) + " args");
    }
    return cls->runConstruct(args, vm);
}