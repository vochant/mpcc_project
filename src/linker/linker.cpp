#include "linker/linker.hpp"

Linker::Linker(std::vector<std::shared_ptr<Asm>> inner, size_t count) : inner(inner), count(count) {}

std::vector<std::shared_ptr<Asm>> Linker::link_program() {
    auto[code, pos] = flagsdump(inner, count);
    return linkjmps(code, pos);
}

std::pair<std::vector<std::shared_ptr<Asm>>, std::vector<size_t>> Linker::flagsdump(std::vector<std::shared_ptr<Asm>> raw, size_t flags) const {
    std::vector<std::shared_ptr<Asm>> res;
    std::vector<size_t> pos;
    pos.resize(flags);
    for (auto i : raw) {
        if (i->type == Asm::Type::FLAG) {
            pos[std::dynamic_pointer_cast<IntegerAsm>(i)->payload] = res.size();
        }
        else {
            res.push_back(i);
        }
    }
    res.push_back(std::make_shared<Asm>(Asm::Type::EOFV));
    return {res, pos};
}

std::vector<std::shared_ptr<Asm>> Linker::linkjmps(std::vector<std::shared_ptr<Asm>> raw, std::vector<size_t> dumps) const {
    for (auto& i : raw) {
        if (i->type == Asm::Type::JMP || i->type == Asm::Type::JT || i->type == Asm::Type::JF || i->type == Asm::Type::JTK || i->type == Asm::Type::JFK) {
            std::dynamic_pointer_cast<IntegerAsm>(i)->payload = dumps[std::dynamic_pointer_cast<IntegerAsm>(i)->payload];
        }
    }
    return raw;
}