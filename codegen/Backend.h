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
	virtual ~Backend() {}
	
	virtual void generate() {
		
	}
	
	virtual void update(string file_name, string content) {
		string text;
		int stack = 0;
		bool newline = false;
		for(char c : content) {
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
		
		// TODO: create dirs
		// TODO: check for existing
		ofstream output(file_name, ios::trunc);
		output << text << endl;
		output.close();
	}
	
};





}}

#endif /* CODEGEN_BACKEND_H_ */
