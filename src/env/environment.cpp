#include "env/common.hpp"
#include "env/instbinder.hpp"
#include "object/null.hpp"
#include "object/reference.hpp"
#include "vm_error.hpp"
#include "eflags.hpp"

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
    if (_eflags.undefinedAsError) {
        throw VMError("CommEnv.get", "Entry " + name + " not found");
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
    if (_eflags.undefinedAsError) {
        throw VMError("CommEnv.getUnder", "Entry " + name + " not found");
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

CommonEnvironment::CommonEnvironment(std::shared_ptr<Environment> parent) : parent(parent) {}

#include "object/lowref.hpp"
#include "object/conproxy.hpp"
#include "object/integer.hpp"

std::shared_ptr<Object> InstanceBinder::get(std::string name) {
    if (name == "this" || name == "self") {
        return std::make_shared<LowReference>(obj);
    }
    if (name == "super") {
        auto res = std::make_shared<ConstructorProxy>(obj->status, obj);
        obj->status = obj->status->parent.get();
        return res;
    }
    if (name == "__index__") {
        return std::make_shared<Integer>(cls->id);
    }
    if (!cls->accessLookup.count(name)) {
        return parent->get(name);
    }
    if (obj->value_store.count(name)) {
        return std::make_shared<Reference>(&obj->value_store[name]);
    }
    if (cls->methods.count(name)) {
        return cls->methods[name]->apply(obj->innerBinder);
    }
    return cls->statics[name];
}

std::shared_ptr<Object> InstanceBinder::getUnder(std::string name, long long ident) {
    if (!cls->accessLookup.count(name)) {
        if (_eflags.undefinedAsError) {
            throw VMError("InstBinder.getUnder", "No such member: " + name);
        }
        return std::make_shared<Null>();
    }
    if (ident != -1 && cls->accessLookup[name] != ident) {
        if (_eflags.privAsError) {
            throw VMError("InstBinder.getUnder", "Member private: " + name);
        }
        return std::make_shared<Null>();
    }
    if (obj->value_store.count(name)) {
        return std::make_shared<Reference>(&obj->value_store[name]);
    }
    if (cls->methods.count(name)) {
        return cls->methods[name]->apply(obj->innerBinder);
    }
    return cls->statics[name];
}

void InstanceBinder::set(std::string name, std::shared_ptr<Object> val) {
    if (!obj->value_store.count(name)) {
        throw VMError("InstBinder.set", "Unable to set value: class value not found: " + name);
    }
    obj->value_store[name] = val;
}

InstanceBinder::InstanceBinder(Instance* inst, std::shared_ptr<Environment> outer) : obj(inst), cls(inst->belong), parent(outer) {}