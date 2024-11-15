#pragma once

#include <vector>
#include <sstream>

#include "ast/base/node.hpp"

// function-or-class(arg1, arg2, ...)

class NewNode : public Node {
public:
    std::string classname;
    std::vector<std::shared_ptr<Node>> args;
	std::vector<int> expands;
    NewNode(std::string classname);
};