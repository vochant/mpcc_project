#pragma once

#include "object/object.hpp"

class Mark : public Object {
public:
    bool isEnum;
    std::string value;
public:
    Mark(bool isEnum, std::string value);
    std::shared_ptr<Object> make_copy() override;
    std::string toString() override;
};