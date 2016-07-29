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
		
		curr_pass = 2;
		for(string root : root_dirs) {
			parse_dir(root);
		}
		
		for(auto entry : INDEX) {
			if(entry.second) {
				entry.second->pre_compile();
			}
		}
		for(auto entry : INDEX) {
			if(entry.second) {
				entry.second->compile();
			}
		}
	}
	
protected:
	void parse_dir(string path) {
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
	
protected:
	int curr_pass = 1;
	
};








}}

#endif /* INCLUDE_VNI_COMPILER_H_ */
