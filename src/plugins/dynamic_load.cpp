#include "plugins/plugin.hpp"
#include "system/system.hpp"
#include "vm_error.hpp"
#include "object/string.hpp"

Plugins::DynamicLoad::DynamicLoad() {}

std::shared_ptr<Object> DL_LoadLibrary(Args args) {
    plain(args);
    if (args.size() != 2 || args[0]->type != Object::Type::String || args[1]->type != Object::Type::String) {
        throw VMError("(DynamicLoad)DL_LoadLibrary", "Incorrect Format");
    }
    return get_ext_function(std::dynamic_pointer_cast<String>(args[0])->value, std::dynamic_pointer_cast<String>(args[0])->value);
}

void Plugins::DynamicLoad::enable() {
    regist("load", DL_LoadLibrary);
}