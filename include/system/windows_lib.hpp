#pragma once

#include "object/nativefunction.hpp"
#include "vm_error.hpp"

#include <windows.h>
#include <filesystem>
#include <map>

std::map<std::string, HMODULE> loadeds;

typedef std::shared_ptr<Object> (*base_function_t)(Args);

std::shared_ptr<NativeFunction> get_ext_function(const std::string _library, const std::string _symbol) {
    HMODULE _module_handle;
    if (loadeds.count(_library)) {
        _module_handle = loadeds[_library];
    }
    else {
        _module_handle = LoadLibraryA(_library.c_str());
        if (_module_handle == NULL) {
            throw VMError("native:get", "Failed to load library: " + _library);
        }
        loadeds.insert({_library, _module_handle});
    }
    
    NFunc func = (base_function_t)GetProcAddress(_module_handle, _symbol.c_str());
    if (func == NULL) {
        throw VMError("native:get", "Failed to find symbol: " + _symbol + " (in library " + _library + ")");
    }
    return std::make_shared<NativeFunction>(func);
}

std::filesystem::path get_executable_path() {
	char szPath[1024];
	GetModuleFileNameA(NULL, szPath, sizeof(szPath)-1);
	return std::filesystem::path(szPath);
}