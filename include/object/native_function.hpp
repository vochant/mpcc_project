#pragma once

#include "object/base.hpp"
#include "object/null.hpp"

#include <functional>
#include <program/vm_options.hpp>

class NativeFunction : public Object {
public:
    enum class Result {
        OK, FORMAT_ERR, DATA_ERR, UNHANDLED_ERR
    };
    typedef std::shared_ptr<Object> valuetype;
    typedef std::vector<valuetype> arglist;
    typedef std::pair<Result, valuetype> resulttype;
    typedef std::function<resulttype(arglist)> native_func;
    native_func func;
    NativeFunction(native_func inner = [](arglist args)->resulttype {
        return std::make_pair(Result::OK, std::make_shared<Null>());
    }) : func(inner), Object(Object::Type::NativeFunction) {}
public:
    std::string toString() const override {
        if (COLORED_OUTPUTS) {
            return "\033[32m[Native Function]\033[0m";
        }
        else {
            return "[Native Function];";
        }
    }

    std::shared_ptr<Object> copy() const override {
        return std::make_shared<NativeFunction>(*this);
    }

    std::string idstr() const override {
        return "native_function";
    }

    void assign(std::shared_ptr<Object> value) override {
        bool _is_mutable = isMutable;
        *this = *std::dynamic_pointer_cast<NativeFunction>(value);
        isMutable = _is_mutable;
    }

    void storeInto(std::ostream& os) const override {
        BinaryOut::write_byte(os, char(Object::Type::NativeFunction));
    }
    void readFrom(std::istream& is) override {}
};