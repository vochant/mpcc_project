#pragma once

#include "object/base.hpp"

class Integer : public Object {
public:
    long long value;
    Integer(long long value = 0) : value(value), Object(Object::Type::Integer) {}
    Integer(std::istream& is) : Object(Object::Type::Integer) {
        readFrom(is);
    }
public:
    std::string toString() const override {
        return toString();
    }

    std::shared_ptr<Object> copy() const override {
        return std::make_shared<Integer>(*this);
    }

    std::string idstr() const override {
        return "integer";
    }

    void assign(std::shared_ptr<Object> value) override {
        bool _is_mutable = isMutable;
        *this = *std::dynamic_pointer_cast<Integer>(value);
        isMutable = _is_mutable;
    }

    void storeInto(std::ostream& os) const override {
        BinaryOut::write_byte(os, char(Object::Type::Integer));
        BinaryOut::write_data(os, value);
    }

    void readFrom(std::istream& is) override {
        value = BinaryIn::read_data<long long>(is);
    }
};