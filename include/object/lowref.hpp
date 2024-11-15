#pragma once

#include "object/object.hpp"

class LowReference : public Object {
public:
    Object* ptr;
public:
    std::string toString() override;
    std::shared_ptr<Object> make_copy() override;
    LowReference(Object* ptr);
};