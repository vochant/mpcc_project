#pragma once

#include "object/environment.hpp"
#include "object/native_function.hpp"
#include "object/error.hpp"

class Plugin {
public:
	Plugin() {}
	virtual void attach(std::shared_ptr<Environment> env) const = 0;
};