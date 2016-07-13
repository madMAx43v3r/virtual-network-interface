/*
 * AST.h
 *
 *  Created on: May 21, 2016
 *      Author: mad
 */

#ifndef INCLUDE_AST_H_
#define INCLUDE_AST_H_

#include <set>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <typeinfo>
#include <assert.h>

#include "CRC64.h"

#define FAIL() assert(false);

#define ERROR(msg) error() << #msg << endl; assert(false); exit(-1);

using namespace std;

namespace vni {
namespace codegen {

class Package;
class Base;
class Type;


static map<string, Base*> INDEX;

static string CURR_FILE = "<unknown>";
static int CURR_LINE = -1;

static Package* PACKAGE = 0;
static map<string, string> IMPORT;


inline uint64_t hash64(const std::string& str) {
	CRC64 hash;
	hash.update(str.c_str(), str.size());
	return hash.getValue();
}

std::ostream& error() {
	return cerr << CURR_FILE << ":" << CURR_LINE << ": error: ";
}



class Base {
public:
	string file;
	string line;
	
	Base() {
		file = CURR_FILE;
		line = CURR_LINE;
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

std::ostream& error(Base* scope) {
	return cerr << scope->file << ":" << scope->line << ": error: ";
}


class Package : public Base {
public:
	string name;
	map<string, Base*> index;
	
	Package(string name) : name(name) {}
	
	string get_name() {	return name; }
	string get_full_name() { return name; }
	
	static Package* get(string name) {
		Base* base = INDEX[name];
		if(!base) {
			Package* pack = new Package(name);
			INDEX[name] = pack;
			return pack;
		}
		Package* pack = dynamic_cast<Package*>(base);
		if(!pack) {
			cout << "ERROR: invalid package name: " << name << endl;
			FAIL();
		}
		return pack;
	}
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
	virtual uint64_t get_hash() { return hash64("vni.ast.integer"); }
};

class Real : public Primitive {
public:
	string name;
	int size = 0;
	
	Real(string name, int size) : name(name), size(size) {}
	
	virtual string get_name() { return name; }
	virtual string get_full_name() { return name; }
	virtual uint64_t get_hash() { return hash64("vni.ast.real"); }
};


class Array : public Base {
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
		hash.update("vni.ast.array");
		hash.update(type->get_hash());
		return hash.getValue();
	}
};


class Field : public Base {
public:
	Base* type = 0;
	vector<Base*> tmpl_types;
	string name;
	string value;
	bool is_const = false;
	
	virtual string get_name() { return name; }
	virtual string get_full_name() { return type->get_full_name() + " " + name; }
	virtual uint64_t get_hash() { return hash64(name); }
};


class Method : public Base {
public:
	Base* type = 0;
	vector<Base*> tmpl_types;
	
	string name;
	vector<Field*> params;
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
		CRC64 hash;
		hash.update(name);
		hash.update(params.size());
		return hash.getValue();
	}
};


class Type : public Base {
public:
	Package* package;
	string name;
	
	set<string> imports;
	
	Type(string name) : name(name) {
		package = PACKAGE;
		package->index[name] = this;
		string full = package->name + "." + name;
		if(INDEX.count(full)) {
			error() << "duplicate type: " << full << endl;
			FAIL();
		}
		INDEX[full] = this;
		for(auto& entry : IMPORT) {
			imports.insert(entry.second);
		}
	}
	
	virtual string get_name() { return name; }
	virtual string get_full_name() { return package->get_full_name() + "." + get_name(); }
};


class Enum : public Type {
public:
	vector<string> values;
	
	Enum(string name) : Type(name) {}
	
};


class Struct : public Type {
public:
	vector<Field*> constants;
	vector<Field*> fields;
	vector<Field*> all_fields;
	
	Struct(string name) : Type(name) {}
	
};


class Class : public Struct {
public:
	Class* super = 0;
	
	Class(string name) : Struct(name) {}
	
};


class Interface : public Type {
public:
	Interface* super = 0;
	vector<string> generic;
	
	vector<Method*> methods;
	vector<Method*> all_methods;
	
	Interface(string name) : Type(name) {}
	
};


class Object : public Interface {
public:
	Object* super = 0;
	vector<Interface*> implements;
	
	vector<Field*> constants;
	vector<Field*> fields;
	vector<Field*> all_fields;
	
	Object(string name) : Interface(name) {}
	
};


class Node : public Object {
public:
	
	Node(string name) : Object(name) {}
	
};



static Base* resolve(const string& ident);

template<typename T>
static T* resolve(const string& ident) {
	Base* res = INDEX[ident];
	if(!res && IMPORT.count(ident)) {
		res = resolve(IMPORT[ident]);
	}
	if(!res) {
		res = PACKAGE->index[ident];
	}
	T* type = dynamic_cast<T*>(res);
	if(!type) {
		error() << "unable to resolve: " << ident << std::endl;
		FAIL();
	}
	return type;
}

static Base* resolve(const string& ident) {
	return resolve<Base>(ident);
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





}}

#endif /* INCLUDE_AST_H_ */
