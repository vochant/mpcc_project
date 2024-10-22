#pragma once

#include <fstream>
#include <string>

struct ToAsmArgs {
    bool isInRepeat;
    size_t next_pt, end_pt;
    size_t* flagcount;
};

class Asm {
public:
	enum class Type : char {
        A2I,    // array to iterator (try)  first           =>  first
		ACCESS, // access member 			first.payload 	=> 	first
		ADC, 	// apply decorator 			@first second 	=> 	first
		ADD, 	// plus 					second+first 	=> 	first
		ADEC, 	// after decreasement 		first-- 		=> 	first
		AINC, 	// after increasement 		first++ 		=> 	first
		ARG, 	// argument 				first 			=> 	args
		ATTR,	// class attribute (extend) first 			=> 	null
		BAND, 	// bitwise and 				second&first 	=> 	first
		BDEC, 	// before decresement 		--first 		=> 	first
		BFD, 	// begin function def 		null 			=> 	null
        BFDC,   // begin constructor def    null            =>  null
        BFDD,   // begin destructor def     null            =>  null
		BINC, 	// before incresement 		++first 		=> 	first
		BNOT, 	// bitwise not 				~first 			=> 	first
		BOR, 	// bitwise or 				second|first 	=> 	first
		BR, 	// begin region 			null 			=> 	null
		BXOR, 	// bitwise xor 				second^first 	=> 	first
		CALL, 	// call 					first(args) 	=> 	first
		CEQ, 	// compare equal 			second==first 	=> 	first
		CF, 	// clear flags 				null 			=> 	null
        CFL32,  // float32 constant         payload         =>  first
		CFL, 	// float constant 			payload 		=> 	first
		CG, 	// compare greater 			second>first 	=> 	first
		CGE, 	// compare greater equal 	second>=first 	=> 	first
        CBYTE,  // byte constant            payload         =>  first
        CI8,    // int8 constant            payload         =>  first
        CI16,   // int16 constant           payload         =>  first
        CI32,   // int32 constant           payload         =>  first
		CINT, 	// integer constant 		payload 		=> 	first
		CL, 	// compare less 			second<first 	=> 	first
		CLARG, 	// clear args 				null 			=> 	args
		CLASS, 	// begin class def 			null 			=> 	null
		CLAT, 	// clear attribute 			null 			=> 	null
        CLD,    // custom load              payload:obj     =>  first
		CLE, 	// compare less equal 		second<=first 	=> 	first
		CNEQ, 	// compare not equal 		second!=first 	=> 	first
		CONSTV, // attribute const 			null 			=> 	null
		CSTR,	// string constant 			payload 		=> 	first
		DEL, 	// delete variable 			payload 		=> 	variable
		DIV, 	// divid 					second/first 	=> 	first
		EARG, 	// expand argument 			...first 		=> 	args
		ECLASS,	// end class def 			null 			=> 	null
		EENTRY,	// enumerate entry 			payload 		=> 	null
		EENUM, 	// end enumerate def 		payload 		=> 	null
		EFD, 	// end function def 		null 			=> 	first
        EFDC,   // end constructor def      payload         =>  null
        EFDD,   // end destructor def       null            =>  null
		EFLAG, 	// environment flags 		payload(first) 	=> 	eflags
		ENUM, 	// begin enumerate def 		null 			=> 	null
        EOFV,   // end of file              null            =>  null
		ER, 	// end region 				null 			=> 	null
		ERECV, 	// recieve expand argument 	...args 		=> 	variable
		ERR, 	// make an error 			null 			=> 	null
        EVAL,   // evaluate a code          first           =>  null
		FACS, 	// force access 			first::payload 	=> 	first
		FALSEV, // constant false 			false 			=> 	first
		FINAL, 	// final tag (non-virtual) 	null 			=> 	null
		FLAG, 	// flag 					payload 		=> 	null
		GLOBAL, // attribute global 		null 			=> 	null
        IEND,   // iterator end             :first          =>  first
		IF, 	// convert to bool 			first? 			=> 	first
        IGET,   // iterator get value       :first          =>  first
        IINC,   // iterator increase        first           =>  first
		INDEX, 	// access index 			second[first] 	=> 	first
		INT, 	// interrupt 				payload 		=> 	null
        JARR,   // join array               second+[first]  =>  first
		JF, 	// goto if not 				payload? !first => 	null
        JFK,    // goto if not (keep first) payload? !:first=>  null
		JMP, 	// goto 					first 			=> 	null
		JT, 	// goto if 					payload? first 	=> 	null
        JTK,    // goto if (keep first)     payload? :first =>  null
		LET, 	// create/replace variable 	first(payload) 	=> 	variable
		LINE, 	// line mark (debug) 		null 			=> 	debugger
		METHOD,	// class method 			first 			=> 	null
        MKARR,  // make empty array         null            =>  first
		MOD, 	// mod 						second/first 	=> 	first
        MOV, 	// assign 					second=first 	=> 	first
		MUL, 	// multiple 				second*first 	=> 	first
		NEG, 	// negative 				-first 			=> 	first
		NEW, 	// create instance 			payload(args) 	=> 	variable
		NOT, 	// logical not 				!first 			=> 	first
		NULLV, 	// null constant 			null 			=> 	first
		PCQ, 	// push call queue 			args 			=> 	args
		PNUL, 	// pop to null 				first 			=> 	null
		POP, 	// pop to variable 			first 			=> 	variable
		POS, 	// positive 				+first 			=> 	first
		PRIV, 	// private tag 				null 			=> 	null
		PUB, 	// public tag 				null 			=> 	null
		PUSH, 	// push variable 			variable 		=> 	first
		RECV, 	// recieve argument 		args 			=> 	variable
		RET, 	// return 					retval 			=> 	first
		SBR, 	// secure begin region 		null 			=> 	first
		SGEH, 	// set global errhandler 	first 			=> 	eflags
		SHL, 	// bitwise left 			second<<first 	=> 	first
		SHR, 	// bitwise right 			second>>first 	=> 	first
		SUB, 	// minus 					second-first 	=> 	first
		SRC, 	// source code (debug) 		payload 		=> 	debugger
		SRVL, 	// set return value 		first 			=> 	retval
		SSCL, 	// set syscall 				payload:first 	=> 	eflags
        STATIC, // class static             first           =>  variable
		THROW, 	// throw error 				first 			=> 	null
		TRAP, 	// trap (debug) 			null 			=> 	debugger
		TREQ, 	// arg type requirement 	payload:payload => 	null
		TRUEV, 	// constant true 			true 			=> 	first
        VALUE,  // class value              first           =>  null
		VAR, 	// create variable 			payload<-first  => 	variable
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
public:
    CustomAsm(Type type);
};