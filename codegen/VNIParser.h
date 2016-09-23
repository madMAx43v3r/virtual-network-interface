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


namespace vni {
namespace codegen {

class VNIParser : public Parser {
public:
	VNIParser(string filename) : Parser(filename) {
		
	}
	
	virtual void first_pass() {
		parse_package();
		token = read_token();
		parse_imports();
		while(!end_of_file) {
			string keyword = token;
			string name = read_token();
			Base* base = 0;
			if(keyword == "enum") {
				base = new Enum(name);
				cout << "  ENUM " << name << endl;
			} else if(keyword == "struct") {
				base = new Struct(name);
				cout << "  STRUCT " << name << endl;
			} else if(keyword == "class") {
				base = new Class(name);
				cout << "  CLASS " << name << endl;
			} else if(keyword == "interface") {
				base = new Interface(name);
				cout << "  INTERFACE " << name << endl;
			} else if(keyword == "object") {
				base = new Object(name);
				cout << "  OBJECT " << name << endl;
			} else {
				ERROR("expected type definition");
			}
			skip_type();
			token = read_token();
		}
	}
	
	virtual void second_pass() {
		parse_package();
		read_token();
		parse_imports();
		while(!end_of_file) {
			string keyword = token;
			string name = read_token();
			string full_name = PACKAGE->name + "." + name;
			Base* p_base = 0;
			Enum* p_enum = 0;
			Struct* p_struct = 0;
			Class* p_class = 0;
			Interface* p_interface = 0;
			Object* p_object = 0;
			if(keyword == "enum") {
				p_enum = resolve<Enum>(full_name);
				cout << "  ENUM " << name << endl;
			} else if(keyword == "struct") {
				p_struct = resolve<Struct>(full_name);
				cout << "  STRUCT " << name << endl;
			} else if(keyword == "class") {
				p_class = resolve<Class>(full_name);
				cout << "  CLASS " << name << endl;
			} else if(keyword == "interface") {
				p_interface = resolve<Interface>(full_name);
				cout << "  INTERFACE " << name << endl;
			} else if(keyword == "object") {
				p_object = resolve<Object>(full_name);
				cout << "  OBJECT " << name << endl;
			}
			if(p_object) {
				p_interface = p_object;
			}
			if(p_interface) {
				p_base = p_interface;
			}
			if(p_class) {
				p_struct = p_class;
			}
			if(p_struct) {
				p_base = p_struct;
			}
			if(p_enum) {
				p_base = p_enum;
			}
			if(!p_base) {
				ERROR("expected type definition");
			}
			read_token();
			if(p_enum) {
				parse_enum(p_enum);
				read_token();
				continue;
			}
			if(p_interface) {
				p_interface->generic = parse_generics();
				for(string& gen : p_interface->generic) {
					cout << "    GENERIC " << gen << endl;
				}
			}
			if(token == "extends") {
				if(!p_class && !p_interface) {
					ERROR("only class and interface can extend");
				}
				string super = read_token();
				cout << "  EXTENDS " << super << endl;
				if(p_object) {
					p_object->super = resolve<Object>(super);
				} else if(p_interface) {
					p_interface->super = resolve<Interface>(super);
				} else if(p_class) {
					p_class->super = resolve<Class>(super);
				}
				read_token();
			}
			if(token == "implements") {
				if(!p_object) {
					ERROR("only object can implement");
				}
				read_token();
				while(!end_of_file) {
					p_object->implements.push_back(resolve<Object>(token));
					cout << "  IMPLEMENTS " << token << endl;
					read_token();
					if(token == "{") {
						break;
					} else if(token == ",") {
						read_token();
					} else {
						ERROR("expected , or {");
					}
				}
			}
			if(token == ";") {
				p_base->generate = false;
				read_token();
				continue;
			}
			if(token != "{") {
				ERROR("expeced {");
			}
			// type body loop
			bool end_of_type = false;
			while(!end_of_file) {
				read_token();
				if(token == "}") {
					read_token();
					end_of_type = true;
					break;
				}
				
				bool is_const = false;
				if(token == "const") {
					is_const = true;
					read_token();
				}
				
				string type = token;
				read_token();
				vector<Base*> tmpl_params = parse_tmpl_types();
				string name = token;
				read_token();
				Field* field = 0;
				Method* method = 0;
				if(token == ";") {
					field = new Field();
					field->type = resolve(type);
					if(p_struct) {
						p_struct->fields.push_back(field);
					} else if(p_interface) {
						p_interface->fields.push_back(field);
					} else {
						ERROR("cannot have fields here");
					}
					cout << "    FIELD " << type << " " << name << endl;
				} else if(token == "=") {
					string value = read_token();
					field = new Field();
					field->type = resolve(type);
					field->value = value;
					if(is_const) {
						if(p_struct) {
							p_struct->constants.push_back(field);
						} else if(p_interface) {
							p_interface->constants.push_back(field);
						} else {
							ERROR("cannot have fields here");
						}
						cout << "    CONST " << type << " " << name << " = " << value << endl;
					} else {
						if(p_struct) {
							p_struct->fields.push_back(field);
						} else if(p_interface) {
							p_interface->fields.push_back(field);
						} else {
							ERROR("cannot have fields here");
						}
						cout << "    FIELD " << type << " " << name << " = " << value << endl;
					}
					if(read_token() != ";") {
						ERROR("expected ; after field");
					}
				} else if(token == "[") {
					read_token();
					int size = atoi(token.c_str());
					if(size <= 0) {
						ERROR("invalid vector size");
					}
					field = new Field();
					Vector* array = new Vector();
					array->type = resolve(type);
					array->size = size;
					field->type = array;
					if(p_struct) {
						p_struct->fields.push_back(field);
					} else if(p_interface) {
						p_interface->fields.push_back(field);
					} else {
						ERROR("cannot have fields here");
					}
					if(read_token() != "]") {
						ERROR("expected ] after [");
					}
					if(read_token() != ";") {
						ERROR("expected ; after field");
					}
					cout << "    FIELD " << type << " " << name << " VECTOR " << size << endl;
				} else if(token == "(") {
					method = new Method();
					method->name = name;
					method->type = resolve(type);
					method->tmpl = tmpl_params;
					method->is_handle = method->name == "handle";
					read_token();
					cout << "    METHOD " << type << " " << name;
					while(token != ")" && !end_of_file) {
						string type_ = token;
						string name_ = read_token();
						Field* param = new Field();
						param->type = resolve(type_);
						param->tmpl = tmpl_params;
						param->name = name_;
						method->params.push_back(param);
						cout << " [" << type_ << " " << name_ << "]";
						read_token();
						if(token == ",") {
							read_token();
						} else if(token != ")") {
							ERROR("expected , or )");
						}
					}
					read_token();
					if(token == ";") {
						method->is_const = false;
					} else if(token == "const") {
						method->is_const = true;
						cout << " CONST";
						read_token();
						if(token != ";") {
							ERROR("expected ;");
						}
					} else {
						ERROR("expected ; or const");
					}
					cout << endl;
					if(p_interface) {
						p_interface->methods.push_back(method);
					} else {
						ERROR("cannot have methods here");
					}
				}
				
				if(field) {
					field->tmpl = tmpl_params;
					field->name = name;
					field->is_const = is_const;
				}
				
			} // type body loop
			
			if(!end_of_type) {
				ERROR("expected } before end of file");
			}
			
		} // file loop
		
	}
	
