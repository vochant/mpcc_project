#pragma once

#include "object/object.hpp"
#include <fstream>

class File : public Object {
public:
    std::fstream fs;
public:
    File(std::string name, std::string mode);
    std::shared_ptr<Object> make_copy() override;
    std::string toString() override;
};