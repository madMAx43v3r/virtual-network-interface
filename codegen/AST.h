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

#define ERROR(msg) { error() << msg << endl << endl; assert(false); exit(-1); }

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
static Type* TYPE = 0;
static map<string, string> IMPORT;


inline uint64_t hash64(const std::string& str) {
	CRC64 hash;
	hash.update(str.c_str(), str.size());
	return hash.getValue();
}

std::ostream& error() {
	return cerr << CURR_FILE << ":" << CURR_LINE << ": error: ";
}

static Base* resolve(const string& ident);


class Base {
public:
	string file;
	int line;
	
	bool generate = true;
	
	Base() {
		file = CURR_FILE;
		line = CURR_LINE;
	}
	virtual ~Base() {}
	
	virtual Base* get() { return this; }
	
	virtual string get_name() = 0;
	
	virtual uint64_t get_hash() { return hash64(get_name()); }
	
	virtual void pre_compile() {}
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
};

class Primitive : public Base {
	
};

class Bool : public Primitive {
public:
	virtual string get_name() { return "bool"; }
};

class Integer : public Primitive {
public:
	string name;
	int size = 0;
	
	Integer(string name, int size) : name(name), size(size) {}
	
	virtual string get_name() { return name; }
};

class Real : public Primitive {
public:
	string name;
	int size = 0;
	
	Real(string name, int size) : name(name), size(size) {}
	
	virtual string get_name() { return name; }
};


class Vector : public Base {
public:
	Base* type = 0;
	int size = 0;
	
	virtual string get_name() {
		std::ostringstream ss;
		ss << type->get_name() << "[" << size << "]";
		return ss.str();
	}
};


class TypeName : public Base {
public:
	Base* type = 0;
	vector<Base*> tmpl;
	bool is_const = false;
	
	TypeName() {}
	TypeName(Base* type) : type(type) {}
	
	virtual string get_name() { return type->get_name(); }
	
};


class Field : public TypeName {
public:
	string name;
	string value;
	
	virtual string get_name() { return name; }
	virtual uint64_t get_hash() { return hash64(name); }
};


class Method : public TypeName {
public:
	string name;
	vector<Field*> params;
	bool is_handle = false;
	
	virtual string get_name() { return name; }
	virtual uint64_t get_hash() {
		CRC64 hash;
		hash.update(name);
		hash.update((uint64_t)params.size());
		return hash.getValue();
	}
};


class Generic : public Base {
public:
	string name;
	
	Generic(string name) : name(name) {}
	
	virtual string get_name() { return name; }
	
};


class Type : public Base {
public:
	Package* package;
	string name;
	
	TypeName* super = 0;
	
	set<Type*> imports;
	
	vector<Field*> constants;
	
	Type(string name) : name(name) {
		package = PACKAGE;
		package->index[name] = this;
		string full = get_full_name();
		if(INDEX.count(full)) {
			error() << "duplicate type: " << full << endl;
			FAIL();
		}
		INDEX[full] = this;
	}
	
	void import(Type* type);
	void import(TypeName* name);
	
	virtual string get_name() { return name; }
	virtual string get_full_name() { return package->name + "." + name; }
	virtual uint64_t get_hash() { return hash64(get_full_name()); }
	
};


class Extern : public Type {
public:
	Extern(string name) : Type(name) {
		generate = false;
	}
	
};


class Enum : public Type {
public:
	vector<string> values;
	
	Enum(string name) : Type(name) {}
	
	virtual void pre_compile();
	
};


class Struct : public Type {
public:
	vector<Field*> fields;
	vector<Field*> all_fields;
	
	Struct(string name) : Type(name) {}
	
	virtual void pre_compile();
	virtual void compile();
	
};


class Class : public Struct {
public:
	set<Class*> sub_types;
	
	Class(string name) : Struct(name) {}
	
	virtual void pre_compile();
	virtual void compile();
	
};


class Interface : public Type {
public:
	vector<Generic*> generic;
	
	vector<Field*> fields;
	vector<Field*> all_fields;
	
	vector<Method*> methods;
	
	Interface(string name) : Type(name) {}
	
	virtual void pre_compile();
	virtual void compile();
	
};


class Object : public Interface {
public:
	vector<Interface*> implements;
	
	set<Class*> handles;
	
	Object(string name) : Interface(name) {}
	
	virtual void pre_compile();
	virtual void compile();
	
	void add_handle_class(Class* p_class) {
		handles.insert(p_class);
		for(Class* sub : p_class->sub_types) {
			handles.insert(sub);
			import(sub);
		}
	}
	
};



