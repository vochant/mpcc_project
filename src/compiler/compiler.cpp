#include "compiler/compiler.hpp"

std::vector<std::shared_ptr<Asm>> Compiler::compile(size_t& flags) {
    ToAsmArgs args;
    flags = 0;
    args.isInRepeat = false;
    args.flagcount = &flags;
    auto res = inner->to_asm(args);
    return res;
}

Compiler::Compiler(std::shared_ptr<Node> inner) : inner(inner) {}
