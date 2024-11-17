// The Next Generation of MPCC Project
// Copyright(C) 2009-2024 Mirekintoc Void
// MPCC Arkscene 1.0dev1

#include <iostream>
#include "program/program.hpp"

#include "plugins/plugin.hpp"

int main(int argc, char* argv[]) {
    Program program;
    program.loadLibrary(std::make_shared<Plugins::Base>());
    program.loadLibrary(std::make_shared<Plugins::DynamicLoad>());
    program.loadLibrary(std::make_shared<Plugins::IO>());
    program.loadLibrary(std::make_shared<Plugins::FileIO>());
    program.loadLibrary(std::make_shared<Plugins::Math>());
    if (argc < 2) {
        std::cerr << "At least an argument required\n";
        return 1;
    }
    int retval = 0;
    bool flag = false;
    if (argv[1][1] == '\0') {
        flag = true;
        switch (argv[1][0]) {
        case 'r':
            program.REPL();
            break;
        default:
            flag = false;
            break;
        }
    }
    if (!flag) {
        
    }
	return retval;
    return 0;
}