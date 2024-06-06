#pragma once

#include <string>
#include <map>
#include <vector>
#include <cstdlib>

#include "program/module.hpp"

#ifdef CONSOLE_BUILD
#ifdef _WIN32

#define _CODEPAGE_REQUIRED
#include <windows.h>

#endif
#endif

typedef std::map<std::string, std::string> Language;
typedef std::map<std::string, std::string> Replacement;

class I18N {
private:
	#ifdef _CODEPAGE_REQUIRED
	unsigned int _system_icodepage, _system_ocodepage; // Compatibility for Windows
	#endif

	Language mp;
	static std::string _replace(std::string str, const std::string& from, const std::string& to) {
		size_t pos = 0;
		while ((pos = str.find(from)) != str.npos) {
			str.replace(pos, from.length(), to);
		}
		return str;
	}
public:
	I18N() {
		#ifdef _CODEPAGE_REQUIRED
		_system_icodepage = GetConsoleCP();
		_system_ocodepage = GetConsoleOutputCP();
		SetConsoleCP(65001);
		SetConsoleOutputCP(65001);
		#endif
	}

	~I18N() {
		#ifdef _CODEPAGE_REQUIRED
		SetConsoleCP(_system_icodepage);
		SetConsoleOutputCP(_system_ocodepage);
		#endif
	}

	std::string lookup(std::string key, Replacement repl = {}) const {
		auto it = mp.find(key);
		if (it == mp.end()) {
			return key;
		}
		std::string result = it->second;
		for (auto it = repl.begin(); it != repl.end(); it++) {
			result = _replace(result, it->first, it->second);
		}
		return result;
	}

	bool Register(const Language lang) {
		mp.insert(lang.begin(), lang.end());
		return false;
	}

	bool Register(const std::pair<std::string, std::string> p) {
		mp.insert(p);
		return false;
	}

	bool Register(Module& mod) {
		auto& v = mod.what();
		if (!v.contains("data") || !v.at("data").is_array()) return true;
		for (auto& i : v.at("data").items()) {
			if (!i.value().is_object()) {
				mp.clear();
				return true;
			}
			if (!i.value().contains("key") || !i.value().contains("value") || !i.value().at("key").is_string() || !i.value().at("value").is_string()) {
				mp.clear();
				return true;
			}
			mp.insert(std::make_pair(i.value().at("key").get<std::string>(), i.value().at("value").get<std::string>()));
		}
		return false;
	}
};