	void parse_package() {
		if(read_token() == "package") {
			string name = read_token();
			cout << "PACKAGE " << name << endl;
			PACKAGE = Package::get(name);
			if(read_token() != ";") {
				ERROR("expected ; after package");
			}
		} else {
			ERROR("expected package definition");
		}
	}
	
	void parse_imports() {
		while(!end_of_file) {
			if(token == "import") {
				string name = read_token();
				if(read_token() != ";") {
					ERROR("expected ; after import");
				}
				auto tmp = split_full_name(name);
				IMPORT[tmp.second] = name;
				cout << "IMPORT " << tmp.second << " FROM " << tmp.first << endl;
				read_token();
			} else {
				break;
			}
		}
	}
	
	void skip_type() {
		while(!end_of_file) {
			read_token();
			if(token == "{") {
				break;
			} else if(token == ";") {
				return;
			}
		}
		int stack = 1;
		while(!end_of_file) {
			read_token();
			if(token == "{") {
				stack++;
			} else if(token == "}") {
				stack--;
				if(stack == 0) {
					break;
				}
			}
		}
	}
	
	void parse_enum(Enum* type) {
		if(token != "{") {
			ERROR("expected {");
		}
		while(token != "}" && !end_of_file) {
			read_token();
			type->values.push_back(token);
			cout << "    " << token << endl;
			read_token();
			if(token != "," && token != "}") {
				ERROR("expected , or }");
			}
		}
	}
	
