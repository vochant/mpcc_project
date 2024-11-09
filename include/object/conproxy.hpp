#pragma once

#include "object/executable.hpp"
#include "vm/gct.hpp"
#include "env/environment.hpp"
#include "object/instance.hpp"

class ConstructorProxy: public Executable {
public:
    MpcClass* cls;
    Instance* inst;
public:
    ConstructorProxy(MpcClass* cls, Instance* inst);
    std::shared_ptr<Object> call(std::vector<std::shared_ptr<Object>> args) override;
    std::shared_ptr<Object> make_copy() override;
    std::string toString() override;
};