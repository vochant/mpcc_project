#pragma once

#include <fstream>
#include <string>

namespace BinaryOut {
	void write_byte(std::ostream& os, const char b) {
		os.write(&b, sizeof(char));
	}

	template<typename _Tp>
	void write_data(std::ostream& os, const _Tp d) {
		os.write(reinterpret_cast<const char*>(&d), sizeof(_Tp));
	}

	void write_string(std::ostream& os, const std::string str) {
		write_data(os, str.length());
		for (size_t i = 0; i < str.length(); i++) {
			write_byte(os, str.at(i));
		}
	}
}

namespace BinaryIn {
	char read_byte(std::istream& is) {
		char _res;
		is.read(&_res, sizeof(char));
		return _res;
	}

	template<typename _Tp>
	_Tp read_data(std::istream& is) {
		_Tp _res;
		is.read(reinterpret_cast<char*>(&_res), sizeof(_Tp));
		return _res;
	}

	std::string read_string(std::istream& is) {
		size_t _len = read_data<size_t>(is);
		std::string _str;
		while (_len--) {
			_str += read_byte(is);
		}
		return _str;
	}
}