#pragma once

// Include your plugin there.
#include "registry/dynamic_load.hpp"
#include "registry/io.hpp"
#include "registry/constants.hpp"
#include "registry/fileio.hpp"
#include "registry/base.hpp"

#include "program/program.hpp"

class PluginProvider {
private:
    std::shared_ptr<Program> _target;
public:
    PluginProvider(std::shared_ptr<Program> _target) : _target(_target) {}
    void attach() {
        // Load your plugin there.
        _target->loadLibrary(std::make_shared<DLoadPlugin>());
        _target->loadLibrary(std::make_shared<IOPlugin>());
        _target->loadLibrary(std::make_shared<ConstantsPlugin>());
        _target->loadLibrary(std::make_shared<FileIOPlugin>());

		// DO NOT DELETE THIS!!!
		// Classes Inheritance cannot run without it.
		// Dynamic Variable Controlling cannot run without it.
		_target->loadLibrary(std::make_shared<BasePlugin>());
    }
};