	vector<string> parse_generics() {
		vector<string> res;
		if(token == "<") {
			while(!end_of_file) {
				string param = read_token();
				res.push_back(param);
				read_token();
				if(token == ">") {
					break;
				} else if(token != ",") {
					ERROR("expected , or > after opening <");
				}
			}
			read_token();
		}
		return res;
	}
	
	vector<Base*> parse_tmpl_types() {
		vector<Base*> res;
		vector<string> strings = parse_generics();
		for(string& str : strings) {
			res.push_back(resolve(str));
		}
		return res;
	}
	
	void parse_comment() {
		string text;
		int c;
		do {
			c = stream.get();
			text += c;
		} while(c != '\n' && stream.good());
		CURR_LINE++;
		cout << "COMMENT //" << text;
	}
	
	void parse_comment_multi() {
		string text;
		int c;
		do {
			c = stream.get();
			if(c == '\n') {
				CURR_LINE++;
			}
			text += c;
		} while((text.size() < 2 || text.substr(text.size()-2) != "*/") && stream.good());
		cout << "COMMENT\n/*" << text << endl;
	}
	
	string parse_string() {
		string text;
		text += stream.get();
		bool escape = false;
		int c;
		do {
			c = stream.get();
			if(!escape && c == '\\') {
				text += c;
				escape = true;
				c = 0;
				continue;
			}
			text += c;
			if(escape) {
				escape = false;
				c = 0;
				continue;
			}
		} while(c != '"' && stream.good());
		return text;
	}
	
	string read_token() {
		token.clear();
		while(true) {
			int c = stream.peek();
			if(stream.fail() || c == EOF) {
				end_of_file = true;
				break;
			}
			if(c == '\n') {
				CURR_LINE++;
			}
			if(c == '"') {
				token = parse_string();
				break;
			} else if(token == "//") {
				parse_comment();
				token.clear();
			} else if(token == "/*") {
				parse_comment_multi();
				token.clear();
			} else if(c == ' ' || c == '\t' || c == '\n') {
				stream.get();
				if(token.size()) {
					break;
				}
			} else if(	c == ',' || c == ':' || c == ';' || c == '(' || c == ')'
					 || c == '[' || c == ']' || c == '<' || c == '>' || c == '!'
					 || c == '@' || c == '#' || c == '$' || c == '^' || c == '&'
					 || c == '=' || c == '?' || c == '~' || c == '%'
					 || c == '{' || c == '}' )
			{
				if(token.size()) {
					break;
				} else {
					stream.get();
					token += c;
					break;
				}
			} else {
				stream.get();
				token += c;
			}
		}
		//cout << "TOKEN " << token << endl;
		return token;
	}
	
	string token;
	bool end_of_file = false;
	
	
};



}}

#endif /* INCLUDE_VNIPARSER_H_ */
