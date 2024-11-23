#include "env/common.hpp"
#include "env/instbinder.hpp"
#include "object/null.hpp"
#include "object/reference.hpp"
#include "vm_error.hpp"

std::shared_ptr<Object> CommonEnvironment::get(std::string name) {
    if (entries.count(name)) {
        if (entries.at(name).isConst) {
            return entries.at(name).value;
        }
        else {
            return std::make_shared<Reference>(&entries.at(name).value);
        }
    }
    if (parent) {
        return parent->get(name);
    }
    return std::make_shared<Null>();
}

std::shared_ptr<Object> CommonEnvironment::getUnder(std::string name, long long ident) {
    if (entries.count(name)) {
        if (entries.at(name).isConst) {
            return entries.at(name).value;
        }
        else {
            return std::make_shared<Reference>(&entries.at(name).value);
        }
    }
    return std::make_shared<Null>();  
}

void CommonEnvironment::set(std::string name, std::shared_ptr<Object> obj) {
    if (entries.count(name)) {
        entries[name] = {false, obj};
    }
    else {
        entries.insert({name, {false, obj}});
    }
}

void CommonEnvironment::makeConst(std::string name) {
    entries[name].isConst = true;
}

bool CommonEnvironment::has(std::string name) {
    return entries.count(name);
}

void CommonEnvironment::remove(std::string name) {
    auto it = entries.find(name);
    if (it != entries.end()) entries.erase(it);
    else if (parent) parent->remove(name);
    else throw VMError("CommonEnv:remove", "Entry " + name + " not found");
}

CommonEnvironment::CommonEnvironment(std::shared_ptr<Environment> parent) : parent(parent) {}

#include "object/lowref.hpp"
#include "object/conproxy.hpp"
#include "object/integer.hpp"

std::shared_ptr<Object> InstanceBinder::get(std::string name) {
    if (name == "this" || name == "self") {
        return std::make_shared<LowReference>(obj);
    }
    if (name == "super") {
        if (!obj->status) {
            throw VMError("InstBinder:get", "Unable to find parent class: Not Found");
        }
        auto res = std::make_shared<ConstructorProxy>(obj->status, obj);
        obj->status = obj->status->parent ? obj->status->parent.get() : nullptr;
        return res;
    }
    if (name == "__index__") {
        return std::make_shared<Integer>(cls->id);
    }
    if (cls->getAL(name) == -3) {
        return parent->get(name);
    }
    if (obj->value_store.count(name)) {
        return std::make_shared<Reference>(&obj->value_store[name]);
    }
    return cls->getObject(name, obj->innerBinder);
}

std::shared_ptr<Object> InstanceBinder::getUnder(std::string name, long long ident) {
    if (cls->getAL(name) == -3) {
        return std::make_shared<Null>();
    }
    if (ident != -1 && cls->getAL(name) == -2) {
        if (!cls->ids.count(ident)) {
            return std::make_shared<Null>();
        }
    }
    else if (cls->getAL(name) != -1 && ident != -1 && cls->getAL(name) != ident) {
        return std::make_shared<Null>();
    }
    if (obj->value_store.count(name)) {
        return std::make_shared<Reference>(&obj->value_store[name]);
    }
    return cls->getObject(name, obj->innerBinder);
}

void InstanceBinder::set(std::string name, std::shared_ptr<Object> val) {
    if (!obj->value_store.count(name)) {
        throw VMError("InstBinder:set", "Unable to set value: class value not found: " + name);
    }
    obj->value_store[name] = val;
}

bool InstanceBinder::has(std::string name) {
    return cls->accessLookup.count(name);
}

void InstanceBinder::remove(std::string name) {
    if (cls->accessLookup.count(name)) throw VMError("InstBinder:remove", "Class members are not removable");
    else if (parent) parent->remove(name);
    else throw VMError("InstBinder:remove", "Entry " + name + " not found");
}

InstanceBinder::InstanceBinder(Instance* inst, std::shared_ptr<Environment> outer) : obj(inst), cls(inst->belong), parent(outer) {}