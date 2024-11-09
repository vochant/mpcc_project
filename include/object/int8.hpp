#pragma once

#include "object/object.hpp"

class Int8 : public Object {
public:
    char value;
public:
    Int8(char value);
    std::shared_ptr<Object> make_copy() override;
    std::string toString() override;
};