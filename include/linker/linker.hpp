#pragma once

#include "asm/asm.hpp"
#include <vector>
#include <memory>

class Linker {
public:
    Linker();
    std::pair<std::vector<std::shared_ptr<Asm>>, std::vector<size_t>> flagsdump(std::vector<std::shared_ptr<Asm>> raw, size_t flags);
    std::vector<std::shared_ptr<Asm>> linkjmps(std::shared_ptr<std::shared_ptr<Asm>> raw, std::vector<size_t> dumps);
};