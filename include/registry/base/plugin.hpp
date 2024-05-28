#pragma once

#include "object/environment.hpp"
#include "object/native_function.hpp"
#include "object/error.hpp"

/**
 * @class
 * @brief MPC Plugin Type Definition.
 */
class Plugin {
public:
	/**
	 * @brief Construct a new Plugin object
	 */
    Plugin() {}
	
	/**
	 * @brief Attach to an environment.
	 * @param env The environment to attach.
	 */
    virtual void attach(std::shared_ptr<Environment> env) const = 0;

	static NativeFunction::resulttype UnhandledError() {
		return std::make_pair(NativeFunction::Result::UNHANDLED_ERR, std::make_shared<Null>());
	}

	static NativeFunction::resulttype FormatError() {
		return std::make_pair(NativeFunction::Result::FORMAT_ERR, std::make_shared<Null>());
	}

	static NativeFunction::resulttype DataError() {
		return std::make_pair(NativeFunction::Result::DATA_ERR, std::make_shared<Null>());
	}

	static NativeFunction::resulttype OK(std::shared_ptr<Object> obj) {
		return std::make_pair(NativeFunction::Result::OK, obj);
	}
};