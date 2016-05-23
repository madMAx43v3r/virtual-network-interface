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
using namespace std;


class Base {
public:
	Base() {}
	
	virtual ~Base() {}
	
	string package;
	string name;
	
	Base* super = 0;
	uint64_t super_hash = 0;
	vector<string> super_generics;
	vector<pair<uint64_t, Base*> > super_bound;
	
	bool isextern = false;
	bool isgeneric = false;
	vector<string> generics;
	vector<pair<uint64_t, Base*> > bound;
	
	map<uint64_t, Field*> constants;
	
	string full_name() {
		return package + "." + name;
	}
	
	virtual uint64_t get_hash() {
		string str = full_name();
		if(!isgeneric && bound.size()) {
			std::ostringstream ss;
			ss << "<";
			int i = 0;
			for(auto& param : bound) {
				ss << NAMES[param.first];
				if(i < bound.size()-1) {
					ss << ",";
				}
				i++;
			}
			ss << ">";
		}
		uint64_t val = hash64(str);
		NAMES[val] = str;
		return val;
	}
	
};


class Primitive : public Base {
public:
	bool isintegral = true;
	uint64_t class_hash = 0;
};

class Bool : public Primitive {
public:
	Bool() { class_hash = HASH; }
	static const uint64_t HASH = hash64("bool");
	virtual uint64_t get_hash() override { return HASH; }
};

class Char : public Primitive {
public:
	Char() { class_hash = hash64("vnl_integer"); }
	static const uint64_t HASH = hash64("char");
	virtual uint64_t get_hash() override { return HASH; }
};

class Short : public Primitive {
public:
	Short() { class_hash = hash64("vnl_integer"); }
	static const uint64_t HASH = hash64("short");
	virtual uint64_t get_hash() override { return HASH; }
};

class Int : public Primitive {
public:
	Int() { class_hash = hash64("vnl_integer"); }
	static const uint64_t HASH = hash64("int");
	virtual uint64_t get_hash() override { return HASH; }
};

class Long : public Primitive {
public:
	Long() { class_hash = hash64("vnl_integer"); }
	static const uint64_t HASH = hash64("long");
	virtual uint64_t get_hash() override { return HASH; }
};

class Float : public Primitive {
public:
	Float() { class_hash = hash64("vnl_real"); }
	static const uint64_t HASH = hash64("float");
	virtual uint64_t get_hash() override { return HASH; }
};

class Double : public Primitive {
public:
	Double() { class_hash = hash64("vnl_real"); }
	static const uint64_t HASH = hash64("double");
	virtual uint64_t get_hash() override { return HASH; }
};


class Array : public Base {
public:
	Base* type = 0;
	uint64_t type_hash = 0;
	int size = 0;
	
	virtual uint64_t get_hash() override {
		std::ostringstream ss;
		ss << NAMES[type_hash] << "[" << size << "]";
		string str = ss.str();
		uint64_t val = hash64(str);
		NAMES[val] = str;
		return val;
	}
	
};


class Binary : public Base {
public:
	static const uint64_t HASH = hash64("Binary");
	virtual uint64_t get_hash() override { return HASH; }
};


class String : public Base {
public:
	static const uint64_t HASH = hash64("String");
	virtual uint64_t get_hash() override { return HASH; }
};


class TmplType : public Base {
public:
	int index = -1;
	
	static uint64_t get_index_hash(int index) {
		std::ostringstream ss;
		ss << "Tmpl_" << index;
		string str = ss.str();
		uint64_t val = hash64(str);
		NAMES[val] = str;
		return val;
	}
	
	virtual uint64_t get_hash() override {
		return get_index_hash(index);
	}
};


class Enum : public Base {
public:
	map<uint32_t, string> values;
	
	uint32_t get_value_hash(string ident) {
		return hash64(full_name() + "." + ident);
	}
	
};


class Field : public Base {
public:
	Base* type = 0;
	uint64_t type_hash = 0;
	
	bool isnull = false;
	string value;
	
	uint64_t get_unique_hash() {
		return hash64(name);
	}
	
	virtual uint64_t get_hash() override {
		CRC64 hash;
		hash.update(package);
		hash.update(type_hash);
		hash.update(name);
		for(auto entry : bound) {
			hash.update(entry.first);
		}
		return hash.getValue();
	}
	
};


class Param : public Base {
public:
	Base* type = 0;
	uint64_t type_hash = 0;
};

class Method : public Base {
public:
	Base* type = 0;
	uint64_t type_hash = 0;
	
	vector<Param*> params;
	
	uint64_t get_unique_hash() {
		CRC64 hash;
		hash.update(name);
		for(Param* param : params) {
			hash.update(param->type_hash);
			for(auto entry : param->bound) {
				hash.update(entry.first);
			}
		}
		return hash.getValue();
	}
	
	virtual uint64_t get_hash() override {
		CRC64 hash;
		hash.update(package);
		hash.update(type_hash);
		hash.update(name);
		for(Param* param : params) {
			hash.update(param->type_hash);
			for(auto entry : param->bound) {
				hash.update(entry.first);
			}
		}
		return hash.getValue();
	}
	
};


class Typedef : public Base {
public:
	Base* type = 0;
	uint64_t type_hash = 0;
};


class Type : public Base {
public:
	map<uint64_t, Field*> fields;
	
	bool add_field(Field* field) {
		uint64_t hash = field->get_unique_hash();
		if(!fields.count(hash)) {
			fields[hash] = field;
			return true;
		}
		return false;
	}
	
};


class Interface : public Base {
public:
	map<uint64_t, Method*> methods;
	
	bool add_method(Method* method) {
		uint64_t hash = method->get_unique_hash();
		if(!methods.count(hash)) {
			methods[hash] = method;
			return true;
		}
		return false;
	}
	
};

















#endif /* INCLUDE_AST_H_ */
