#pragma once

#include "config.h"

#define VERSION_NUMBER 0
#define COLORED_OUTPUTS true
#define IS_DEBUG true
#define DEFAULT_I18N zh_CN
#define CONSOLE_BUILD
#define doImport true

#include <random>
#include <iostream>
#include <set>

unsigned long long _cert;

size_t _line, _column;

bool hasError, assignBeforeDeclare, errorAsCrash;

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

std::set<std::string> implist;