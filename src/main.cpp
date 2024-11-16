#pragma once

// The Next Generation of MPCC Project
// Copyright(C) 2009-2024 Mirekintoc Void
// MPCC Arkscene 1.0dev1

#include <iostream>
#include "program/program.hpp"

#include "plugins/plugin.hpp"

template<typename _Tp>
std::shared_ptr<Plugin> makePlugin() {
    return std::make_shared<_Tp>();
} 

int main(int argc, char* argv[]) {
    Program program;
    program.loadLibrary(makePlugin<Plugins::Base>());
    program.loadLibrary(makePlugin<Plugins::DynamicLoad>());
    program.loadLibrary(makePlugin<Plugins::IO>());
    program.loadLibrary(makePlugin<Plugins::FileIO>());
    program.loadLibrary(makePlugin<Plugins::Math>());
    if (argc < 2) {
        std::cerr << "At least two arguments required\n";
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
}