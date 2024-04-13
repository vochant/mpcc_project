#pragma once

#include "object/base.hpp"
#include "object/null.hpp"

#include <sstream>

class Item {
public:
	enum class AccessToken {
		Private, Public
	} _access;
	std::shared_ptr<Object> value;
};

class Environment : public Object {
public:
	std::shared_ptr<Environment> _parent;
	std::map<std::string, Item> values;
	bool globalLock;
public:
	std::shared_ptr<Object> get(const std::string _name) const {
		if (_name == "_parent") return _parent;
		auto it = values.find(_name);
		if (it != values.end()) return it->second.value;
		else if (_parent) return _parent->get(_name);
		else return std::make_shared<Null>();
	}
	void set(const std::string _name, const std::shared_ptr<Object> v) {
		values[_name] = Item{Item::AccessToken::Public, v};
	}
	std::shared_ptr<Object> getThere(const std::string _name) const {
		if (_name == "_parent") return _parent;
		auto it = values.find(_name);
		if (it != values.end()) return it->second.value;
		else return std::make_shared<Null>();
	}
	void create(const std::string _name, const std::shared_ptr<Object> v) {
		if (globalLock) {
			global_lock_error();
			return;
		}
		auto it = values.find(_name);
		if (it == values.end()) {
			values.insert(std::make_pair(_name, Item{Item::AccessToken::Public, v}));
		}
	}
	Item::AccessToken getAT(const std::string _name) const {
		auto it = values.find(_name);
		if (it != values.end()) return it->second._access;
		else return Item::AccessToken::Public;
	}
	void setAT(const std::string _name, const Item::AccessToken _at) {
		if (globalLock) {
			global_lock_error();
			return;
		}
		auto it = values.find(_name);
		if (it != values.end()) it->second._access = _at;
	}
	void remove(const std::string _name) {
		if (globalLock) {
			global_lock_error();
			return;
		}
		auto it = values.find(_name);
		if (it != values.end()) values.erase(_name);
		else if (_parent) _parent->remove(_name);
	}
public:
	std::string toString() const override {
		std::stringstream ss;
		ss << "{";
		for (auto it = values.begin(); it != values.end(); it++) {
			if (it != values.begin()) {
				ss << ",";
			}
			ss << it->first << ":" << it->second.value->toString();
		}
		ss << "}";
		return ss.str();
	}

	std::shared_ptr<Object> copy() const override {
		std::shared_ptr<Environment> result(_parent);
		for (auto it = values.begin(); it != values.end(); it++) {
			result->values.insert(std::make_pair(it->first, Item{it->second._access, it->second.value->copy()}));
		}
		return result;
	}

	std::string idstr() const override {
		return "object";
	}

	void assign(std::shared_ptr<Object> value) override {
		*this = *std::dynamic_pointer_cast<Environment>(value->copy());
	}

	void storeInto(std::ostream& os) const override {
		BinaryOut::write_byte(os, char(Object::Type::Environment));
	}

	void readFrom(std::istream& is) override {}
public:
	Environment(std::shared_ptr<Environment> _parent = nullptr) : _parent(_parent), globalLock(false), Object(Object::Type::Environment) {}
};