#pragma once

#include "object/object.hpp"

class Int32 : public Object {
public:
    int value;
public:
    Int32(int value);
    std::shared_ptr<Object> make_copy() override;
    std::string toString() override;
};