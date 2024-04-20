#pragma once

#include "object/class.hpp"
#include "object/environment.hpp"

class Instance : public Object {
public:
    std::shared_ptr<Class> parentClass;
    std::shared_ptr<Environment> inner;
    Instance(std::shared_ptr<Class> parentClass) : parentClass(parentClass), Object(Object::Type::Class), inner(std::dynamic_pointer_cast<Environment>(parentClass->inner->copy())) {}
public:
    std::string toString() const override {
        return "Instance:" + inner->toString();
    }

    std::shared_ptr<Object> copy() const override {
        std::shared_ptr<Instance> result = std::make_shared<Instance>(parentClass);
        result->inner = std::dynamic_pointer_cast<Environment>(inner->copy());
        return result;
    }

    std::string idstr() const override {
        return parentClass->idstr();
    }

    void assign(std::shared_ptr<Object> obj) override {
        auto _cast = std::dynamic_pointer_cast<Instance>(obj);
        parentClass = _cast->parentClass;
        inner = std::dynamic_pointer_cast<Environment>(_cast->inner->copy());
    }

    void storeInto(std::ostream& os) const override {
        BinaryOut::write_byte(os, char(Object::Type::Instance));
    }

    void readFrom(std::istream& is) override {}
};