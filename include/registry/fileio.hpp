#pragma once

#include "registry/base/plugin.hpp"
#include "object/string.hpp"
#include "object/integer.hpp"
#include "object/float.hpp"
#include "object/boolean.hpp"
#include "object/enumerate.hpp"
#include "object/native_data.hpp"

#include <fstream>
#include <algorithm>

class FileIOPlugin : public Plugin {
public:
    FileIOPlugin() : Plugin() {}
    void attach(std::shared_ptr<Environment> env) const override {
        env->set("open_mode", std::make_shared<Enumerate>(std::map<std::string, unsigned long long>{
            { "read", 0 }, { "write", 1 }, { "append", 2 }, { "binary_read", 3 }, { "binary_write", 4 }, { "binary_append", 5 }
        }));
        env->set("fopen", std::make_shared<NativeFunction>(open));
        env->set("fisok", std::make_shared<NativeFunction>(isok));
        env->set("isfailed", std::make_shared<NativeFunction>(isfailed));
        env->set("fgetline", std::make_shared<NativeFunction>(getLine));
        env->set("fgetint", std::make_shared<NativeFunction>(getInt));
        env->set("fgetfloat", std::make_shared<NativeFunction>(getFloat));
        env->set("fgetchar", std::make_shared<NativeFunction>(getChar));
        env->set("fgetstring", std::make_shared<NativeFunction>(getString));
        env->set("fgetbool", std::make_shared<NativeFunction>(getBool));
        env->set("fclose", std::make_shared<NativeFunction>(close));
    }
private:
    static NativeFunction::resulttype open(NativeFunction::arglist args, Environment* env) {
        std::string toOpen;
        unsigned long long openMode;
        if (args.size() != 2) {
            return FormatError();
        }
        if (args[0]->type != Object::Type::String || args[1]->type != Object::Type::Integer) {
            return FormatError();
        }
        toOpen = args[0]->toString();
        openMode = std::dynamic_pointer_cast<Integer>(args[1])->value;
        if (openMode > 5) {
            return FormatError();
        }
        std::shared_ptr<NativeData> res;
        std::shared_ptr<std::fstream> fs;
        switch (openMode) {
        case 0:
            fs->open(toOpen, std::ios::in);
            break;
        case 1:
            fs->open(toOpen, std::ios::out);
            break;
        case 2:
            fs->open(toOpen, std::ios::app);
            break;
        case 3:
            fs->open(toOpen, std::ios::in | std::ios::binary);
            break;
        case 4:
            fs->open(toOpen, std::ios::out | std::ios::binary);
            break;
        case 5:
            fs->open(toOpen, std::ios::app | std::ios::binary);
            break;
        default:
            return FormatError();
        }
        res->value = std::make_any<std::shared_ptr<std::fstream>>(fs);
    }
    static bool isGood(std::shared_ptr<NativeData> ptr) {
        return ptr->value.type() == typeid(std::shared_ptr<std::fstream>);
    }
    static std::shared_ptr<std::fstream> dataCast(std::shared_ptr<NativeData> ptr) {
        return std::any_cast<std::shared_ptr<std::fstream>>(ptr->value);
    }
    static NativeFunction::resulttype isok(NativeFunction::arglist args, Environment* env) {
        if (args.size() != 1) {
            return FormatError();
        }
        if (args[0]->type != Object::Type::NativeData) {
            return FormatError();
        }
        auto _cast = std::dynamic_pointer_cast<NativeData>(args[0]);
        if (!isGood(_cast)) {
            return FormatError();
        }
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Boolean>(dataCast(_cast)->good()));
    }
    static NativeFunction::resulttype isfailed(NativeFunction::arglist args, Environment* env) {
        if (args.size() != 1) {
            return FormatError();
        }
        if (args[0]->type != Object::Type::NativeData) {
            return FormatError();
        }
        auto _cast = std::dynamic_pointer_cast<NativeData>(args[0]);
        if (!isGood(_cast)) {
            return FormatError();
        }
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Boolean>(dataCast(_cast)->fail()));
    }
    static NativeFunction::resulttype fprint(NativeFunction::arglist args, Environment* env) {
        if (args.size() < 1) {
            return FormatError();
        }
        if (args[0]->type != Object::Type::NativeData) {
            return FormatError();
        }
        auto _cast = std::dynamic_pointer_cast<NativeData>(args[0]);
        if (!isGood(_cast)) {
            return FormatError();
        }
        for (auto i : args) {
            (*dataCast(_cast)) << i->toString();
        }
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Null>());
    }
    static NativeFunction::resulttype fprintln(NativeFunction::arglist args, Environment* env) {
        if (args.size() < 1) {
            return FormatError();
        }
        if (args[0]->type != Object::Type::NativeData) {
            return FormatError();
        }
        auto _cast = std::dynamic_pointer_cast<NativeData>(args[0]);
        if (!isGood(_cast)) {
            return FormatError();
        }
        for (auto i : args) {
            (*dataCast(_cast)) << i->toString();
        }
        (*dataCast(_cast)) << "\n";
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Null>());
    }

    static NativeFunction::resulttype getLine(NativeFunction::arglist args, Environment* env) {
        if (args.size() != 1) {
            return FormatError();
        }
        if (args[0]->type != Object::Type::NativeData) {
            return FormatError();
        }
        auto _cast = std::dynamic_pointer_cast<NativeData>(args[0]);
        if (!isGood(_cast)) {
            return FormatError();
        }
        std::string str;
        std::getline((*dataCast(_cast)), str);
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<String>(str));
    }

    static NativeFunction::resulttype getInt(NativeFunction::arglist args, Environment* env) {
        if (args.size() != 1) {
            return FormatError();
        }
        if (args[0]->type != Object::Type::NativeData) {
            return FormatError();
        }
        auto _cast = std::dynamic_pointer_cast<NativeData>(args[0]);
        if (!isGood(_cast)) {
            return FormatError();
        }
        long long _val;
        (*dataCast(_cast)) >> _val;
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Integer>(_val));
    }

    static NativeFunction::resulttype getFloat(NativeFunction::arglist args, Environment* env) {
        if (args.size() != 1) {
            return FormatError();
        }
        if (args[0]->type != Object::Type::NativeData) {
            return FormatError();
        }
        auto _cast = std::dynamic_pointer_cast<NativeData>(args[0]);
        if (!isGood(_cast)) {
            return FormatError();
        }
        double _val;
        (*dataCast(_cast)) >> _val;
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<Float>(_val));
    }

    static NativeFunction::resulttype getChar(NativeFunction::arglist args, Environment* env) {
        if (args.size() != 1) {
            return FormatError();
        }
        if (args[0]->type != Object::Type::NativeData) {
            return FormatError();
        }
        auto _cast = std::dynamic_pointer_cast<NativeData>(args[0]);
        if (!isGood(_cast)) {
            return FormatError();
        }
        char ch;
        (*dataCast(_cast)) >> ch;
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<String>(ch));
    }

    static NativeFunction::resulttype getString(NativeFunction::arglist args, Environment* env) {
        if (args.size() != 1) {
            return FormatError();
        }
        if (args[0]->type != Object::Type::NativeData) {
            return FormatError();
        }
        auto _cast = std::dynamic_pointer_cast<NativeData>(args[0]);
        if (!isGood(_cast)) {
            return FormatError();
        }
        std::string str;
        (*dataCast(_cast)) >> str;
        return std::make_pair(NativeFunction::Result::OK, std::make_shared<String>(str));
    }

    static NativeFunction::resulttype getBool(NativeFunction::arglist args, Environment* env) {
        if (args.size() != 1) {
            return FormatError();
        }
        if (args[0]->type != Object::Type::NativeData) {
            return FormatError();
        }
        auto _cast = std::dynamic_pointer_cast<NativeData>(args[0]);
        if (!isGood(_cast)) {
            return FormatError();
        }
        std::string str;
        (*dataCast(_cast)) >> str;
        std::transform(str.begin(), str.end(), str.begin(), tolower);
        if (str == "1" || str == "t" || str == "true") {
            return std::make_pair(NativeFunction::Result::OK, std::make_shared<Boolean>(true));
        }
        else {
            return std::make_pair(NativeFunction::Result::OK, std::make_shared<Boolean>(false));
        }
    }

    static NativeFunction::resulttype close(NativeFunction::arglist args, Environment* env) {
        if (args.size() != 1) {
            return FormatError();
        }
        if (args[0]->type != Object::Type::NativeData) {
            return FormatError();
        }
        auto _cast = std::dynamic_pointer_cast<NativeData>(args[0]);
        if (!isGood(_cast)) {
            return FormatError();
        }
        dataCast(_cast)->close();
    }
};