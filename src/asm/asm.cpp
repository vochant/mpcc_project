#include "asm/asm.hpp"
#include "util.hpp"

Asm::Asm(Type type) : type(type) {}
StringAsm::StringAsm(Type type, std::string payload) : Asm(type), payload(payload) {}
IntegerAsm::IntegerAsm(Type type, long long payload) : Asm(type), payload(payload) {}
Int32Asm::Int32Asm(Type type, int payload) : Asm(type), payload(payload) {}
Int16Asm::Int16Asm(Type type, short payload) : Asm(type), payload(payload) {}
Int8Asm::Int8Asm(Type type, char payload) : Asm(type), payload(payload) {}
SIAsm::SIAsm(Type type, long long payload_a, std::string payload_b) : Asm(type), payload_a(payload_a), payload_b(payload_b) {}
FloatAsm::FloatAsm(Type type, double payload) : Asm(type), payload(payload) {}
Float32Asm::Float32Asm(Type type, float payload) : Asm(type), payload(payload) {}
CustomAsm::CustomAsm(Type type) : Asm(type) {}

void Asm::save(std::fstream& fs) const {
    BinaryOut::write_byte(fs, (char)type);
}

void StringAsm::save(std::fstream& fs) const {
    BinaryOut::write_byte(fs, (char)type);
    BinaryOut::write_string(fs, payload);
}

void IntegerAsm::save(std::fstream& fs) const {
    BinaryOut::write_byte(fs, (char)type);
    BinaryOut::write_data(fs, payload);
}

void Int32Asm::save(std::fstream& fs) const {
    BinaryOut::write_byte(fs, (char)type);
    BinaryOut::write_data(fs, payload);
}

void Int16Asm::save(std::fstream& fs) const {
    BinaryOut::write_byte(fs, (char)type);
    BinaryOut::write_data(fs, payload);
}

void Int8Asm::save(std::fstream& fs) const {
    BinaryOut::write_byte(fs, (char)type);
    BinaryOut::write_data(fs, payload);
}

void SIAsm::save(std::fstream& fs) const {
    BinaryOut::write_byte(fs, (char)type);
    BinaryOut::write_data(fs, payload_a);
    BinaryOut::write_string(fs, payload_b);
}

void FloatAsm::save(std::fstream& fs) const {
    BinaryOut::write_byte(fs, (char)type);
    BinaryOut::write_data(fs, payload);
}

void Float32Asm::save(std::fstream& fs) const {
    BinaryOut::write_byte(fs, (char)type);
    BinaryOut::write_data(fs, payload);
}

void Asm::read(std::fstream& fs) {
    type = (Type)BinaryIn::read_byte(fs);
}

void StringAsm::read(std::fstream& fs) {
    type = (Type)BinaryIn::read_byte(fs);
    payload = BinaryIn::read_string(fs);
}

void IntegerAsm::read(std::fstream& fs) {
    type = (Type)BinaryIn::read_byte(fs);
    payload = BinaryIn::read_data<long long>(fs);
}

void Int32Asm::read(std::fstream& fs) {
    type = (Type)BinaryIn::read_byte(fs);
    payload = BinaryIn::read_data<int>(fs);
}

void Int16Asm::read(std::fstream& fs) {
    type = (Type)BinaryIn::read_byte(fs);
    payload = BinaryIn::read_data<short>(fs);
}

void Int8Asm::read(std::fstream& fs) {
    type = (Type)BinaryIn::read_byte(fs);
    payload = BinaryIn::read_data<char>(fs);
}

void SIAsm::read(std::fstream& fs) {
    type = (Type)BinaryIn::read_byte(fs);
    payload_a = BinaryIn::read_data<long long>(fs);
    payload_b = BinaryIn::read_string(fs);
}

void FloatAsm::read(std::fstream& fs) {
    type = (Type)BinaryIn::read_byte(fs);
    payload = BinaryIn::read_data<double>(fs);
}

void Float32Asm::read(std::fstream& fs) {
    type = (Type)BinaryIn::read_byte(fs);
    payload = BinaryIn::read_data<float>(fs);
}