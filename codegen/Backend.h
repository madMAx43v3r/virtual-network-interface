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
	
	set<string> whitelist;
	
	virtual ~Backend() {
		delete_old(root);
	}
	
	virtual void generate_all() {
		vector<Type*> types;
		for(auto entry : INDEX) {
			Type* p_type = dynamic_cast<Type*>(entry.second);
			if(p_type && p_type->generate) {
				types.push_back(p_type);
			}
		}
		sort(types.begin(), types.end(), [](Type* a, Type* b) -> bool{
			return a->get_full_name() < b->get_full_name();
		});
		for(Type* type : types) {
			generate(type);
		}
	}
	
	virtual void generate(Type* type) = 0;
	
	virtual void update(string path, string name, string content) {
		path = root + path;
		
		string mkdir = "mkdir -p " + path;
		if(system(mkdir.c_str())) {
			cout << "ERROR: " << mkdir << " failed!" << endl;
		}
		
		string file_name = path + name;
		files_written.insert(file_name);
		
		string source = augment_source(content);
		bool same = false;
		
		std::ifstream reader(file_name, ios::in);
		if(reader.good()) {
			std::stringstream buffer;
			buffer << reader.rdbuf();
			if(buffer.str() == source) {
				same = true;
			}
		}
		reader.close();
		if(!same) {
			cout << "UPDATE " << file_name << endl;
			ofstream output(file_name, ios::trunc);
			output << source;
			output.close();
		}
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
					if(!files_written.count(file) && !whitelist.count(name)) {
						cout << "DELETE " << file << endl;
						remove(file.c_str());
					} else {
						count++;
					}
				} else if(ent->d_type == DT_DIR) {
					if(name != "." && name != ".." && !whitelist.count(name)) {
						int res = delete_old(file + "/");
						if(!res) {
							cout << "DELETE " << file << endl;
							string rm_cmd = "rm -r " + file;
							if(system(rm_cmd.c_str())) {
								cout << "ERROR: " << rm_cmd << " failed!" << endl;
							}
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
