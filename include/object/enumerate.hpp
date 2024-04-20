#pragma once

#include "object/base.hpp"

#include <sstream>

class Enumerate : public Object {
public:
    std::map<std::string, unsigned long long> _enum;
    Enumerate(std::map<std::string, unsigned long long> _enum) : _enum(_enum), Object(Object::Type::Enumerate) {}
public:
    std::string toString() const override {
        std::stringstream ss;
        ss << "Enumerate{";
        for (auto it = _enum.begin(); it != _enum.end(); it++) {
            if (it != _enum.begin()) {
                ss << ",";
            }
            ss << it->first;
        }
        ss << "}";
        return ss.str();
    }

    std::shared_ptr<Object> copy() const override {
        return std::make_shared<Enumerate>(*this);
    }

    std::string idstr() const override {
        return "enumerate";
    }

    void assign(std::shared_ptr<Object> value) override {
        auto _cast = std::dynamic_pointer_cast<Enumerate>(value);
        *this = *_cast;
    }

    void storeInto(std::ostream& os) const override {
        BinaryOut::write_byte(os, char(Object::Type::Enumerate));
    }

    void readFrom(std::istream& is) override {}
};