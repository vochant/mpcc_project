#pragma once

#include "system/system.hpp"

#ifdef _WIN32
#include "system/windows_lib.hpp"
#endif

#ifdef __linux__
#include "system/linux_lib.hpp"
#endif

#ifdef __APPLE__
#include "system/apple_lib.hpp"
#endif

#ifdef _POSIX_VERSION
#include "system/posix_lib.hpp"
#endif