#pragma once

#include "object/nativefunction.hpp"
#include "vm_error.hpp"

#include <dlfcn.h>
#include <map>

std::map<std::string, void*> loadeds;

typedef std::shared_ptr<Object> (*base_function_t)(Args);

std::shared_ptr<NativeFunction> get_ext_function(const std::string _library, const std::string _symbol) {
    void* _library_handle;
    if (loadeds.count(_library)) {
        _library_handle = loadeds[_library];
    }
    else {
        _library_handle = dlopen(_library.c_str(), RTLD_LAZY);
        if (_library_handle == NULL) {
            throw VMError("native:get", "Failed to load library: " + _library);
        }
        loadeds[_library] = _library_handle;
    }
    
    NFunc func = (base_function_t)dlsym(_library_handle, _symbol.c_str());
    if (func == NULL) {
        throw VMError("native:get", "Failed to find symbol: " + _symbol + " (in library " + _library + ")");
    }
    return std::make_shared<NativeFunction>(func);
}