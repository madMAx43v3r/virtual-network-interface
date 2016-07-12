/*
 * Generator.h
 *
 *  Created on: Jul 9, 2016
 *      Author: mad
 */

#ifndef CODEGEN_BACKEND_H_
#define CODEGEN_BACKEND_H_

#include "AST.h"
#include <algorithm>

namespace vni {
namespace codegen {

std::string subs(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}


class Backend {
public:
	string root = "generated/";
	
	virtual ~Backend() {
		delete_old(root);
	}
	
	virtual void generate_all() {
		for(auto entry : INDEX) {
			Type* p_type = dynamic_cast<Type*>(entry.second);
			if(p_type) {
				generate(p_type);
			}
		}
	}
	
	virtual void generate(Type* type) = 0;
	
	virtual void update(string path, string name, string content) {
		path = root + path;
		
		string mkdir = "mkdir -p " + path;
		system(mkdir.c_str());
		
		string file_name = path + name;
		files_written.insert(file_name);
		
		string source = augment_source(content);
		
		// TODO: check for equal
		
		cout << "UPDATE " << file_name << endl;
		ofstream output(file_name, ios::trunc);
		output << source << endl;
		output.close();
	}
	
	string augment_source(string input) {
		string text;
		int stack = 0;
		bool newline = false;
		for(char c : input) {
			if(c == '@') {
				stack++;
				continue;
			} else if(c == '$') {
				stack--;
				continue;
			}
			if(newline) {
				for(int i = 0; i < stack; ++i) {
					text += '\t';
				}
				newline = false;
			}
			if(c == '\n') {
				newline = true;
			}
			text += c;
		}
		return text;
	}
	
	virtual int delete_old(string path) {
		int count = 0;
		DIR* dir;
		if((dir = opendir(path.c_str())) != 0) {
			dirent* ent;
			while((ent = readdir(dir)) != 0) {
				string name = ent->d_name;
				string file = path + name;
				if(ent->d_type == DT_REG) {
					if(!files_written.count(file)) {
						cout << "DELETE " << file << endl;
						remove(file.c_str());
					} else {
						count++;
					}
				} else if(ent->d_type == DT_DIR) {
					if(name != "." && name != "..") {
						int res = delete_old(file + "/");
						if(!res) {
							cout << "DELETE " << file << endl;
							string rm_cmd = "rm -r " + file;
							system(rm_cmd.c_str());
						} else {
							count += res;
						}
					}
				}
			}
			closedir (dir);
		} else {
			cerr << "ERROR: Unable to open " << path << endl;
			FAIL();
		}
		return count;
	}
	
private:
	set<string> files_written;
	
};





}}

#endif /* CODEGEN_BACKEND_H_ */