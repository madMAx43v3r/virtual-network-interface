/*
 * AST.h
 *
 *  Created on: May 21, 2016
 *      Author: mad
 */

#ifndef INCLUDE_AST_H_
#define INCLUDE_AST_H_

#include <map>
#include <vector>
#include <string>
#include <iostream>

#include "CRC64.h"


using namespace std;

class Package;
class Type;


static map<uint64_t, string> NAMES;
static map<uint64_t, Base*> INDEX;

static string FILE = "<unknown>";
static string LINE = -1;

static Package* PACKAGE = 0;
static Type* TYPE = 0;


inline uint64_t hash64(const std::string& str) {
	CRC64 hash;
	hash.update(str.c_str(), str.size());
	uint64_t val = hash.getValue();
	NAMES[val] = str;
	return val;
}

inline void die() {
	die(FILE, LINE);
}

inline void die(string file, string line) {
	std::cout << "In " << file << ":" << line << std::endl;
	exit(-1);
}


class Base {
public:
	string file;
	string line;
	
	Base() {
		file = FILE;
		line = LINE;
	}
	virtual ~Base() {}
	
	virtual Base* get() { return this; }
	
	virtual string get_name() = 0;
	virtual string get_full_name() = 0;
	
	virtual uint64_t get_hash() {
		return hash64(get_full_name());
	}
	
	virtual bool check() { return true; }
	
};


class Package : public Base {
public:
	string name;
	map<string, Base*> index;
	
	Package(string name) : name(name) {}
	
	void add(Base* type) {
		index[type->get_name()] = type;
	}
	
	string get_name() {	return name; }
	string get_full_name() { return name; }
	
	static Package* get(string name) {
		uint64_t hash = hash64(name);
		Package* res = INDEX[hash];
		if(!res) {
			res = new Package(name);
			INDEX[hash] = res;
		}
		return res;
	}
};


enum {
	BYTE = 1, WORD = 2, DWORD = 4, QWORD = 8
};

class Void : public Base {
	virtual string get_name() { return "void"; }
	virtual string get_full_name() { return "void"; }
};

class Primitive : public Base {
	
};

class Bool : public Primitive {
public:
	virtual string get_name() { return "bool"; }
	virtual string get_full_name() { return "bool"; }
};

class Integer : public Primitive {
public:
	string name;
	int size = 0;
	
	Integer(string name, int size) : name(name), size(size) {}
	
	virtual string get_name() { return name; }
	virtual string get_full_name() { return name; }
	virtual uint64_t get_hash() { return hash64("vni.ast.Integer"); }
};

class Real : public Primitive {
public:
	string name;
	int size = 0;
	
	Real(string name, int size) : name(name), size(size) {}
	
	virtual string get_name() { return name; }
	virtual string get_full_name() { return name; }
	virtual uint64_t get_hash() { return hash64("vni.ast.Real"); }
};


class Array : public Primitive {
public:
	Base* type = 0;
	int size = 0;
	
	virtual string get_name() {
		std::ostringstream ss;
		ss << type->get_name() << "[" << size << "]";
		return ss.str();
	}
	virtual string get_full_name() {
		std::ostringstream ss;
		ss << type->get_full_name() << "[" << size << "]";
		return ss.str();
	}
	virtual uint64_t get_hash() {
		CRC64 hash;
		hash.update("vni.ast.Array");
		hash.update(type->get_hash());
		return hash.getValue();
	}
};


class Field : public Base {
public:
	Base* type = 0;
	string name;
	string value;
	
	virtual string get_name() { return name; }
	virtual string get_full_name() { return type->get_full_name() + " " + name; }
	virtual uint64_t get_hash() { return hash64(name); }
};


class Param : public Base {
public:
	Base* type = 0;
	string name;
	
	virtual string get_name() { return name; }
	virtual string get_full_name() { return type->get_full_name() + " " + name; }
	virtual uint64_t get_hash() { return hash64(name); }
};


class Method : public Base {
public:
	Base* type = 0;
	string name;
	vector<Param*> params;
	bool is_const = false;
	
	virtual string get_name() { return name; }
	virtual string get_full_name() {
		string str = type->get_name() + " " + name + "(";
		for(int i = 0; i < params.size(); ++i) {
			str += params[i]->type->get_full_name();
			if(i < params.size()-1) {
				str += ", ";
			}
		}
		return str + ")";
	}
	virtual uint64_t get_hash() {
		return hash64(name);
	}
};


class Type : public Base {
public:
	Package* package;
	string name;
	
	Type(string name) : name(name) {
		package = PACKAGE;
		package->index[name] = this;
	}
	
	virtual string get_name() { return name; }
	virtual string get_full_name() { return package->get_full_name() + "." + get_name(); }
};


class Enum : public Type {
public:
	string name;
	vector<string> values;
	
};


class Struct : public Type {
public:
	vector<Field*> fields;
	vector<Field*> constants;
	
};


class Class : public Struct {
public:
	Type* super = 0;
	
};


class Interface : public Class {
public:
	vector<string> params;
	vector<Method*> methods;
	
	Interface() {
		
	}
};


class Object : public Interface {
public:
	
	
};


class Node : public Object {
public:
	
	
};




static Base* resolve(const string& ident) {
	Base* res = INDEX[hash64(ident)];
	if(!res) {
		res = PACKAGE->index[ident];
	}
	if(!res) {
		std::cout << "Unable to resolve: " << ident << std::endl;
		die();
	}
	return res;
}



struct init_type_system {
	init_type_system() {
		add(new Void());
		add(new Bool());
		add(new Integer("char", 1));
		add(new Integer("short", 2));
		add(new Integer("int", 4));
		add(new Integer("long", 8));
		add(new Real("float", 4));
		add(new Real("double", 8));
	}
	void add(Base* type) {
		INDEX[type->get_name()] = type;
	}
};

static init_type_system init_type_system_;












#endif /* INCLUDE_AST_H_ */
