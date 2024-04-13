#pragma once

#include "object/native_function.hpp"

#include <dlfcn.h>

typedef NativeFunction::resulttype (*base_function_t)(NativeFunction::arglist);

std::pair<std::shared_ptr<NativeFunction>, bool> get_ext_function(const std::string _library, const std::string _symbol) {
	void* _library_handle = dlopen(_library.c_str(), RTLD_LAZY);
	if (_library_handle == NULL) {
		nf_not_found_error();
		return std::make_pair(std::make_shared<NativeFunction>(), true);
	}
	base_function_t func = dlsym(_library_handle, _symbol.c_str());
	if (func == NULL) {
		nf_not_found_error();
		return std::make_pair(std::make_shared<NativeFunction>(), true);
	}
	return std::make_pair(std::make_shared<NativeFunction>(func), false);
}