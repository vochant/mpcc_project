#pragma once

#include "program/program.hpp"

void Program::loadLibrary(std::shared_ptr<Plugin> _plg) {
    _plg->attach(_outer);
}

void Program::Execute(std::shared_ptr<ProgramNode> _program) {
	gVM->Execute(_program, gVM->inner);
}

void Program::ExecuteCode(std::string src, std::string from) {
    Parser parser(src, from);
    auto prog = parser.parse_program();
    gVM->Execute(std::dynamic_pointer_cast<ProgramNode>(prog), gVM->inner);
}

void Program::ExecuteOuter(std::shared_ptr<ProgramNode> _program) {
	gVM->Execute(_program, gVM->outer);
}

void Program::REPL() {
	std::cout << "MPCC Project 2.2\nRunning REPL Mode.\nType ':exit' to exit\n\n";
	while (true) {
        try {
            long long _shift = 0;
            std::string src = "";
            std::cout << ">> ";
            std::cin >> src;
            Parser parser(src, "[stdin]", []()->std::string {
                std::string tmp;
                std::cout << ".. ";
                std::cin >> tmp;
                return tmp;
            });
            auto prog = parser.parse_program();
            gVM->Execute(std::dynamic_pointer_cast<ProgramNode>(prog), gVM->inner);
            if (gVM->lastObject->type == Object::Type::String) {
                std::cout << escape(gVM->lastObject->toString()) << '\n';
            }
            else {
                std::cout << gVM->lastObject->toString() << '\n';
            }
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }
	}
}

Program::Program() {
    std::ios::sync_with_stdio(false);
    _outer = std::make_shared<CommonEnvironment>();
    gVM = new VirtualMachine(_outer);
}

Program::~Program() {
    delete gVM;
}