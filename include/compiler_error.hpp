#pragma once

#include <exception>
#include <string>

#include "lexer.hpp"

class CompilerError : public std::exception {
private:
    std::string desc;
public:
    const char* what() const noexcept override;
    CompilerError(std::string desc) noexcept;
};