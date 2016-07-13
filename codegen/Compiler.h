/*
 * Compiler.h
 *
 *  Created on: Jul 8, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_COMPILER_H_
#define INCLUDE_VNI_COMPILER_H_

#include <dirent.h>
#include "VNIParser.h"


namespace vni {
namespace codegen {

template<typename TParser>
class Compiler {
public:
	vector<string> root_dirs;
	
	virtual ~Compiler() {}
	
	virtual void register_default() {
		INDEX["Value"] = resolve("vni.Value");
		INDEX["Bool"] = resolve("vni.Bool");
		INDEX["Integer"] = resolve("vni.Integer");
		INDEX["Real"] = resolve("vni.Real");
		INDEX["Binary"] = resolve("vni.Binary");
		INDEX["String"] = resolve("vni.String");
		INDEX["Array"] = resolve("vni.Array");
		INDEX["List"] = resolve("vni.List");
	}
	
	virtual void compile() {
		curr_pass = 1;
		for(string root : root_dirs) {
			parse_dir(root);
		}
		cout << endl << "INDEX = {" << endl;
		for(auto entry : INDEX) {
			cout << "  " << entry.first << " = " << entry.second << endl;
		}
		cout << "}" << endl << endl;
		register_default();
		curr_pass = 2;
		for(string root : root_dirs) {
			parse_dir(root);
		}
		postprocess();
	}
	
	virtual void parse_dir(string path) {
		cout << "DIR " << path << endl;
		DIR* dir;
		if((dir = opendir(path.c_str())) != 0) {
			dirent* ent;
			while((ent = readdir(dir)) != 0) {
				string name = ent->d_name;
				string file_name = path + (path.back() != '/' ? "/" : "") + name;
				if(ent->d_type == DT_REG) {
					cout << "FILE " << file_name << endl;
					if(name.size() > 4 && name.substr(name.size()-4) == ".vni") {
						TParser parser(file_name);
						if(curr_pass == 1) {
							parser.first_pass();
						}
						if(curr_pass == 2) {
							parser.second_pass();
						}
					}
				} else if(ent->d_type == DT_DIR) {
					if(name != "." && name != "..") {
						parse_dir(file_name);
					}
				}
			}
			closedir (dir);
		} else {
			cerr << "ERROR: Unable to open " << path << endl;
			FAIL();
		}
		
	}
	
	virtual void postprocess() {
		for(auto entry : INDEX) {
			Struct* p_struct = dynamic_cast<Struct*>(entry.second);
			Class* p_class = dynamic_cast<Class*>(entry.second);
			if(p_struct) {
				p_struct->all_fields = p_struct->fields;
			}
			if(p_class) {
				Class* super = p_class->super;
				while(super) {
					p_class->all_fields.insert(p_class->all_fields.end(), super->fields.begin(), super->fields.end());
					super = super->super;
				}
			}
			if(p_struct) {
				check_dup_fields(p_struct->fields);
			}
			
			Interface* p_iface = dynamic_cast<Interface*>(entry.second);
			if(p_iface) {
				p_iface->all_methods = p_iface->methods;
				Interface* super = p_iface->super;
				while(super) {
					p_iface->all_methods.insert(p_iface->all_methods.end(), super->methods.begin(), super->methods.end());
					super = super->super;
				}
				p_iface->all_methods = get_unique_methods(p_iface->all_methods);
			}
			
			Object* p_object = dynamic_cast<Object*>(entry.second);
			if(p_object) {
				Object* super = p_object->super;
				while(super) {
					p_object->all_fields.insert(p_object->all_fields.end(), super->fields.begin(), super->fields.end());
					p_object->all_methods.insert(p_object->all_methods.end(), super->methods.begin(), super->methods.end());
					for(Interface* iface : super->implements) {
						p_object->all_methods.insert(p_object->all_methods.end(), iface->methods.begin(), iface->methods.end());
					}
					super = super->super;
				}
				for(Interface* iface : p_object->implements) {
					p_object->all_methods.insert(p_object->all_methods.end(), iface->methods.begin(), iface->methods.end());
				}
				check_dup_fields(p_object->fields);
				p_iface->all_methods = get_unique_methods(p_iface->all_methods);
			}
		}
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
	
protected:
	int curr_pass = 1;
	
};








}}

#endif /* INCLUDE_VNI_COMPILER_H_ */
