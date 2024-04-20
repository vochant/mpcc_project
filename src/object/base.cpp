#pragma once

#include "object/_all.hpp"

void nf_not_found_error() {
    err_begin();
    std::cerr << "Cannot find Native Function from a dynamic library.\n";
    std::cerr << "Make sure that you are using correct version of system library and the dynamic library is at correct location and using correct version.";
    err_end();
}

void global_lock_error() {
    err_begin();
    std::cerr << "Tried to create/remove/change an element in a global-locked environment(maybe class/instance).";
    err_end();
}

std::shared_ptr<Object> Object::get_object(std::istream& is) {
    Object::Type _sign = Object::Type(BinaryIn::read_byte(is));
    switch (_sign) {
    case Type::Boolean:
        return std::make_shared<Boolean>(is);
    case Type::Float:
        return std::make_shared<Float>(is);
    case Type::Integer:
        return std::make_shared<Integer>(is);
    case Type::Null:
        return std::make_shared<Null>();
    case Type::String:
        return std::make_shared<String>(is);
    default:
        return std::make_shared<Error>();
    }
}