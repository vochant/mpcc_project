#pragma once

#include <fstream>
#include <string>

#include "object/object.hpp"

struct ToAsmArgs {
    bool isInRepeat;
    size_t next_pt, end_pt;
    size_t* flagcount;
};

class Asm {
public:
	enum class Type : char {
        // Basic - 2410
        A2I, ACCESS, ADC, ADD, ADEC, AINC,
		ARG, ATTR, BAND, BDEC, BFD, BFDC,
        BFDD, BINC, BNOT, BOR, BR, BXOR,
		CALL, CEQ, CF, CFL32, CFL, CG, CGE,
        CBYTE, CI8, CI16, CI32, CINT, CL,
		CLARG, CLASS, CLD, CLE, CNEQ, CONSTV,
		CSTR, DEL, DIV, EARG, ECLASS, EENTRY,
		EENUM, EFD, EFDC, EFDD, EFLAG, ENUM,
        EOFV, ER, ERECV, ERR, EVAL, FACS,
        FALSEV, FINAL, FLAG, GLOBAL, IEND,
		IF, IGET, IINC, INDEX, INT, JARR,
		JF, JFK, JMP, JT, JTK, LET, LINE,
		METHOD, MKARR, MOD, MOV, MUL, NEG,
		NEW, NOT, NULLV, PCQ, PNUL, POS,
        PRIV, PUB, PUSH, RECV, RET, SHL,
		SHR, SUB, SRC, SRVL, SSCL, STATIC,
        THROW, TRAP, TREQ, TRUEV, VALUE, VAR,
        // Bugfix A - 2410f1
        BRR, ERRI
	} type;
public:
	virtual void save(std::fstream& fs) const;
	virtual void read(std::fstream& fs);
public:
	Asm(Type type);
};

class StringAsm : public Asm{
public:
    std::string payload;
public:
    StringAsm(Type type, std::string payload);
    void save(std::fstream& fs) const override;
	void read(std::fstream& fs) override;
};

class IntegerAsm : public Asm {
public:
    long long payload;
public:
    IntegerAsm(Type type, long long payload);
    void save(std::fstream& fs) const override;
	void read(std::fstream& fs) override;
};

class Int32Asm : public Asm {
public:
    int payload;
public:
    Int32Asm(Type type, int payload);
    void save(std::fstream& fs) const override;
	void read(std::fstream& fs) override;
};

class Int16Asm : public Asm {
public:
    short payload;
public:
    Int16Asm(Type type, short payload);
    void save(std::fstream& fs) const override;
	void read(std::fstream& fs) override;
};

class Int8Asm : public Asm {
public:
    char payload;
public:
    Int8Asm(Type type, char payload);
    void save(std::fstream& fs) const override;
	void read(std::fstream& fs) override;
};

class SIAsm : public Asm {
public:
    long long payload_a;
    std::string payload_b;
public:
    SIAsm(Type type, long long payload_a, std::string payload_b);
    void save(std::fstream& fs) const override;
	void read(std::fstream& fs) override;
};

class FloatAsm : public Asm {
public:
    double payload;
public:
    FloatAsm(Type type, double payload);
    void save(std::fstream& fs) const override;
	void read(std::fstream& fs) override;
};

class Float32Asm : public Asm {
public:
    float payload;
public:
    Float32Asm(Type type, float payload);
    void save(std::fstream& fs) const override;
	void read(std::fstream& fs) override;
};

class CustomAsm : public Asm {
private:
    // std::shared_ptr<Object> payload;
public:
    CustomAsm(Type type);
};