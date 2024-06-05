#pragma once

#include "registry/base/selection.hpp"
#include "program/program.hpp"

int main(int argc, char* argv[]) {
	Program program;
	PluginProvider pp(&program);
	pp.attach();
	if (argc == 1) {
		std::cout << program.GetInformation("about");
		std::cout << program.GetInformation("help");
		return 0;
	}
	std::string argv1 = argv[1];
	if (argv1 == "-b") {
		if (argc != 3) {
			std::cout << "Unknown usage.\n";
			return 1;
		}
		program.Execute(argv[2]);
		return 0;
	}
	if (argv1 == "-a") {
		std::cout << program.GetInformation("about");
		return 0;
	}
	if (argv1 == "-v") {
		std::cout << program.GetInformation("version");
		return 0;
	}
	if (argv1 == "-h") {
		std::cout << program.GetInformation("help");
		return 0;
	}
	if (argv1 == "-c") {
		if (argc != 4) {
			std::cout << "Unknown usage.";
			return 1;
		}
		auto prog = program.GetContent(argv[2]);
		std::fstream fs(argv[3], std::ios::out | std::ios::binary);
		prog->storeInto(fs);
		return 0;
	}
	if (argc != 2) {
		std::cout << "Unknown usage.";
		return 1;
	}
	if (argv1 == "-r") {
		program.REPL();
		return 0;
	}
	program.Execute(argv[1]);
	return 0;
}