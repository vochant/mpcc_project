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
};