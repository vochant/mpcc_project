#pragma once

#include "registry/base/plugin.hpp"
#include "object/string.hpp"
#include "object/integer.hpp"
#include "object/float.hpp"
#include "object/boolean.hpp"

#include <iostream>
#include <algorithm>

class IOPlugin : public Plugin {
public:
    IOPlugin() : Plugin() {}
    void attach(std::shared_ptr<Environment> env) const override {
        // Optimize speed for <iostream> and disable <stdio>
        std::ios::sync_with_stdio(false);
        std::cout.tie(nullptr);
        std::cin.tie(nullptr);
        // Register native functions
        env->set("print", std::make_shared<NativeFunction>(print));
        env->set("println", std::make_shared<NativeFunction>(println));
        env->set("getline", std::make_shared<NativeFunction>(getLine));
        env->set("getint", std::make_shared<NativeFunction>(getInt));
        env->set("getfloat", std::make_shared<NativeFunction>(getFloat));
        env->set("getchar", std::make_shared<NativeFunction>(getChar));
        env->set("getstring", std::make_shared<NativeFunction>(getString));
        env->set("getbool", std::make_shared<NativeFunction>(getBool));
        env->set("termformat", std::make_shared<NativeFunction>(termFormat));
    }
private:
    static NativeFunction::resulttype print(NativeFunction::arglist args, Environment* env) {
        for (auto i : args) {
            std::cout << i->toString();
        }
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Null>());
    }

    static NativeFunction::resulttype println(NativeFunction::arglist args, Environment* env) {
        for (auto i : args) {
            std::cout << i->toString();
        }
        std::cout << "\n";
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Null>());
    }

    static NativeFunction::resulttype getLine(NativeFunction::arglist args, Environment* env) {
        if (args.size() > 0) {
            return std::make_pair(NativeFunction::Result::FORMAT_ERR, std::make_shared<Error>());
        }
        std::string str;
        std::getline(std::cin, str);
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<String>(str));
    }

    static NativeFunction::resulttype getInt(NativeFunction::arglist args, Environment* env) {
        if (args.size() > 0) {
            return std::make_pair(NativeFunction::Result::FORMAT_ERR, std::make_shared<Error>());
        }
        long long _val;
        std::cin >> _val;
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Integer>(_val));
    }

    static NativeFunction::resulttype getFloat(NativeFunction::arglist args, Environment* env) {
        if (args.size() > 0) {
            return std::make_pair(NativeFunction::Result::FORMAT_ERR, std::make_shared<Error>());
        }
        double _val;
        std::cin >> _val;
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Float>(_val));
    }

    static NativeFunction::resulttype getChar(NativeFunction::arglist args, Environment* env) {
        if (args.size() > 0) {
            return std::make_pair(NativeFunction::Result::FORMAT_ERR, std::make_shared<Error>());
        }
        char ch;
        std::cin >> ch;
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<String>(ch));
    }

    static NativeFunction::resulttype getString(NativeFunction::arglist args, Environment* env) {
        if (args.size() > 0) {
            return std::make_pair(NativeFunction::Result::FORMAT_ERR, std::make_shared<Error>());
        }
        std::string str;
        std::cin >> str;
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<String>(str));
    }

    static NativeFunction::resulttype getBool(NativeFunction::arglist args, Environment* env) {
        if (args.size() > 0) {
            return std::make_pair(NativeFunction::Result::FORMAT_ERR, std::make_shared<Error>());
        }
        std::string str;
        std::cin >> str;
        std::transform(str.begin(), str.end(), str.begin(), tolower);
        if (str == "1" || str == "t" || str == "true") {
            return std::make_pair(NativeFunction::Result::OK, std::make_shared<Boolean>(true));
        }
        else {
            return std::make_pair(NativeFunction::Result::OK, std::make_shared<Boolean>(false));
        }
    }

    static NativeFunction::resulttype termFormat(NativeFunction::arglist args, Environment* env) {
        for (auto i : args) {
            if (i->type != Object::Type::String) {
                return std::make_pair(NativeFunction::Result::FORMAT_ERR, std::make_shared<Error>());
            }
        }
        for (auto i : args) {
            std::cout << "\033[" << i->toString();
        }
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Null>());
    }
};