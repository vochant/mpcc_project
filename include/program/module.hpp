#pragma once

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include <string>
#include <fstream>
#include <algorithm>

#include "system/current.hpp"
#include "program/util.hpp"

class Module {
public:
	std::string name, type;
	json feature;
public:
	Module() : name("__null__"), feature({}) {}
	bool load(std::string modulePath, std::string moduleName) {
		auto _pos = moduleName.find('.');
		if (_pos == moduleName.npos) return true;
		name = moduleName.substr(0, _pos);
		type = moduleName.substr(_pos + 1);
		feature = {};
		if (loadFile(modulePath, moduleName)) {
			std::cout << "load:failed " << modulePath << " " << moduleName << "\n";
			return true;
		}
		return false;
	}
	json& what() {
		return feature;
	}
private:
	void updateJson(json& origin, const json& updater) {
		for (auto& i : updater.items()) {
			if (origin.contains(i.key())) {
				if (origin[i.key()].is_object() && i.value().is_object()) {
					updateJson(origin[i.key()], i.value());
					continue;
				}
			}
			origin[i.key()] = i.value();
		}
	}
	bool loadFile(std::string modulePath, std::string moduleName) {
		std::ifstream fs(respath / modulePath);
		if (!fs.good()) {
			return true;
		}
		json _global;
		try {
			fs >> _global;
		}
		catch (std::exception& e) {
			std::cout << e.what();
			return true;
		}
		fs.close();
		if (!_global.contains("ident") || !_global.contains("version")) {
			return true;
		}
		if (!_global.at("ident").is_string() || !_global.at("version").is_number_integer()) {
			return true;
		}
		if (_global.at("ident") != "com.minphtis.mpcc" || _global.at("version") > 1) {
			return true;
		}
		if (!_global.contains("features") || !_global.contains("repo")) {
			return true;
		}
		if (!_global.at("features").is_array() || !_global.at("repo").is_object()) {
			return true;
		}
		return getMod(_global, moduleName);
	}
	bool getMod(json& _global, std::string moduleName) {
		if (std::find_if(_global.at("features").begin(), _global.at("features").end(), [&](const json& element) {
			if (element.is_string()) {
				if (element.get<std::string>() == moduleName) {
					return true;
				}
			}
			return false;
		}) == _global.at("features").end()) {
			return true;
		}
		if (!_global.at("repo").contains(moduleName)) {
			return true;
		}
		return loadMod(_global, moduleName);
	}
	bool loadMod(json& _global, std::string moduleName) {
		if (!_global.at("repo").at(moduleName).is_array()) {
			return true;
		}
		json& repo = _global.at("repo").at(moduleName);
		for (auto& x : repo.items()) {
			if (loadEach(_global, x.value())) {
				return true;
			}
		}
		return false;
	}
	bool loadEach(json& _global, json& _local) {
		if (!_local.is_object()) {
			return true;
		}
		if (!_local.contains("from")) {
			return true;
		}
		if (!_local.at("from").is_string()) {
			return true;
		}
		auto str = _local.at("from").get<std::string>();
		if (str == "local") {
			if (!_local.contains("value")) {
				return true;
			}
			if (!_local.at("value").is_object()) {
				return true;
			}
			updateJson(feature, _local.at("value"));
			return false;
		}
		if (str == "include") {
			if (!_local.contains("path")) {
				return true;
			}
			if (!_local.at("path").is_string()) {
				return true;
			}
			return getMod(_global, _local.at("path").get<std::string>());
		}
		if (str == "foreign") {
			if (!_local.contains("path")) {
				return true;
			}
			if (!_local.at("path").is_string()) {
				return true;
			}
			auto path = _local.at("path").get<std::string>();
			auto pos = path.find('@');
			if (pos == path.npos) {
				return true;
			}
			return loadFile(path.substr(0, pos), path.substr(path.substr(pos + 1)));
		}
		return true;
	}
};