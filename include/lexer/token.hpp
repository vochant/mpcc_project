#pragma once

#include <string>
#include <map>

class Token {
public:
    enum class Type {
        Integer, String, Extand, If, Else, Identifier,
        Let, For, While, End, Error, Scope, Float,
        LParan, RParan, LBrace, RBrace, LBracket, RBracket,
        Comma, Semicolon, Return, Import,
        Class, Constructor, Destructor, Plus, Minus, Asterisk,
        Slash, Modulus, BitwiseLeft, BitwiseRight, BitwiseAnd,
        BitwiseOr, BitwiseXor, BitwiseNot, LogicalNot, LogicalAnd,
        LogicalOr, True, False, Assign, PlusAssign, MinusAssign,
        AsteriskAssign, SlashAssign, ModulusAssign, BitwiseLeftAssign,
        BitwiseRightAssign, BitwiseAndAssign, BitwiseOrAssign,
        BitwiseXorAssign, Equal, NotEqual, Greater, Less, GreaterEqual,
        LessEqual, Lambda, Global, Var, Function,
        Public, Private, Dowhile, Const, Delete, Enumerate,
        Null, Increment, Decrement, As, Ternary, More, Func,
        Method, Static, Value, ForceExtand, FunctionDef,
        New, Decorate, Final, Break, Continue, Arrow
    } type;
    std::string value, dit;
    Token(const Type type, const std::string value);
    Token(const std::string value, const Type type);
    static const std::map<Type, std::string> typeNames;
};