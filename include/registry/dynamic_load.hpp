#pragma once

#include "registry/base/plugin.hpp"
#include "object/string.hpp"

#include "system/current.hpp"

class DLoadPlugin : public Plugin {
public:
    DLoadPlugin() : Plugin() {}
    void attach(std::shared_ptr<Environment> env) const override {
        env->set("load", std::make_shared<NativeFunction>(load));
    }
private:
    static NativeFunction::resulttype load(NativeFunction::arglist args, Environment* env) {
        if (args.size() != 2) {
            return FormatError();
        }
        if (args[0]->type != Object::Type::String || args[1]->type != Object::Type::String) {
            return FormatError();
        }
        std::string _library = std::dynamic_pointer_cast<String>(args[0])->value;
        std::string _symbol = std::dynamic_pointer_cast<String>(args[1])->value;
        auto result = get_ext_function(_library, _symbol);
        if (result.second) {
            return FormatError();
        }
        return std::make_pair(NativeFunction::Result::OK, result.first);
    }
};