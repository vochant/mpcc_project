#pragma once

#include "convertor/asm2plain.hpp"
#include "util.hpp"

std::string casm2plain(std::shared_ptr<Asm> ptr) {
    using Type = Asm::Type;
    switch(ptr->type) {
    case Type::A2I:
        return "a2i";
    case Type::ACCESS:
        return "access";
    case Type::ADC:
        return "adc";
    case Type::ADD:
        return "add";
    case Type::ADEC:
        return "adec";
    case Type::AINC:
        return "ainc";
    case Type::ARG:
        return "arg";
    case Type::ATTR:
        return "attr";
    case Type::BAND:
        return "band";
    case Type::BDEC:
        return "bdec";
    case Type::BFD:
        return "bfd";
    case Type::BFDC:
        return "bfdc";
    case Type::BFDD:
        return "bfdd";
    case Type::BINC:
        return "binc";
    case Type::BNOT:
        return "bnot";
    case Type::BOR:
        return "bor";
    case Type::BR:
        return "br";
    case Type::BXOR:
        return "bxor";
    case Type::CALL:
        return "call";
    case Type::CEQ:
        return "ceq";
    case Type::CF:
        return "cf";
    case Type::CFL32:
        return "cfl32";
    case Type::CFL:
        return "cfl";
    case Type::CG:
        return "cg";
    case Type::CGE:
        return "cge";
    case Type::CBYTE:
        return "cbyte";
    case Type::CI8:
        return "ci8";
    case Type::CI16:
        return "ci16";
    case Type::CI32:
        return "ci32";
    case Type::CINT:
        return "cint";
    case Type::CL:
        return "cl";
    case Type::CLARG:
        return "clarg";
    case Type::CLASS:
        return "class";
    case Type::CLAT:
        return "clat";
    case Type::CLD:
        return "cld";
    case Type::CLE:
        return "cle";
    case Type::CNEQ:
        return "cneq";
    case Type::CONSTV:
        return "constv";
    case Type::CSTR:
        return "cstr";
    case Type::DEL:
        return "del";
    case Type::DIV:
        return "div";
    case Type::EARG:
        return "earg";
    case Type::ECLASS:
        return "eclass";
    case Type::EENTRY:
        return "eentry";
    case Type::EENUM:
        return "eenum";
    case Type::EFD:
        return "efd";
    case Type::EFDC:
        return "efdc";
    case Type::EFDD:
        return "efdd";
    case Type::EFLAG:
        return "eflag";
    case Type::ENUM:
        return "enum";
    case Type::EOFV:
        return "eofv";
    case Type::ER:
        return "er";
    case Type::ERECV:
        return "erecv";
    case Type::ERR:
        return "err";
    case Type::EVAL:
        return "eval";
    case Type::FACS:
        return "facs";
    case Type::FALSEV:
        return "falsev";
    case Type::FINAL:
        return "final";
    case Type::FLAG:
        return "flag";
    case Type::GLOBAL:
        return "global";
    case Type::IEND:
        return "iend";
    case Type::IF:
        return "if";
    case Type::IGET:
        return "iget";
    case Type::IINC:
        return "iinc";
    case Type::INDEX:
        return "index";
    case Type::INT:
        return "int";
    case Type::JARR:
        return "jarr";
    case Type::JF:
        return "jf";
    case Type::JFK:
        return "jfk";
    case Type::JMP:
        return "jmp";
    case Type::JT:
        return "jt";
    case Type::JTK:
        return "jtk";
    case Type::LET:
        return "let";
    case Type::LINE:
        return "line";
    case Type::METHOD:
        return "method";
    case Type::MKARR:
        return "mkarr";
    case Type::MOD:
        return "mod";
    case Type::MOV:
        return "mov";
    case Type::MUL:
        return "mul";
    case Type::NEG:
        return "neg";
    case Type::NEW:
        return "new";
    case Type::NOT:
        return "not";
    case Type::NULLV:
        return "nullv";
    case Type::PCQ:
        return "pcq";
    case Type::PNUL:
        return "pnul";
    case Type::POP:
        return "pop";
    case Type::POS:
        return "pos";
    case Type::PRIV:
        return "priv";
    case Type::PUB:
        return "pub";
    case Type::PUSH:
        return "push";
    case Type::RECV:
        return "recv";
    case Type::RET:
        return "ret";
    case Type::SBR:
        return "sbr";
    case Type::SGEH:
        return "sgeh";
    case Type::SHL:
        return "shl";
    case Type::SHR:
        return "shr";
    case Type::SUB:
        return "sub";
    case Type::SRC:
        return "src";
    case Type::SRVL:
        return "srvl";
    case Type::SSCL:
        return "sscl";
    case Type::STATIC:
        return "static";
    case Type::THROW:
        return "throw";
    case Type::TRAP:
        return "trap";
    case Type::TREQ:
        return "treq";
    case Type::TRUEV:
        return "truev";
    case Type::VALUE:
        return "value";
    case Type::VAR:
        return "var";
    default:
        return "<unknown>";
    }
}

std::string sasm2plain(std::shared_ptr<StringAsm> ptr) {
    return casm2plain(ptr) + " " + escape(ptr->payload);
}

std::string iasm2plain(std::shared_ptr<IntegerAsm> ptr) {
    return casm2plain(ptr) + " " + std::to_string(ptr->payload);
}

std::string fasm2plain(std::shared_ptr<FloatAsm> ptr) {
    return casm2plain(ptr) + " " + std::to_string(ptr->payload);
}

std::string siasm2plain(std::shared_ptr<SIAsm> ptr) {
    return casm2plain(ptr) + " " + std::to_string(ptr->payload_a) + " " + escape(ptr->payload_b);
}

std::string asm2plain(std::shared_ptr<Asm> ptr) {
    using Type = Asm::Type;
    switch (ptr->type) {
    case Type::ACCESS:
    case Type::ATTR:
    case Type::CSTR:
    case Type::DEL:
    case Type::ECLASS:
    case Type::EENTRY:
    case Type::EENUM:
    case Type::FACS:
    case Type::LET:
    case Type::METHOD:
    case Type::NEW:
    case Type::SRC:
    case Type::STATIC:
    case Type::PUSH:
    case Type::RECV:
    case Type::ERECV:
    case Type::VALUE:
    case Type::VAR:
        return sasm2plain(std::dynamic_pointer_cast<StringAsm>(ptr));
    case Type::CINT:
    case Type::EFDC:
    case Type::FLAG:
    case Type::JF:
    case Type::JFK:
    case Type::JMP:
    case Type::JT:
    case Type::JTK:
        return iasm2plain(std::dynamic_pointer_cast<IntegerAsm>(ptr));
    case Type::CFL:
        return fasm2plain(std::dynamic_pointer_cast<FloatAsm>(ptr));
    case Type::TREQ:
        return siasm2plain(std::dynamic_pointer_cast<SIAsm>(ptr));
    default:
        return casm2plain(ptr);
    }
}