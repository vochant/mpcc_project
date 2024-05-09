#pragma once

#include "config.h"

#define VERSION_NUMBER 0
#define IS_DEBUG true

#include <random>
#include <iostream>

unsigned long long _cert;

size_t _line, _column;

bool hasError, doImport, assignBeforeDeclare, errorAsCrash;

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