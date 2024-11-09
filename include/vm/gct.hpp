#pragma once

// GCT (Global Class Table)

#include <string>
#include <map>
#include <memory>

#include "object/memberf.hpp"

class MpcClass {
public:
    std::string parentName;
    std::shared_ptr<MpcClass> parent;
    std::map<std::string, std::shared_ptr<MemberFunction>> methods;
    std::map<std::string, std::shared_ptr<Object>> values, statics;
    std::map<size_t, std::shared_ptr<MemberFunction>> constructors;
    std::map<std::string, long long> accessLookup;
    std::map<std::string, bool> ism;
    std::shared_ptr<MemberFunction> destructor;
    long long id;
public:
    MpcClass();
    std::shared_ptr<Object> makeInstance(std::vector<std::shared_ptr<Object>> args);
    std::shared_ptr<Object> runConstruct(std::vector<std::shared_ptr<Object>> args);
};

extern std::map<std::string, std::shared_ptr<MpcClass>> GCT;
extern long long gctIdCount;