#pragma once

// GCT (Global Class Table)

#include <string>
#include <map>
#include <memory>
#include <set>

#include "object/memberf.hpp"

class MpcClass {
public:
    std::string parentName, name;
    std::shared_ptr<MpcClass> parent;
    std::map<std::string, std::shared_ptr<MemberFunction>> methods;
    std::map<std::string, std::shared_ptr<Object>> values, statics;
    std::map<size_t, std::shared_ptr<MemberFunction>> constructors;
    std::map<std::string, long long> accessLookup;
    std::set<std::string> ism;
    std::shared_ptr<MemberFunction> destructor;
    long long id;
    std::set<long long> ids;
public:
    long long getAL(std::string str);
    bool Ism(std::string str);
    std::shared_ptr<Object> getObject(std::string name, std::shared_ptr<Environment> binder);
private:
    std::shared_ptr<Object> getIStatic(std::string name);
    void copyInstance(std::map<std::string, std::shared_ptr<Object>>& store);
public:
    MpcClass();
    std::shared_ptr<Object> getStatic(std::string name, long long ident);
    std::shared_ptr<Object> makeInstance(std::vector<std::shared_ptr<Object>> args);
    std::shared_ptr<Object> runConstruct(std::shared_ptr<Environment> instr, std::vector<std::shared_ptr<Object>> args);
};

extern std::map<std::string, std::shared_ptr<MpcClass>> GCT;
extern long long gctIdCount;

class MpcEnum {
public:
    std::map<std::string, long long> entries;
    MpcEnum();
};

extern std::map<std::string, std::shared_ptr<MpcEnum>> GENT;