#include "plugins/plugin.hpp"
#include "vm_error.hpp"
#include "iostream"
#include "object/integer.hpp"
#include "object/string.hpp"
#include "object/float.hpp"

Plugins::IO::IO() {}

std::shared_ptr<Object> Get_Int(Args args) {
    if(args.size()) {
        throw VMError("(IO)Get_Int", "Incorrect Format");
    }
    long long tmp;
    std::cin >> tmp;
    return std::make_shared<Integer>(tmp);
}

std::shared_ptr<Object> Get_String(Args args) {
    if(args.size()) {
        throw VMError("(IO)Get_String", "Incorrect Format");
    }
    std::string tmp;
    std::cin >> tmp;
    return std::make_shared<String>(tmp);
}

std::shared_ptr<Object> Get_Float(Args args) {
    if(args.size()) {
        throw VMError("(IO)Get_Float", "Incorrect Format");
    }
    double tmp;
    std::cin >> tmp;
    return std::make_shared<Float>(tmp);
}

std::shared_ptr<Object> Get_Line(Args args) {
    if(args.size()) {
        throw VMError("(IO)Get_Line", "Incorrect Format");
    }
    std::string tmp;
    std::getline(std::cin, tmp);
    return std::make_shared<String>(tmp);
}

std::shared_ptr<Object> Get_Char(Args args) {
    if(args.size()) {
        throw VMError("(IO)Get_Char" , "Incorrect Format");
    }
    char tmp;
    std::cin.get(tmp);
    return std::make_shared<String>(tmp);
}

void Plugins::IO::enable() {
    regist("getint" , Get_Int);
    regist("input" , Get_String);
    regist("getfloat" , Get_Float);
    regist("getline" , Get_Line);
    regist("getchar" , Get_Char);
}