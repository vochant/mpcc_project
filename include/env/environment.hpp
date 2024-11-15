#pragma once

#include <string>
#include "object/object.hpp"

class Environment {
public:
    virtual std::shared_ptr<Object> get(std::string name) = 0;
    virtual std::shared_ptr<Object> getUnder(std::string name, long long ident = 0) = 0;
    virtual void set(std::string name, std::shared_ptr<Object> obj) = 0;
    virtual bool has(std::string name) = 0;
    virtual void remove(std::string name) = 0;
};