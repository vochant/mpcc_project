#pragma once

// The Next Generation of MPCC Project
// Copyright(C) 2009-2024 Mirekintoc Void
// MPCC Arkscene 1.0dev1

// #include "program/program.hpp"
#include <iostream>
#include "parser/parser.hpp"
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
    ToAsmArgs args;
    size_t count = 0;
    args.isInRepeat = false;
    args.flagcount = &count;
    auto asmcode = res->to_asm(args);
    for (auto i : asmcode) {
        std::cout << asm2plain(i) << '\n';
    }
	// Program program;
	// pp.attach();
	// if (argc == 1) {
	// 	std::cout << program.GetInformation("about");
	// 	std::cout << program.GetInformation("help");
	// 	return 0;
	// }
	// std::string argv1 = argv[1];
	// if (argv1.length() == 1 && argv1 != "x") {

    // }
    // else if (argv1 == "x") {
    //     if (argc < 3) {
    //         std::cout << "Too less args\n";
    //         std::cout << program.GetInformation("about");
	// 	    std::cout << program.GetInformation("help");
    //     }
    // }
	// program.Execute(argv1);
	return 0;
}