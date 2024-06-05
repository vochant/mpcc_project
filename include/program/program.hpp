#pragma once

#include "ast/program.hpp"
#include "object/environment.hpp"
#include "parser/parser.hpp"
#include "registry/base/plugin.hpp"
#include "evaluator/evaluator.hpp"

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
	std::shared_ptr<Node> GetBinary(const std::string filePath) const {
		std::ifstream is(filePath.substr(filePath.length() - 6) == ".precf" ? filePath : (filePath + ".precf"), std::ios::in | std::ios::binary);
		if (is.good()) {
            auto _res = std::make_shared<ProgramNode>(nullptr);
            _res->readFrom(is);
            return _res;
        }
		else return std::make_shared<ErrorNode>();
	}
    std::shared_ptr<Node> Import(const std::string filePath) const {
        auto content = GetBinary(filePath);
		if (content->type == Node::Type::Program) {
			return content;
		}
        else {
            return GetContent(filePath);
        }
    }
    void Evaluate(std::shared_ptr<ProgramNode> _program);
	void EvaluateOuter(std::shared_ptr<ProgramNode> _program);
	void Execute(std::string bin);
	void Run(std::string src);
	void REPL();
	std::string GetInformation(std::string info);
private:
	void shift(long long& where, std::string what) {
		for (size_t i = 0; i < what.length(); i++) {
			if (what.at(i) == '(' || what.at(i) == '[' || what.at(i) == '{') {
				where++;
			}
			if (what.at(i) == ')' || what.at(i) == ']' || what.at(i) == '}') {
				where--;
			}
		}
	}
};