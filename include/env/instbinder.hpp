#pragma once

#include "env/environment.hpp"
#include "object/instance.hpp"

class InstanceBinder : public Environment {
public:
    Instance* obj;
    MpcClass* cls;
    std::shared_ptr<Environment> parent;
public:
    std::shared_ptr<Object> get(std::string name) override;
    std::shared_ptr<Object> getUnder(std::string name, long long ident) override;
    void set(std::string name, std::shared_ptr<Object> obj) override;
    bool has(std::string name) override;
    void remove(std::string name) override;
    InstanceBinder(Instance* inst, std::shared_ptr<Environment> outer);
};