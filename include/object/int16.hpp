#pragma once

#include "object/object.hpp"

class Int16 : public Object {
public:
    short value;
public:
    Int16(short value);
    std::shared_ptr<Object> make_copy() override;
    std::string toString() override;
};