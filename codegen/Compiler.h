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
		check();
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
	
	virtual void check() {
		// TODO
	}
	
	virtual void postprocess() {
		
	}
	
protected:
	int curr_pass = 1;
	
};


}}

#endif /* INCLUDE_VNI_COMPILER_H_ */
