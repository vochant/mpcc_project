#pragma once

// The Next Generation of MPCC Project
// Copyright(C) 2009-2024 Mirekintoc Void
// MPCC Arkscene 1.0dev1

#include <iostream>
#include "parser/parser.hpp"
#include "compiler/compiler.hpp"
#include "linker/linker.hpp"
#include "convertor/asm2plain.hpp"

int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false);
    std::fstream fs("main.mpc", std::ios::in);
    std::string src = "", tmp;
    while (!fs.eof()) {
        std::getline(fs, tmp);
        src = src + "\n" + tmp;
    }
    Parser parser(src, "main.mpc");
    auto res = parser.parse_program();
    Compiler compiler(res);
    size_t count;
    auto asmcode = compiler.compile(count);
    Linker linker(asmcode, count);
    auto linked = linker.link_program();
    for (auto i : linked) {
        std::cout << asm2plain(i) << '\n';
    }
	return 0;
}