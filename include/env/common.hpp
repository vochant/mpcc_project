#pragma once

#include "env/environment.hpp"
#include <map>

struct CommonEntry {
    bool isConst;
    std::shared_ptr<Object> value;
};

class CommonEnvironment : public Environment {
public:
    std::shared_ptr<Environment> parent;
    std::map<std::string, CommonEntry> entries;
public:
    std::shared_ptr<Object> get(std::string name) override;
    std::shared_ptr<Object> getUnder(std::string name, long long ident) override;
    void set(std::string name, std::shared_ptr<Object> obj) override;
    void makeConst(std::string name);
    bool has(std::string name) override;
    void remove(std::string name) override;
    CommonEnvironment(std::shared_ptr<Environment> parent = nullptr);
};