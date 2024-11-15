#pragma once

#include "env/environment.hpp"
#include "object/nativefunction.hpp"

class Plugin {
private:
    std::shared_ptr<Environment> _env;
public:
    void attach(std::shared_ptr<Environment> env);
protected:
    void regist(std::string name, std::shared_ptr<Object> value);
    void regist(std::string name, NFunc func);
    virtual void enable() = 0;
public:
    Plugin();
};

namespace Plugins {
    class IO : private Plugin {
    protected:
        void enable() override;
    public:
        IO();
    };

    class FileIO : private Plugin {
    protected:
        void enable() override;
    public:
        FileIO();
    };

    class Math : private Plugin {
    protected:
        void enable() override;
    public:
        Math();
    };

    class Base : private Plugin {
    protected:
        void enable() override;
    public:
        Base();
    };
}