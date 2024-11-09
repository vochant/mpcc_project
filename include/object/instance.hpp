#pragma once

#include "object/object.hpp"
#include "vm/gct.hpp"
#include "env/environment.hpp"

class Instance : public Object {
public:
    MpcClass* belong, * status;
    std::map<std::string, std::shared_ptr<Object>> value_store;
    std::shared_ptr<Environment> innerBinder;
public:
    std::string toString() override;
    std::shared_ptr<Object> make_copy();
    Instance(MpcClass* belong);
};