template<typename T>
static T* resolve(const string& ident) {
	Base* res = INDEX[ident];
	if(!res && TYPE) {
		Interface* p_iface = dynamic_cast<Interface*>(TYPE);
		if(p_iface) {
			for(Generic* gen : p_iface->generic) {
				if(gen->name == ident) {
					res = gen;
				}
			}
		}
	}
	if(!res && IMPORT.count(ident)) {
		res = resolve(IMPORT[ident]);
	}
	if(!res && PACKAGE) {
		res = PACKAGE->index[ident];
	}
	if(!res && INDEX["vnl"]) {
		res = ((Package*)INDEX["vnl"])->index[ident];
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


inline void check_dup_fields(vector<Field*>& fields) {
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

inline vector<Method*> get_unique_methods(vector<Method*>& methods) {
	map<uint64_t, Method*> map;
	for(Method* method : methods) {
		uint64_t hash = method->get_hash();
		if(method->is_handle) {
			hash ^= method->params[0]->type->get_hash();
		}
		map[hash] = method;
	}
	vector<Method*> res;
	for(auto& entry : map) {
		res.push_back(entry.second);
	}
	return res;
}

template<typename T>
void gather_fields(T* next, vector<Field*>& vec) {
	if(next) {
		if(next->super) {
			gather_fields(dynamic_cast<T*>(next->super->type), vec);
		}
		vec.insert(vec.end(), next->fields.begin(), next->fields.end());
	}
}

template<typename T>
void gather_methods(T* next, vector<Method*>& vec) {
	if(next) {
		if(next->super) {
			gather_methods(dynamic_cast<T*>(next->super->type), vec);
		}
		vec.insert(vec.end(), next->methods.begin(), next->methods.end());
	}
}


void Type::import(Type* p_type) {
	imports.insert(p_type);
}

void Type::import(TypeName* p_name) {
	Type* p_type = dynamic_cast<Type*>(p_name->type);
	Vector* p_vector = dynamic_cast<Vector*>(p_name->type);
	if(p_type) {
		import(p_type);
	}
	if(p_vector) {
		Type* p_vector_type = dynamic_cast<Type*>(p_vector->type);
		if(p_vector_type) {
			import(p_vector_type);
		}
	}
	for(Base* param : p_name->tmpl) {
		TypeName* p_param_name = dynamic_cast<TypeName*>(param);
		Type* p_param_type = dynamic_cast<Type*>(param);
		if(p_param_name) {
			import(p_param_name);
		} else if(p_param_type) {
			import(p_param_type);
		}
	}
}


void Enum::pre_compile() {
	if(!super && get_full_name() != "vnl.Enum") {
		super = new TypeName(resolve<Enum>("vnl.Enum"));
	}
}


void Struct::pre_compile() {
	if(!super && get_full_name() != "vnl.Value") {
		super = new TypeName(resolve<Struct>("vnl.Value"));
	}
}

void Struct::compile() {
	Type::compile();
	for(Field* field : fields) {
		import(field);
	}
	gather_fields(this, all_fields);
	check_dup_fields(all_fields);
}


void Class::pre_compile() {
	if(!super && get_full_name() != "vnl.Value") {
		super = new TypeName(resolve<Class>("vnl.Value"));
	}
}

void Class::compile() {
	Struct::compile();
	if(super) {
		import(super);
	}
	TypeName* next = super;
	while(next) {
		Class* p_class = dynamic_cast<Class*>(next->type);
		if(p_class) {
			p_class->sub_types.insert(this);
			next = p_class->super;
		} else {
			next = 0;
		}
	}
	all_fields.clear();
	gather_fields(this, all_fields);
	check_dup_fields(all_fields);
}


void Interface::pre_compile() {
	if(!super && get_full_name() != "vnl.Interface") {
		super = new TypeName(resolve<Interface>("vnl.Interface"));
	}
}

void Interface::compile() {
	Type::compile();
	if(super) {
		import(super);
	}
	for(Field* field : fields) {
		import(field);
	}
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
	}
	gather_fields(this, all_fields);
	check_dup_fields(all_fields);
	methods = get_unique_methods(methods);
}


void Object::pre_compile() {
	if(!super && get_full_name() != "vnl.Object") {
		super = new TypeName(resolve<Object>("vnl.Object"));
	}
	Interface::pre_compile();
	for(Interface* iface : implements) {
		methods.insert(methods.end(), iface->methods.begin(), iface->methods.end());
	}
}

void Object::compile() {
	Interface::compile();
	all_fields.clear();
	gather_fields(this, all_fields);
	check_dup_fields(all_fields);
	methods = get_unique_methods(methods);
	for(Method* method : methods) {
		if(method->is_handle) {
			if(method->params.size() != 1) {
				error(method) << "handle() method must have one parameter" << endl;
				FAIL();
			}
			if(method->is_const) {
				error(method) << "handle() method cannot be const" << endl;
				FAIL();
			}
			if(!dynamic_cast<Class*>(method->params[0]->type)) {
				error(method) << "handle() method can only take a class" << endl;
				FAIL();
			}
			Class* p_class = dynamic_cast<Class*>(method->params[0]->type);
			if(p_class) {
				add_handle_class(p_class);
			}
		}
	}
}



inline void add_type(Base* type) {
	type->generate = false;
	INDEX[type->get_name()] = type;
}

inline void init_type_system() {
	add_type(new Void());
	add_type(new Bool());
	add_type(new Integer("char", 1));
	add_type(new Integer("short", 2));
	add_type(new Integer("int", 4));
	add_type(new Integer("long", 8));
	add_type(new Real("float", 4));
	add_type(new Real("double", 8));
	PACKAGE = Package::get("vnl");
	add_type(new Type("Type"));
	PACKAGE = 0;
}



}}

#endif /* INCLUDE_AST_H_ */
