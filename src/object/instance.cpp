#include "object/instance.hpp"
#include "env/instbinder.hpp"

std::string Instance::toString() {
    auto _toString = innerBinder->getUnder("toString", -1);
    if (_toString->type == Object::Type::Executable) {
        auto _f = std::dynamic_pointer_cast<Executable>(_toString);
        return _f->call({})->toString();
    }
    return "[instance]";
}

std::shared_ptr<Object> Instance::make_copy() {
    auto _copy = std::make_shared<Instance>(belong);
    _copy->status = status;
    _copy->innerBinder = std::make_shared<InstanceBinder>(_copy.get(), std::dynamic_pointer_cast<InstanceBinder>(innerBinder)->parent);
    for (auto[k, v] : value_store) {
        _copy->value_store.insert({k, v->make_copy()});
    }
    return _copy;
}

Instance::Instance(MpcClass* belong) : Object(Type::Instance), belong(belong) {}