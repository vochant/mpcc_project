#pragma once

#include "object/object.hpp"

class Float32 : public Object {
public:
    float value;
public:
    Float32(float value);
    std::shared_ptr<Object> make_copy() override;
    std::string toString() override;
};