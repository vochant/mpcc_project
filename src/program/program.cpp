#include "program/program.hpp"

#include <fstream>

void Program::loadLibrary(std::shared_ptr<Plugin> _plg) {
    _plg->attach(_outer);
}

int Program::Execute(std::shared_ptr<ProgramNode> _program) {
	return gVM->Execute(_program, gVM->inner);
}

int Program::ExecuteCode(std::string src, std::string from) {
    Parser parser(src, from);
    auto prog = parser.parse_program();
    return gVM->Execute(std::dynamic_pointer_cast<ProgramNode>(prog), gVM->inner);
}

int Program::ExecuteFile(std::string fileName) {
    std::fstream fs(fileName, std::ios::in);
    std::string src = "", tmp;
    while (std::getline(fs, tmp)) {
        src += tmp + "\n";
    }
    fs.close();
    Parser parser(src, fileName);
    auto prog = parser.parse_program();
    return gVM->Execute(std::dynamic_pointer_cast<ProgramNode>(prog), gVM->inner);
}

void Program::IncludeFile(std::string fileName) {
    ExecuteFile(fileName);
}

int Program::ExecuteOuter(std::shared_ptr<ProgramNode> _program) {
	return gVM->Execute(_program, gVM->outer);
}

void Program::REPL() {
	std::cout << "MPCC Project 2.3 Arkscene-Aivot\nRunning REPL Mode.\nType ':exit' to exit\n\n";
    std::cout << ">> ";
	while (true) {
        bool oed = false;
        try {
            std::string src = "";
            std::getline(std::cin, src);
            if (src == ":exit") break;
            Parser parser(src, "[stdin]", []()->std::string {
                std::string tmp;
                std::cout << ".. ";
                std::getline(std::cin, tmp);
                return tmp;
            });
            auto prog = parser.parse_program();
            gVM->Execute(std::dynamic_pointer_cast<ProgramNode>(prog), gVM->inner);
            std::cout << "\n<< ";
            if (gVM->lastObject->type == Object::Type::Reference) {
                gVM->lastObject = gVM->lastObject->make_copy();
            }
            if (gVM->lastObject->type == Object::Type::String) {
                std::cout << escape(gVM->lastObject->toString()) << "\n\n";
            }
            else {
                std::cout << gVM->lastObject->toString() << "\n\n";
            }
            std::cout << ">> ";
            oed = true;
            while (isspace(std::cin.peek())) std::cin.get();
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }
        if (!oed) std::cout << ">> ";
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