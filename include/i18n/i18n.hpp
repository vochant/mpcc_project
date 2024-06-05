#pragma once

#include <string>
#include <map>
#include <vector>
#include <cstdlib>

#include "program/vm_options.hpp"

#ifdef CONSOLE_BUILD
#ifdef _WIN32

#define _CODEPAGE_REQUIRED
#include <windows.h>

#endif
#endif

typedef std::map<std::string, std::string> Language;
typedef std::map<std::string, std::string> Replacement;

class I18N {
public:
	enum LanguageId {
		en_US, zh_CN, zh_TW, ja_JP,
		ma_MP, ko_KR, ru_RU, fr_FR,
		de_DE, it_IT, pt_PT, es_ES,
		vi_VN, lzh, jbo, la_LA,
		lol_US, enp, tok, zh_C2,
		zh_KAWAII,
		Count // This is a counter of supported languages.
	} current;
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
	static std::string idOf(const LanguageId& lang) {
		if (lang == en_US) return "en_US"; // Englist (United States)
		if (lang == zh_CN) return "zh_CN"; // Simplified Chinese (China Mainland)
		if (lang == zh_TW) return "zh_TW"; // Traditional Chinese (Taiwan)
		if (lang == ja_JP) return "ja_JP"; // Japanese (Japan)
		if (lang == ma_MP) return "ma_MP"; // Minffosa (Minphtis)
		if (lang == ko_KR) return "ko_KR"; // Korean (Korea)
		if (lang == ru_RU) return "ru_RU"; // Russian (Russia)
		if (lang == fr_FR) return "fr_FR"; // French (France)
		if (lang == de_DE) return "de_DE"; // German (Germany)
		if (lang == it_IT) return "it_IT"; // Italian (Italy)
		if (lang == pt_PT) return "pt_PT"; // Portuguese (Portugal)
		if (lang == es_ES) return "es_ES"; // Spanish (Spain)
		if (lang == vi_VN) return "vi_VN"; // Vietnamese (Vietnam)
		if (lang == lzh) return "lzh"; // Legacy Chinese (Huaxia)
		if (lang == jbo) return "jbo"; // Lojban
		if (lang == la_LA) return "la_LA"; // Latin (Latin)
		if (lang == lol_US) return "lol_US"; // LOLCAT!
		if (lang == enp) return "enp"; // Old English
		if (lang == tok) return "tok"; // Toki Pona
		if (lang == zh_C2) return "zh_C2"; // Simplified Chinese 中二版本
		if (lang == zh_KAWAII) return "zh_KAWAII"; // Simplified Chinese 卡哇伊版本
		return "unknown";
	}

	I18N() : current(DEFAULT_I18N) {
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
			return key + "." + idOf(current);
		}
		std::string result = it->second;
		for (auto it = repl.begin(); it != repl.end(); it++) {
			result = _replace(result, it->first, it->second);
		}
		return result;
	}

	void Register(const Language lang) {
		mp = lang;
	}
};