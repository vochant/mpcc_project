#pragma once

#include "config.h"

#define VERSION_NUMBER 0
#define IS_DEBUG true
#define CONSOLE_BUILD
const bool doImport = true

std::string _i18n_name = "zh_CN";

#include <random>
#include <iostream>
#include <set>
#include <filesystem>

unsigned long long _cert;

size_t _line, _column;

bool hasError;

void err_begin(bool _abstract = false) {
    hasError = true;
    if (COLORED_OUTPUTS) {
        std::cerr << "\033[31m";
    }
    std::cerr << "[ERROR]";
    if (!_abstract) {
        std::cerr << "[" << _line << ":" << _column << "]";
    }
    else {
        std::cerr << "[ASTVM]";
    }
    std::cerr << " ";
}

void err_end() {
    if (COLORED_OUTPUTS) {
        std::cerr << "\033[0m";
    }
    std::cerr << "\n";
}

std::set<std::string> implist; // Imported files

std::filesystem::path respath;