#pragma once

#include <string>
#include <memory>
#include <map>
#include <iostream>

#include "storage/binary.hpp"

void nf_not_found_error();
void global_lock_error();

class Object {
public:
    enum class Type{
        Integer, Float, String, Boolean, 
        Function, NativeFunction, Class,
        Instance, Array, Byte,
        Null, Error, NativeData,
        Enumerate, Unlimited // This is a placeholder.
    } type;
public:
    virtual std::string toString() const = 0;
    virtual std::shared_ptr<Object> copy() const = 0;
    virtual void assign(std::shared_ptr<Object> obj) = 0;
    virtual std::string idstr() const = 0;
    virtual void markMutable(const bool _is_mutable) {
        isMutable = _is_mutable;
    }
    static std::string typeOf(const Type _type) {
        auto it = typeNames.find(_type);
        if (it == typeNames.end()) {
            return "Undefined";
        }
        return it->second;
    }
    std::string typeOf() const {
        return typeOf(type);
    }
public:
    bool isMutable, isReturn;
    virtual void storeInto(std::ostream& os) const = 0;
    virtual void readFrom(std::istream& is) = 0;
    static std::shared_ptr<Object> get_object(std::istream& is);
public:
    Object(Type type) : type(type), isMutable(true), isReturn(false) {}
    static const std::map<Type, std::string> typeNames;
};