#pragma once

#include "ast/program.hpp"
#include "object/environment.hpp"
#include "parser/parser.hpp"
#include "registry/base/plugin.hpp"

class Program {
public:
    std::shared_ptr<Environment> _outer, _inner;
    Program() : _outer(std::make_shared<Environment>()), _inner(std::make_shared<Environment>(_outer)) {}
public:
    void loadLibrary(std::shared_ptr<Plugin> _plg) {
        _plg->attach(_outer);
    }
    std::shared_ptr<Node> GetContent(const std::string filePath) const {
        std::string _input_src = "", _input_line;
        std::ifstream is(filePath);
        while (!is.eof()) {
            std::getline(is, _input_line);
            _input_src += "\n" + _input_line;
        }
        Parser parser(_input_src);
        return parser.parse_program();
    }
    std::shared_ptr<Node> Import(const std::string filePath) const {
        std::ifstream is(filePath.substr(filePath.length() - 6) == ".precf" ? filePath : (filePath + ".precf"), std::ios::in | std::ios::binary);
        if (is.good()) {
            auto _res = std::make_shared<ProgramNode>(nullptr);
            _res->readFrom(is);
            return _res;
        }
        else {
            return GetContent(filePath);
        }
    }
    void Evaluate(std::shared_ptr<Node> _program) {
        
    }
};