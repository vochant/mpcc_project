#pragma once

#include "asm/asm.hpp"
#include <memory>

std::string asm2plain(std::shared_ptr<Asm> ptr);