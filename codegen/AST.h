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


map<uint64_t, string> 		NAMES;
map<uint64_t, Base*> 		INDEX;


uint64_t hash64(const std::string& str) {
	CRC64 hash;
	hash.update(str.c_str(), str.size());
	uint64_t val = hash.getValue();
	NAMES[val] = str;
	return val;
}


class Base {
public:
	Base() {}
	virtual ~Base() {}
	
	virtual string get_name() = 0;
	virtual string get_full_name() = 0;
	
	virtual uint64_t get_hash() {
		return hash64(get_full_name());
	}
	
};


class Ref {
public:
	Base* type = 0;
	uint64_t hash = 0;
	string name;
	vector<Ref> tmpl;
	
	string get_name() {
		return type->get_name();
	}
	string get_full_name() {
		return type->get_full_name();
	}
};


class Primitive : public Base {
	
};

class Integer : public Primitive {
public:
	int size = 0;
	virtual string get_name() { return "Integer"; }
	virtual string get_full_name() { return "vni.Integer"; }
};

class Real : public Primitive {
public:
	int size = 0;
	virtual string get_name() { return "Real"; }
	virtual string get_full_name() { return "vni.Real"; }
};

class Bool : public Primitive {
public:
	static const uint64_t HASH = hash64("bool");
	virtual string get_name() { return "bool"; }
	virtual string get_full_name() { return get_name(); }
	virtual uint64_t get_hash() { return HASH; }
};

class Char : public Integer {
public:
	Char() { size = 1; }
	static const uint64_t HASH = hash64("char");
	virtual string get_name() { return "char"; }
	virtual string get_full_name() { return get_name(); }
	virtual uint64_t get_hash() { return HASH; }
};

class Short : public Integer {
public:
	Short() { size = 2; }
	static const uint64_t HASH = hash64("short");
	virtual string get_name() { return "short"; }
	virtual string get_full_name() { return get_name(); }
	virtual uint64_t get_hash() { return HASH; }
};

class Int : public Integer {
public:
	Int() { size = 4; }
	static const uint64_t HASH = hash64("int");
	virtual string get_name() { return "int"; }
	virtual string get_full_name() { return get_name(); }
	virtual uint64_t get_hash() { return HASH; }
};

class Long : public Integer {
public:
	Long() { size = 8; }
	static const uint64_t HASH = hash64("long");
	virtual string get_name() { return "long"; }
	virtual string get_full_name() { return get_name(); }
	virtual uint64_t get_hash() { return HASH; }
};

class Float : public Real {
public:
	Float() { size = 4; }
	static const uint64_t HASH = hash64("float");
	virtual string get_name() { return "float"; }
	virtual string get_full_name() { return get_name(); }
	virtual uint64_t get_hash() { return HASH; }
};

class Double : public Real {
public:
	Double() { size = 8; }
	static const uint64_t HASH = hash64("double");
	virtual string get_name() { return "double"; }
	virtual string get_full_name() { return get_name(); }
	virtual uint64_t get_hash() { return HASH; }
};


class Array : public Base {
public:
	Ref type;
	int size = 0;
	
	virtual string get_name() {
		std::ostringstream ss;
		ss << type.get_name() << "[" << size << "]";
		return ss.str();
	}
	
	virtual string get_full_name() {
		std::ostringstream ss;
		ss << type.get_full_name() << "[" << size << "]";
		return ss.str();
	}
	
};


class Binary : public Base {
public:
	static const uint64_t HASH = hash64("Binary");
	virtual string get_name() { return "Binary"; }
	virtual string get_full_name() { return get_name(); }
	virtual uint64_t get_hash() { return HASH; }
};


class String : public Base {
public:
	static const uint64_t HASH = hash64("String");
	virtual string get_name() { return "String"; }
	virtual string get_full_name() { return get_name(); }
	virtual uint64_t get_hash() { return HASH; }
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


class Field : public Base {
public:
	Ref type;
	string name;
	
	bool isnull = false;
	string value;
	
	virtual string get_name() { return name; }
	virtual string get_full_name() { return get_name(); }
	
	virtual uint64_t get_hash() override {
		CRC64 hash;
		hash.update(type.hash);
		hash.update(name);
		for(auto entry : bound) {
			hash.update(entry.first);
		}
		return hash.getValue();
	}
	
};


class Param : public Base {
public:
	Ref type;
	string name;
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


class Type : public Base {
public:
	string package;
	string name;
	
	bool isextern = false;
	bool isgeneric = false;
	vector<string> generics;
	vector<Ref> tmpl;
	
	virtual string get_full_name() {
		string str = package + "." + name;
		if(tmpl.size()) {
			str += "<";
			for(int i = 0; i < tmpl.size(); ++i) {
				
				if(i < tmpl.size()-1) {
					str += ",";
				}
			}
			str += ">";
		}
		return str;
	}
	
	virtual uint64_t get_hash() {
		string str = full_name();
		if(!isgeneric && bound.size()) {
			std::ostringstream ss;
			ss << "<";
			int i = 0;
			for(auto& param : bound) {
				ss << NAMES[param.hash];
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


class Typedef : public Type {
public:
	Ref type;
	string name;
};


class Enum : public Type {
public:
	vector<string> values;
	
};


class Struct : public Type {
public:
	
};


class Class : public Type {
public:
	Ref super;
	
	vector<Field*> constants;
	vector<Field*> fields;
	
};


class Interface : public Class {
public:
	vector<Method*> methods;
	
	
};

















#endif /* INCLUDE_AST_H_ */
