#pragma once

#include "object/base.hpp"
#include "program/vm_options.hpp"

class Error : public Object {
public:
    Error() : Object(Object::Type::Error) {}
public:
    std::string toString() const override {
        if (COLORED_OUTPUTS) {
            return "\033[31m[Error]\033[0m";
        }
        else {
            return "[Error]";
        }
    }

    std::shared_ptr<Object> copy() const override {
        return std::make_shared<Error>();
    }

    std::string idstr() const override {
        return "error";
    }

    void assign(std::shared_ptr<Object> value) override {
        // Nothing to assign
    }

    void storeInto(std::ostream& os) const override {
        BinaryOut::write_byte(os, char(Object::Type::Error));
    }
    void readFrom(std::istream& is) override {}
};