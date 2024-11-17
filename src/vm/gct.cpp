#include "vm/gct.hpp"

long long MpcClass::getAL(std::string str) {
    auto it = accessLookup.find(str);
    if (it != accessLookup.end()) {
        return it->second;
    }
    if (parent) return parent->getAL(str);
    return -3;
}

bool MpcClass::Ism(std::string str) {
    if (accessLookup.count(str)) {
        return ism.count(str);
    }
    if (parent) return parent->Ism(str);
    return false;
}

MpcClass::MpcClass() {
    parentName = name = "";
    parent = nullptr;
    destructor = nullptr;
    id = ++gctIdCount;
}

#include "vm/vm.hpp"
#include "object/instance.hpp"
#include "env/instbinder.hpp"
#include "vm_error.hpp"

std::shared_ptr<Object> MpcClass::getIStatic(std::string name) {
    if (accessLookup.count(name)) {
        auto it = statics.find(name);
        if (it == statics.end()) return gVM->VNull;
        return it->second;
    }
    if (parent) return parent->getIStatic(name);
    return gVM->VNull;
}

std::shared_ptr<Object> MpcClass::getStatic(std::string name, long long ident) {
    auto ac = getAL(name);
    if (ac == -3) return gVM->VNull;
    if (ident != -1) {
        if (ac == -2) if (!ids.count(ident)) return gVM->VNull;
        if (ac != -1) if (ident != ac) return gVM->VNull;
    }
    return getIStatic(name);
}

void MpcClass::copyInstance(std::map<std::string, std::shared_ptr<Object>>& store) {
    for (auto& p : values) {
        store.insert(p);
    }
    if (parent) parent->copyInstance(store);
}

std::shared_ptr<Object> MpcClass::makeInstance(std::vector<std::shared_ptr<Object>> args) {
    auto inst = std::make_shared<Instance>(this);
    inst->status = parent.get();
    copyInstance(inst->value_store);
    inst->innerBinder = std::make_shared<InstanceBinder>(inst.get(), gVM->inner);
    auto cons = constructors.find(args.size());
    if (cons != constructors.end()) {
        cons->second->apply(inst->innerBinder, id)->call(args);
    }
    else if (args.size()) {
        throw VMError("MpcClass:makeInstance", "Class " + name + ": no such constructor with " + std::to_string(args.size()) + " args");
    }
    return inst;
}

std::shared_ptr<Object> MpcClass::runConstruct(std::shared_ptr<Environment> instr, std::vector<std::shared_ptr<Object>> args) {
    auto cons = constructors.find(args.size());
    if (cons != constructors.end()) {
        cons->second->apply(instr, id)->call(args);
    }
    else {
        throw VMError("MpcClass:runConstruct", "Class " + name + ": no such constructor with " + std::to_string(args.size()) + " args");
    }
    return gVM->VNull;
}

std::shared_ptr<Object> MpcClass::getObject(std::string name, std::shared_ptr<Environment> binder) {
    if (methods.count(name)) {
        return methods[name]->apply(binder, id);
    }
    if (statics.count(name)) {
        return statics[name];
    }
    return parent->getObject(name, binder);
}

MpcEnum::MpcEnum() {}

std::map<std::string, std::shared_ptr<MpcClass>> GCT;
long long gctIdCount;
std::map<std::string, std::shared_ptr<MpcEnum>> GENT;