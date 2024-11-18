// The Next Generation of MPCC Project
// Copyright(C) 2009-2024 Mirekintoc Void
// MPCC Arkscene-Aivot 2.3

#include <iostream>
#include "program/program.hpp"

#include "plugins/plugin.hpp"

#ifdef _WIN32
# include <windows.h>
#endif

int main(int argc, char* argv[]) {
    #ifdef _WIN32
    auto prevICP = GetConsoleCP(), prevOCP = GetConsoleOutputCP();
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    #endif

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
    if (argv[1][0] == '-') {
        if (argc != 2) {
            std::cerr << "Incorrect Format\n";
            return 2;
        }
        std::string v = argv[1];
        if (v == "-v" || v == "--version") {
            std::cout << "MPCC Project 2.3 Arkscene-Aivot\nCopyright(C) 2019-2024 Minftis Dev\nCopyright(C) 2009-2024 Mirekintoc Void\n\n";
            return 0;
        }
        if (v == "-h" || v == "--help") {
            std::cout << "Usage: mpcc [mode]\n\n"
                         "Modes:\n\n"
                         "    -v or --version     Get the version info.\n"
                         "    -h or --help        Get the help document.\n"
                         "    e <filename>        Execute a file.\n"
                         "    r                   REPL (Read-Evaluate-Print Loop) mode.\n"
                         "    <filename>          Same as 'e', execute a file.\n"
                         "    x <filename>        Execute a binary file.\n"
                         "    c <in> <out>        Convert a file into binary format.\n"
                         "    s <in> <out>        Convert a file into a token list.\n"
                         "    t <in> <out>        Convert a file into a AST json.\n\n";
            return 0;
        }
        std::cerr << "Incorrect Format.\n";
        return 2;
    }
    if (argv[1][1] == '\0') {
        flag = true;
        switch (argv[1][0]) {
        case 'r':
            program.REPL();
            break;
        case 'e':
            if (argc != 3) {
                std::cerr << "Incorrect Format\n";
                return 2;
            }
            retval = program.ExecuteFile(argv[2]);
        default:
            flag = false;
            break;
        }
    }
    if (!flag) {
        retval = program.ExecuteFile(argv[1]);
    }

    #ifdef _WIN32
    SetConsoleCP(prevICP);
    SetConsoleOutputCP(prevOCP);
    #endif
	return retval;
}