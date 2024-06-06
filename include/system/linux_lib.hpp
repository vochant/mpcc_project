#pragma once

#include <unistd.h>
#include <filesystem>

std::filesystem::path get_executable_path() {
	char szPath[1024];
	readlink("/proc/self/exe", szPath, sizeof(szPath));
	return std::filesystem::path(szPath);
}