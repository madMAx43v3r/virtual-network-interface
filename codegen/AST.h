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

#define FAIL() { cout << endl; assert(false); }

#define ERROR(msg) { error() << #msg << endl << endl; assert(false); exit(-1); }

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
	
	virtual void compile() {}
	
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
};

class Real : public Primitive {
public:
	string name;
	int size = 0;
	
	Real(string name, int size) : name(name), size(size) {}
	
	virtual string get_name() { return name; }
	virtual string get_full_name() { return name; }
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
};


class TypeName : public Base {
public:
	Base* type = 0;
	vector<Base*> tmpl;
	
};


class Field : public TypeName {
public:
	string name;
	string value;
	bool is_const = false;
	
	virtual string get_name() { return name; }
	virtual string get_full_name() { return name; }
	virtual uint64_t get_hash() { return hash64(name); }
};


class Method : public TypeName {
public:
	string name;
	vector<Field*> params;
	bool is_const = false;
	bool is_handle = false;
	
	virtual string get_name() { return name; }
	virtual string get_full_name() { return name; }
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
	
	void import(Type* type);
	void import(TypeName* name);
	
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
	
	virtual void compile();
	
};


class Class : public Struct {
public:
	Class* super = 0;
	
	Class(string name) : Struct(name) {}
	
	virtual void compile();
	
};


class Interface : public Type {
public:
	Interface* super = 0;
	vector<string> generic;
	
	vector<Method*> methods;
	vector<Method*> all_methods;
	
	Interface(string name) : Type(name) {}
	
	virtual void compile();
	
};


class Object : public Interface {
public:
	Object* super = 0;
	vector<Interface*> implements;
	
	vector<Field*> constants;
	vector<Field*> fields;
	vector<Field*> all_fields;
	
	Object(string name) : Interface(name) {}
	
	virtual void compile();
	
};


class Module : public Object {
public:
	
	Module(string name) : Object(name) {}
	
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


void check_dup_fields(vector<Field*>& fields) {
	map<uint64_t, Field*> map;
	for(Field* field : fields) {
		uint64_t hash = field->get_hash();
		if(map.count(hash)) {
			error(field) << "duplicate field: " << field->name << endl;
			FAIL();
		}
		map[hash] = field;
	}
}

vector<Method*> get_unique_methods(vector<Method*>& methods) {
	map<uint64_t, Method*> map;
	for(Method* method : methods) {
		uint64_t hash = method->get_hash();
		map[hash] = method;
	}
	vector<Method*> res;
	for(auto& entry : map) {
		res.push_back(entry.second);
	}
	return res;
}


void Type::import(Type* p_type) {
	imports.insert(p_type->package->name + "." + p_type->name);
}

void Type::import(TypeName* p_name) {
	Type* p_type = dynamic_cast<Type*>(p_name->type);
	Interface* p_iface = dynamic_cast<Interface*>(p_name->type);
	if(p_type) {
		import(p_type);
		for(Base* param : p_name->tmpl) {
			Type* p_param_type = dynamic_cast<Type*>(param);
			TypeName* p_param_name = dynamic_cast<TypeName*>(param);
			if(p_param_name) {
				import(p_param_name);
			} else {
				import(p_param_type);
			}
		}
	}
	if(p_name->tmpl.size() && (!p_iface || p_iface->generic.empty())) {
		error(p_name) << "is not a template" << endl;
		FAIL();
	}
}


void Struct::compile() {
	Type::compile();
	for(Field* field : fields) {
		import(field);
	}
	check_dup_fields(fields);
	all_fields = fields;
}

void Class::compile() {
	Struct::compile();
	Class* next = super;
	while(next) {
		all_fields.insert(all_fields.end(), next->fields.begin(), next->fields.end());
		next = next->super;
	}
}

void Interface::compile() {
	Type::compile();
	for(Method* method : methods) {
		import(method);
		for(Field* param : method->params) {
			import(param);
		}
		if(method->is_const && method->type->get_name() == "void") {
			error(method) << "const method cannot return void" << endl;
			FAIL();
		}
		if(!method->is_const && method->type->get_name() != "void") {
			error(method) << "non-const method must return void" << endl;
			FAIL();
		}
		if(method->name == "handle") {
			if(method->params.size() != 1) {
				error(method) << "handle() method must have one parameter" << endl;
				FAIL();
			}
			if(method->is_const) {
				error(method) << "handle() method cannot be const" << endl;
				FAIL();
			}
			method->is_handle = true;
		}
	}
	all_methods = methods;
	Interface* next = super;
	while(next) {
		all_methods.insert(all_methods.end(), next->methods.begin(), next->methods.end());
		next = next->super;
	}
	all_methods = get_unique_methods(all_methods);
}

void Object::compile() {
	Interface::compile();
	for(Field* field : fields) {
		import(field);
	}
	check_dup_fields(fields);
	all_fields = fields;
	Object* next = super;
	while(next) {
		all_fields.insert(all_fields.end(), next->fields.begin(), next->fields.end());
		all_methods.insert(all_methods.end(), next->methods.begin(), next->methods.end());
		for(Interface* iface : next->implements) {
			all_methods.insert(all_methods.end(), iface->methods.begin(), iface->methods.end());
		}
		next = next->super;
	}
	for(Interface* iface : implements) {
		all_methods.insert(all_methods.end(), iface->methods.begin(), iface->methods.end());
	}
	all_methods = get_unique_methods(all_methods);
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
