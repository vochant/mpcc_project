#pragma once

#include "object/base.hpp"
#include <vector>
#include <memory>
#include <sstream>

class Array : public Object {
public:
    std::vector<std::shared_ptr<Object>> elements;
    Array() : Object(Object::Type::Array) {}
	Array(std::vector<std::shared_ptr<Object>> elements) : elements(elements), Object(Object::Type::Array) {}
public:
    std::string toString() const override {
        std::stringstream result;
        result << "[";
        bool isFirst = true;
        for (auto i : elements) {
            if (isFirst) {
                isFirst = false;
            }
            else {
                result << ", ";
            }
            result << i->toString();
        }
        result << "]";
        return result.str();
    }

    std::shared_ptr<Object> copy() const override {
        auto result = std::make_shared<Array>();
        for (auto i : elements) {
            result->elements.push_back(i->copy());
        }
        return result;
    }

    void assign(std::shared_ptr<Object> value) override {
        auto _cast = std::dynamic_pointer_cast<Array>(value);
        elements.clear();
        for (auto i : _cast->elements) {
            auto _each = i->copy();
            _each->markMutable(isMutable);
            elements.push_back(_each);
        }
    }
    
    void markMutable(bool _is_mutable) override {
        isMutable = _is_mutable;
        for (auto i : elements) {
            i->markMutable(_is_mutable);
        }
    }

    std::string idstr() const override {
        return "array";
    }

    void storeInto(std::ostream& os) const override {
        BinaryOut::write_byte(os, char(Object::Type::Array));
    }

    void readFrom(std::istream& is) override {}
};