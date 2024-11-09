#pragma once

#include "object/nativefunction.hpp"
#include <filesystem>

std::shared_ptr<NativeFunction> get_ext_function(const std::string _library, const std::string _symbol);
std::filesystem::path get_executable_path();