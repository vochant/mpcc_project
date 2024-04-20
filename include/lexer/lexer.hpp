#pragma once

#include <fstream>
#include <string>
#include <algorithm>
#include <memory>
#include <set>

#include "program/vm_options.hpp"
#include "lexer/token.hpp"

void reserved_err(std::string _res);

class Lexer {
private:
    std::string _input;
    size_t _at;
private:
    void break_whitespace();
    std::string read_identifier();
    static Token::Type lookup(std::string str) {
        Token::Type result = Token::Type::Identifier;
        for (auto it = Token::typeNames.begin(); it != Token::typeNames.end(); it++) {
            if (it->second == str) {
                result = it->first;
            }
        }
        return result;
    }
    std::pair<std::string, Token::Type> read_number();
    std::string read_string();
public:
    Lexer(std::string _input) : _input(_input), _at(0) {}
    std::shared_ptr<Token> parseNext();
    static std::set<std::string> reserved;
};
