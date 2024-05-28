#pragma once

#include "registry/base/plugin.hpp"
#include "object/boolean.hpp"
#include "object/integer.hpp"
#include "object/float.hpp"

class ConstantsPlugin : public Plugin {
public:
    ConstantsPlugin() : Plugin() {}
    void attach(std::shared_ptr<Environment> env) const override {
        env->set("is_linux", std::make_shared<NativeFunction>(is_linux));
        env->set("is_windows", std::make_shared<NativeFunction>(is_windows));
        env->set("is_apple", std::make_shared<NativeFunction>(is_apple));
        env->set("is_unix", std::make_shared<NativeFunction>(is_unix));
        env->set("is_android", std::make_shared<NativeFunction>(is_android));
        env->set("posix_supported", std::make_shared<NativeFunction>(posix_supported));
        env->set("is_debug", std::make_shared<NativeFunction>(is_debug));
        env->set("mpcc_version", std::make_shared<NativeFunction>(mpcc_version));

		env->set("NaN", std::make_shared<Float>(NAN));
		env->set("Infinity", std::make_shared<Float>(INFINITY));
    }
private:
    static NativeFunction::resulttype is_linux(NativeFunction::arglist args, Environment* env) {
        if (args.size() > 0) {
            return FormatError();
        }
        #ifdef __linux__
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Boolean>(true));
        #else
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Boolean>(false));
        #endif
    }

    static NativeFunction::resulttype is_windows(NativeFunction::arglist args, Environment* env) {
        if (args.size() > 0) {
            return FormatError();
        }
        #ifdef _WIN32
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Boolean>(true));
        #else
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Boolean>(false));
        #endif
    }

    static NativeFunction::resulttype is_apple(NativeFunction::arglist args, Environment* env) {
        if (args.size() > 0) {
            return FormatError();
        }
        #ifdef __APPLE__
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Boolean>(true));
        #else
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Boolean>(false));
        #endif
    }

    static NativeFunction::resulttype is_android(NativeFunction::arglist args, Environment* env) {
        if (args.size() > 0) {
            return FormatError();
        }
        #ifdef __ANDROID__
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Boolean>(true));
        #else
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Boolean>(false));
        #endif
    }

    static NativeFunction::resulttype is_unix(NativeFunction::arglist args, Environment* env) {
        if (args.size() > 0) {
            return FormatError();
        }
        #ifdef __unix__
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Boolean>(true));
        #else
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Boolean>(false));
        #endif
    }

    static NativeFunction::resulttype posix_supported(NativeFunction::arglist args, Environment* env) {
        if (args.size() > 0) {
            return FormatError();
        }
        #ifdef _POSIX_VERSION
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Boolean>(true));
        #else
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Boolean>(false));
        #endif
    }

    static NativeFunction::resulttype is_debug(NativeFunction::arglist args, Environment* env) {
        if (args.size() > 0) {
            return FormatError();
        }
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Boolean>(IS_DEBUG));
    }

    static NativeFunction::resulttype mpcc_version(NativeFunction::arglist args, Environment* env) {
        if (args.size() > 0) {
            return FormatError();
        }
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Integer>(VERSION_NUMBER));
    }
};