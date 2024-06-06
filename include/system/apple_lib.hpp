#pragma once

#ifdef _MACOS
#include <libproc.h>
#endif

#include <mach-o/dyld.h>

#include <filesystem>

std::filesystem::path get_executable_path() {
	char szPath[1024];
	size_t size = sizeof(path);
	if (_NSGetExecutablePath(szPath, &size) == 0) {
		return std::filesystem::path(szPath);
	}
	proc_pidpath(getpid(), szPath, sizeof(szPath));
    return szPath;
}