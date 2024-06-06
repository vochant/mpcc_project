#pragma once

#include "program/program.hpp"

void Program::Evaluate(std::shared_ptr<ProgramNode> _program) {
	Evaluator evaluator;
	evaluator.evaluate_program(_program, _inner.get());
}

void Program::EvaluateOuter(std::shared_ptr<ProgramNode> _program) {
	Evaluator evaluator;
	evaluator.evaluate_program(_program, _outer.get());
}

void Program::Execute(std::string bin) {
	auto obj = GetBinary(bin);
	if (obj->type != Node::Type::Program) {
		err_begin(true);
		std::cout << "Invalid Program.";
		err_end();
	}
	Evaluate(std::dynamic_pointer_cast<ProgramNode>(obj));
}

void Program::Run(std::string src) {
	auto obj = GetContent(src);
	if (obj->type != Node::Type::Program) {
		err_begin(true);
		std::cout << "Invalid Program.";
		err_end();
	}
	Evaluate(std::dynamic_pointer_cast<ProgramNode>(obj));
}

void Program::REPL() {
	Evaluator evaluator;
	std::cout << "MPCC Project 1.0\nRunning REPL Mode.\nType ':exit' to exit\n\n";
	while (true) {
		long long _shift = 0;
		std::string src = "", tmp;
		std::cout << "$ ";
		std::cin >> tmp;
		src += tmp + '\n';
		shift(_shift, tmp);
		while (_shift > 0) {
			std::cout << "> ";
			std::cin >> tmp;
			src += tmp + '\n';
			shift(_shift, tmp);
		}
		if (src == ":exit\n") {
			return;
		}
		Parser parser(src);
		auto prog = parser.parse_program();
		if (prog->type != Node::Type::Program) {
			err_begin(true);
			std::cout << "Invalid Program.";
			err_end();
			continue;
		}
		evaluator.evaluate_program(std::dynamic_pointer_cast<ProgramNode>(prog), _inner.get());
	}
}