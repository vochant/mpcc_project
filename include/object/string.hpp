#pragma once

#include "object/base.hpp"

class String : public Object {
public:
	std::string value;
	String(std::string value = "") : value(value), Object(Object::Type::String) {}
	String(std::istream& is) : Object(Object::Type::String) {
		readFrom(is);
	}
public:
	static bool isVisible(const char ch) {
		return std::string("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`~!@#$%^&*()-=_+[]{}\\|;:'\",./<>? ").find(ch) != std::string::npos;
	}
	static std::string to_hex(const char ch) {
		std::string hex_charset = "0123456789abcdef";
		std::string result = "\\x";
		result += hex_charset[ch >> 4];
		result += hex_charset[ch & 15];
		return result;
	}
	static std::string escape(const std::string str) {
		std::string result = "\"";
		for (size_t i = 0; i < str.length(); i++) {
			switch (str.at(i)) {
				case '\a':
					result += "\\a";
					break;
				case '\b':
					result += "\\b";
					break;
				case '\f':
					result += "\\f";
					break;
				case '\n':
					result += "\\n";
					break;
				case '\r':
					result += "\\r";
					break;
				case '\t':
					result += "\\t";
					break;
				case '\v':
					result += "\\v";
					break;
				case '\\':
					result += "\\\\";
					break;
				case '"':
					result += "\\\"";
					break;
				case '\0':
					result += "\\0";
					break;
				default:
					if (isVisible(str.at(i))) {
						result += str.at(i);
					}
					else {
						result += to_hex(str.at(i));
					}
					break;
			}
		}
		result += "\"";
		return result;
	}
	static std::string unescape(const std::string str) {
		std::string result;
		for (size_t i = 1; i < str.length() - 1; i++) {
			if (str.at(i) == '\\') {
				switch (str.at(i + 1)) {
					case 'a':
						result += '\a';
						break;
					case 'b':
						result += '\b';
						break;
					case 'f':
						result += '\f';
						break;
					case 'n':
						result += '\n';
						break;
					case 'r':
						result += '\r';
						break;
					case 't':
						result += '\t';
						break;
					case 'v':
						result += '\v';
						break;
					case '\\':
						result += '\\';
						break;
					case '0':
						result += '\0';
						break;
					case 'x':
						int _iv;
						try {
							_iv = stoi(str.substr(i + 2, 2), nullptr, 16);
							result += char(_iv);
							i += 2;
						}
						catch (...) {
							result += 'x';
						}
						break;
					default:
						result += str.at(i + 1);
						break;
				}
				i++;
			}
			else {
				result += str.at(i);
			}
		}
		return result;
	}
public:
	std::string toString() const override {
		return value;
	}

	std::shared_ptr<Object> copy() const override {
		return std::make_shared<String>(*this);
	}

	std::string idstr() const override {
		return "string";
	}

	void assign(std::shared_ptr<Object> value) override {
		bool _is_mutable = isMutable;
		*this = *std::dynamic_pointer_cast<String>(value);
		isMutable = _is_mutable;
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Object::Type::String));
		BinaryOut::write_string(os, value);
	}
	void readFrom(std::istream& is) override {
		value = BinaryIn::read_string(is);
	}
};