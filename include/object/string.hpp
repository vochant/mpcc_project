#pragma once

#include "object/object.hpp"

class String : public Object {
public:
    std::string value;
public:
    String(std::string value);
    std::shared_ptr<Object> make_copy() override;
    std::string toString() override;
};