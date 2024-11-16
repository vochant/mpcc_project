#include "plugins/plugin.hpp"
#include "vm_error.hpp"
#include "iostream"
#include "object/integer.hpp"
#include "object/string.hpp"
#include "object/float.hpp"
#include "object/nativeobject.hpp"
#include "vm/vm.hpp"

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

std::shared_ptr<Object> Print(Args args) {
    if(args.size() == 0)
    {
        throw VMError("(IO)Print" , "Incorrect Format");
    }
    bool isFirst = true;
    for (auto& e : args) {
        if (isFirst) isFirst = false;
        else std::cout << ' ';
        std::cout << e->toString();
    }
    return gVM->VNull;
}

std::shared_ptr<Object> FastIO_Get_Int(Args args) {
    if(args.size()) {
        throw VMError("(IO)FastIO:Get_Int", "Incorrect Format");
    }
    long long tmp , f = 1;
    char ch;
    std::cin.get(ch);
    while(ch < '0' || ch > '9') {
        if(ch == '-')
        {
            f = -1;
        }
        std::cin.get(ch);
    }
    while(ch >= '0' && ch <= '9') {
        tmp = tmp * 10 + ch - '0';
        std::cin.get(ch);
    }
    return std::make_shared<Integer>(f * tmp);
}

std::shared_ptr<Object> FastIO_Get_Float(Args args) {
    if(args.size()) {
        throw VMError("(IO)FastIO:Get_Float", "Incorrect Format");
    }
    double tmp , f = 1 , s = 1;
    char ch;
    std::cin.get(ch);
    while(ch < '0' || ch > '9') {
        if(ch == '-')
        {
            f = -1;
        }
        if(ch == '.')
        {
            goto readt;
        }
        std::cin.get(ch);
    }
    while(ch >= '0' && ch <= '9' && ch != '.') {
        tmp = tmp * 10 + ch - '0';
        std::cin.get(ch);
    }
    readt:while(ch == '.') {std::cin.get(ch);}
    while(ch >= '0' && ch <= '9') {
        tmp = tmp * 10 + ch - 0;
        s *= 10;
        std::cin.get(ch);
    }
    return std::make_shared<Integer>(f * (tmp / s));
}

void Plugins::IO::enable() {
    regist("getint" , Get_Int);
    regist("input" , Get_String);
    regist("getfloat" , Get_Float);
    regist("getline" , Get_Line);
    regist("getchar" , Get_Char);
    regist("print" , Print);
    auto FastIO = std::make_shared<NativeObject>();
    FastIO->set("getint", std::make_shared<NativeFunction>(FastIO_Get_Int));
    FastIO->set("getfloat" , std::make_shared<NativeFunction>(FastIO_Get_Float));
    regist("FastIO", FastIO);
}