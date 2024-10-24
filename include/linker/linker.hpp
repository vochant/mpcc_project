#pragma once

#include "asm/asm.hpp"
#include <vector>
#include <memory>

class Linker {
public:
    Linker(std::vector<std::shared_ptr<Asm>> inner, size_t count);
    std::vector<std::shared_ptr<Asm>> link_program();
private:
    std::vector<std::shared_ptr<Asm>> inner;
    size_t count;
    std::pair<std::vector<std::shared_ptr<Asm>>, std::vector<size_t>> flagsdump(std::vector<std::shared_ptr<Asm>> raw, size_t flags) const;
    std::vector<std::shared_ptr<Asm>> linkjmps(std::vector<std::shared_ptr<Asm>> raw, std::vector<size_t> dumps) const;
};