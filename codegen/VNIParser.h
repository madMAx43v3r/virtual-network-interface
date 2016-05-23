/*
 * VNIParser.h
 *
 *  Created on: May 15, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNIPARSER_H_
#define INCLUDE_VNIPARSER_H_

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <map>
#include <vector>
using namespace std;

#include "CRC64.h"
#include "AST.h"
#include "Parser.h"


class VNIParser : public Parser {
public:
	
	uint64_t resolve(string ident, bool relax = false) {
		if(this_base) {
			// check if own name
			if(this_base->name == ident) {
				return this_base->get_hash();
			}
			// check if template parameter
			int i = 0;
			for(string str : this_base->generics) {
				if(str == ident) {
					return TmplType::get_index_hash(i);
				}
				i++;
			}
		}
		// check if already defined
		uint64_t hash = hash64(ident);
		if(INDEX.count(hash)) {
			if(relax) {
				Primitive* prim = dynamic_cast<Primitive*>(INDEX[hash]);
				if(prim) {
					return prim->class_hash;
				}
			}
			return hash;
		}
		// check if qualified
		if(ident.find('.') != string::npos) {
			return hash;
		}
		// check if imported
		if(imports.count(ident)) {
			return imports[ident];
		}
		// last resort, it's defined in currrent package
		string full_ident = package + "." + ident;
		uint64_t hash = hash64(full_ident);
		NAMES[hash] = full_ident;
		return hash;
	}
	
	virtual bool parse() override {
		/*
		 * PACKAGE
		 */
		if(next() == "package") {
			package = next();
			if(next() != ";") {
				return error("expected ; after package");
			}
		} else {
			return error("expected package definition");
		}
		
		/*
		 * IMPORT
		 */
		while(stream.good()) {
			token = next();
			if(token == "import") {
				token = next();
				if(next() != ";") {
					return error("expected ; after import");
				}
				vector<string> toks = split(token, '.');
				uint64_t hash = hash64(token);
				imports[toks.back()] = hash;
				NAMES[hash] = token;
			} else {
				break;
			}
		}
		
	block_loop:
		this_base = 0;
		this_type = 0;
		this_iface = 0;
		/*
		 * CLASS | INTERFACE | TYPEDEF | EXTERN
		 */
		if(token == "class") {
			this_type = new Type();
			this_base = this_type;
		} else if(token == "interface") {
			this_iface = new Interface();
			this_type = this_iface;
			this_base = this_iface;
		} else if(token == "typedef") {
			Typedef* type = new Typedef();
			type->package = package;
			type->type_hash = resolve(next());
			type->generics = parse_generics();
			type->name = token;
			if(next() != ";") {
				return error("expected ; after typedef");
			}
			uint64_t hash = type->get_hash();
			if(!INDEX.count(hash)) {
				INDEX[hash] = type;
				TYPEDEFS[hash] = type;
			} else {
				return error("invalid re-definition of " + type->full_name());
			}
		} else if(token == "extern") {
			Base* type = new Base();
			type->isextern = true;
			type->package = package;
			type->name = next();
			if(next() != ";") {
				return error("expected ; after typedef");
			}
			uint64_t hash = type->get_hash();
			if(!INDEX.count(hash)) {
				INDEX[hash] = type;
			} else {
				return error("invalid re-definition of " + type->full_name());
			}
		} else {
			return error("expected typedef, extern, class or interface");
		}
		
		/*
		 * CLASS | INTERFACE
		 */
		this_base->package = package;
		this_base->name = next();
		
		vector<string> tmpl = parse_generics();
		if(tmpl.size()) {
			this_base->isgeneric = true;
			this_base->generics = tmpl;
		}
		if(token == "extends") {
			token = next();
			this_base->super_hash = resolve(token);
			this_base->super_generics = parse_generics();
			if(this_base->super_generics.size()) {
				for(string str : this_base->super_generics) {
					this_base->super_bound.push_back(std::make_pair(resolve(str), (Base*)0));
				}
			}
		}
		if(token != "{") {
			return error("expected { for class definition");
		}
		
		/*
		 * METHOD | FIELD | ENUM | CONSTANT
		 */
		while(stream.good()) {
			token = next();
			if(token == "enum") {
				/*
				 * ENUM
				 */
				// TODO
			} else if(token == "static") {
				/*
				 * CONSTANT
				 */
				if(next() != "const") {
					return error("expected const after static");
				}
				// TODO
			} else if(token != "}") {
				/*
				 * METHOD | FIELD
				 */
				string type_name = token;
				vector<string> tmpl = parse_generics();
				vector<pair<uint64_t, Base*> > bound;
				if(tmpl.size()) {
					for(string str : tmpl) {
						bound.push_back(std::make_pair(resolve(str), (Base*)0));
					}
				}
				string name = token;
				token = next();
				if(token == "(") {
					/*
					 * METHOD
					 */
					if(!this_iface) {
						return error("invalid method definition");
					}
					Method* func = new Method();
					token = next();
					while(token != ")" && stream.good()) {
						Param* param = new Param();
						param->type_hash = resolve(next(), true);
						param->generics = parse_generics();
						if(param->generics.size()) {
							for(string str : param->generics) {
								param->bound.push_back(std::make_pair(resolve(str), (Base*)0));
							}
						}
						param->name = token;
						func->params.push_back(param);
						token = next();
					}
					func->type_hash = resolve(type_name, true);
					func->package = this_base->full_name();
					func->name = name;
					func->generics = tmpl;
					func->bound = bound;
					
					if(!this_iface->add_method(func)) {
						return error("invalid re-definition of method " + name);
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
					if(token == "[") {
						/*
						 * ARRAY
						 */
						token = next();
						int size = atol(token.c_str());
						if(size <= 0) {
							return error("array must be larger than zero");
						}
						Array* array = new Array();
						array->type_hash = resolve(type_name);
						array->size = size;
						field->type = array;
						field->type_hash = array->get_hash();
					} else if(token == "=") {
						field->value = next();
						if(field->value == "null") {
							field->isnull = true;
						}
						if(next() == ";") {
							field->type_hash = resolve(type_name, true);
						} else {
							return error("expected ; after field");
						}
					} else if(token == ";") {
						field->type_hash = resolve(type_name, true);
					} else {
						return error("expected [ or ; after field name");
					}
					
					if(!this_type->add_field(field)) {
						return error("invalid re-definition of field " + name);
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
			if(!INDEX.count(hash)) {
				INDEX[hash] = this_base;
				if(this_type) {
					TYPES[hash] = this_type;
				} else if(this_iface) {
					INTERFACES[hash] = this_iface;
				}
			} else {
				return error("invalid re-definition of " + this_base->name);
			}
			token = next();
			if(stream.good()) {
				goto block_loop;
			}
		}
		return true;
	}
	
	vector<string> parse_generics() {
		vector<string> res;
		token = next();
		if(token == "<") {
			while(stream.good()) {
				res.push_back(next());
				token = next();
				if(token == ">") {
					break;
				} else if(token != ",") {
					return error("expected , or > after opening <");
				}
			}
			token = next();
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
	
	string token;
	int curr_line = 0;
	string package;
	map<string, uint64_t> imports;
	Base* this_base = 0;
	Type* this_type = 0;
	Interface* this_iface = 0;
	
	
};





#endif /* INCLUDE_VNIPARSER_H_ */
