#pragma once

#include "object/native_function.hpp"

#include <windows.h>
#include <filesystem>

typedef NativeFunction::resulttype (*base_function_t)(NativeFunction::arglist);

std::pair<std::shared_ptr<NativeFunction>, bool> get_ext_function(const std::string _library, const std::string _symbol) {
    HMODULE _module_handle = LoadLibraryA(_library.c_str());
    if (_module_handle == NULL) {
        nf_not_found_error();
        return std::make_pair(std::make_shared<NativeFunction>(), true);
    }
    base_function_t func = (base_function_t)GetProcAddress(_module_handle, _symbol.c_str());
    if (func == NULL) {
        nf_not_found_error();
        return std::make_pair(std::make_shared<NativeFunction>(), true);
    }
    return std::make_pair(std::make_shared<NativeFunction>(func), false);
}

std::filesystem::path get_executable_path() {
	char szPath[1024];
	GetModuleFileNameA(NULL, szPath, sizeof(szPath)-1);
	return std::filesystem::path(szPath);
}