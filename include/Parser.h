/*
 * Parser.h
 *
 *  Created on: May 15, 2016
 *      Author: mad
 */

#ifndef INCLUDE_PARSER_H_
#define INCLUDE_PARSER_H_

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <map>
#include <vector>
using namespace std;

#include "CRC64.h"



vector<string> split(const std::string& s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

uint64_t hash64(const std::string& str) {
	CRC64 hash;
	hash.update(str.c_str(), str.size());
	uint64_t val = hash.getValue();
	names[str] = val;
	return val;
}


class Base {
public:
	Base() {}
	
	virtual ~Base() {}
	
	string package;
	string name;
	
	Base* super = 0;
	uint64_t super_hash = 0;
	
	map<string, uint64_t> imports;
	
	bool isgeneric = false;
	vector<string> generics;
	vector<pair<uint64_t, Base*> > bound;
	
	string full_name() {
		return package + "." + name;
	}
	
	uint64_t resolve(string ident) {
		if(ident == name) {
			return get_hash();
		}
		if(imports.count(ident)) {
			return imports[ident];
		}
		string full_ident = package + "." + ident;
		uint64_t hash = hash64(full_ident);
		names[hash] = full_ident;
		return hash;
	}
	
	virtual uint64_t get_hash() {
		string str = full_name();
		uint64_t val = hash64(str);
		names[val] = str;
		return val;
	}
	
};


class Primitive : public Base {
public:
	bool isintegral = true;
};

class Bool : public Primitive {
public:
	static const uint64_t HASH = hash64("bool");
	virtual uint64_t get_hash() override { return HASH; }
};
class Char : public Primitive {
public:
	static const uint64_t HASH = hash64("char");
	virtual uint64_t get_hash() override { return HASH; }
};
class Short : public Primitive {
public:
	static const uint64_t HASH = hash64("short");
	virtual uint64_t get_hash() override { return HASH; }
};

class String : public Primitive {
public:
	String() { isintegral = false; }
	static const uint64_t HASH = hash64("String");
	virtual uint64_t get_hash() override { return HASH; }
};

class Array : public Primitive {
public:
	Array() { isintegral = false; }
	
	Base* type = 0;
	uint64_t type_hash = 0;
	int size = 0;
	
	virtual uint64_t get_hash() override {
		std::ostringstream ss;
		ss << names[type_hash] << "[" << size << "]";
		string str = ss.str();
		uint64_t val = hash64(str);
		names[val] = str;
		return val;
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
	
	string value;
	
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
	map<uint64_t, Field*> fields;
	map<uint64_t, Field*> constants;
	map<uint64_t, Method*> methods;
	
};


class Interface : public Type {
public:
	
};


struct init_type_system {
	init_type_system() {
		index[Bool::HASH] = new Bool();
		index[Char::HASH] = new Char();
		index[Short::HASH] = new Short();
	}
};

init_type_system init_type_system_;


static map<uint64_t, string> names;
static map<uint64_t, Base*> index;
static map<uint64_t, Type*> types;
static map<uint64_t, Interface*> ifaces;


class Parser {
public:
	Parser(string filename) : file(filename), stream(filename, std::ifstream::in) {
		if(stream.fail()) {
			cout << "error: unable to open " << filename << endl;
		}
	}
	
	~Parser() {
		stream.close();
	}
	
	bool parse() {
		/*
		 * PACKAGE
		 */
		string package;
		if(next() == "package") {
			package = next();
			if(next() != ";") {
				return error("expected ;");
			}
		} else {
			return error("expected package definition");
		}
		
		/*
		 * IMPORT
		 */
		string tok;
		while(stream.good()) {
			tok = next();
			if(tok == "import") {
				tok = next();
				if(next() != ";") {
					return error("expected ;");
				}
				vector<string> toks = split(tok, '.');
				uint64_t hash = hash64(tok);
				imports[toks.back()] = hash;
				names[hash] = tok;
			} else {
				break;
			}
		}
		
		/*
		 * CLASS | INTERFACE
		 */
		if(tok == "class") {
			this_type = new Type();
			this_base = this_type;
		} else if(tok == "interface") {
			this_iface = new Interface();
			this_type = this_iface;
			this_base = this_iface;
		} else {
			return error("expected class or interface");
		}
		
		this_base->package = package;
		this_base->name = next();
		this_base->imports = imports;
		
		tok = next();
		vector<string> tmpl = parse_generics(tok);
		if(tmpl.size()) {
			this_base->isgeneric = true;
			this_base->generics = tmpl;
			tok = next();
		}
		if(tok == ":") {
			tok = next();
			this_base->super_hash = this_base->resolve(tok);
			tok = next();
		}
		if(tok != "{") {
			return error("expected { for class definition");
		}
		
		/*
		 * METHOD | FIELD | ENUM | CONSTANT
		 */
		while(stream.good()) {
			tok = next();
			if(tok == "enum") {
				/*
				 * ENUM
				 */
			} else if(tok == "static") {
				/*
				 * CONSTANT
				 */
				if(next() != "const") {
					return error("expected const after static");
				}
				
			} else if(tok != "}") {
				/*
				 * METHOD | FIELD
				 */
				string type_name = tok;
				tok = next();
				vector<string> tmpl = parse_generics(tok);
				vector<pair<uint64_t, Base*> > bound;
				if(tmpl.size()) {
					for(string str : tmpl) {
						bound.push_back(std::make_pair(this_base->resolve(str), (Base*)0));
					}
					tok = next();
				}
				string name = tok;
				tok = next();
				if(tok == "(") {
					/*
					 * METHOD
					 */
					Method* func = new Method;
					CRC64 unique_hash;
					unique_hash.update(name);
					tok = next();
					while(tok != ")" && stream.good()) {
						Param* param = new Param();
						param->type_hash = this_base->resolve(next());
						tok = next();
						param->generics = parse_generics(tok);
						if(param->generics.size()) {
							for(string str : param->generics) {
								param->bound.push_back(std::make_pair(this_base->resolve(str), (Base*)0));
							}
							tok = next();
						}
						param->name = tok;
						unique_hash.update(param->type_hash);
						for(auto entry : param->bound) {
							unique_hash.update(entry.first);
						}
						func->params.push_back(param);
						tok = next();
					}
					func->type_hash = this_base->resolve(type_name);
					func->package = this_base->full_name();
					func->name = name;
					func->generics = tmpl;
					func->bound = bound;
					
					if(!this_type->methods.count(unique_hash.getValue())) {
						this_type->methods[unique_hash] = func;
					} else {
						return error("redefinition of " + name);
					}
				} else {
					/*
					 * FIELD
					 */
					Field* field = new Field();
					field->package = this_base->full_name();
					field->name = name;
					field->generics = tmpl;
					field->bound = bound;
					tok = next();
					if(tok == "[") {
						/*
						 * ARRAY
						 */
						tok = next();
						int size = atol(tok.c_str());
						if(size <= 0) {
							return error("array must be larger than zero");
						}
						Array* array = new Array();
						array->type_hash = this_base->resolve(type_name);
						array->size = size;
						field->type = array;
						field->type_hash = array->get_hash();
					} else if(tok == ";") {
						field->type_hash = this_base->resolve(type_name);
					} else {
						return error("expected [ or ; after field name");
					}
					
					uint64_t unique_hash = hash64(name);
					if(!this_type->fields.count(unique_hash)) {
						this_type->fields[unique_hash] = field;
					} else {
						return error("redefinition of " + name);
					}
				}
			} else {
				break;
			}
		}
		
		/*
		 * FINISHED
		 */
		if(stream.good()) {
			uint64_t hash = this_base->get_hash();
			if(!index.count(hash)) {
				index[hash] = this_base;
				if(this_type) {
					types[hash] = this_type;
				} else if(this_iface) {
					ifaces[hash] = this_iface;
				}
			} else {
				return error("redefinition of " + this_base->name);
			}
			return true;
		} else {
			return error("unexpected end of file");
		}
	}
	
	vector<string> parse_generics(string tok) {
		vector<string> res;
		if(tok == "<") {
			while(stream.good()) {
				res.push_back(next());
				tok = next();
				if(tok == ">") {
					break;
				} else if(tok != ",") {
					return error("expected , or > after opening <");
				}
			}
		}
		return res;
	}
	
	string next() {
		string tok;
		bool skip = false;
		while(true) {
			char c = stream.peek();
			if(stream.fail()) { break; }
			if(c == '\n') {
				curr_line++;
			}
			if(c == ' ' || c == '\t' || c == '\n') {
				stream.get();
				if(tok.size()) {
					break;
				}
			} else if(	   c == ',' || c == ':' || c == ';' || c == '(' || c == ')' || c == '[' || c == ']' || c == '<' || c == '>'
						|| c == '!' || c == '@' || c == '#' || c == '$' || c == '^' || c == '&' || c == '+' || c == '-'
						|| c == '=' || c == '?' || c == '~' || c == '\\'	)
			{
				if(tok.size()) {
					break;
				} else {
					stream.get();
					tok += c;
					break;
				}
			} else if(c == '/') {
				if(skip) {
					do {
						c = stream.get();
					} while(c != '\n' && stream.good());
					skip = false;
					curr_line++;
				} else {
					stream.get();
					skip = true;
				}
			} else if(c == '*') {
				if(skip) {
					do {
						c = stream.get();
						if(c == '\n') { curr_line++; }
						skip = (c == '*');
					} while(!(c == '/' && skip) && stream.good());
					skip = false;
				} else {
					if(!tok.size()) {
						stream.get();
						tok += c;
					}
					break;
				}
			} else {
				if(skip) {
					skip = false;
				}
				stream.get();
				tok += c;
			}
		}
		return tok;
	}
	
	std::ostream& error() {
		return cout << file << ":" << curr_line << ": error: ";
	}
	
	bool error(string msg) {
		error() << msg << endl;
		return false;
	}
	
	bool fail() {
		return stream.fail();
	}
	
	int curr_line = 0;
	map<string, uint64_t> imports;
	Base* this_base = 0;
	Type* this_type = 0;
	Interface* this_iface = 0;
	
private:
	string file;
	std::ifstream stream;
	
};





#endif /* INCLUDE_PARSER_H_ */
