#pragma once

#include "object/null.hpp"

#include <sstream>

class Item {
public:
    enum class AccessToken {
        Private, Public
    } _access;
    std::shared_ptr<Object> value;
};

class Environment {
public:
    std::shared_ptr<Environment> _parent;
    std::map<std::string, Item> values;
    bool globalLock;
public:
    std::shared_ptr<Object> get(const std::string _name) const {
        auto it = values.find(_name);
        if (it != values.end()) return it->second.value;
        else if (_parent) return _parent->get(_name);
        else return std::make_shared<Null>();
    }
    void set(const std::string _name, const std::shared_ptr<Object> v) {
        values[_name] = Item{Item::AccessToken::Public, v};
    }
    std::shared_ptr<Object> getThere(const std::string _name) const {
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
            values.insert(std::make_pair(_name, Item{Item::AccessToken::Public, v->copy()}));
        }
    }
    Item::AccessToken getAT(const std::string _name) const {
        auto it = values.find(_name);
        if (it != values.end()) return it->second._access;
        else return Item::AccessToken::Public;
    }
    void setAT(const std::string _name, const Item::AccessToken _at) {
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
    std::string toString() const {
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

	void copyFrom(std::shared_ptr<Environment> env) {
		_parent = env->_parent;
		for (auto it = env->values.begin(); it != env->values.end(); it++) {
			values.insert(std::make_pair(it->first, Item{it->second._access, it->second.value->copy()}));
		}
	}
public:
    Environment(std::shared_ptr<Environment> _parent = nullptr) : _parent(_parent), globalLock(false) {
        // values.insert(std::make_pair("super", Item{Item::AccessToken::Public, _parent}));
    }
};