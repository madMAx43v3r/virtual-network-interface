/*
 * Parser.h
 *
 *  Created on: May 21, 2016
 *      Author: mad
 */

#ifndef INCLUDE_PARSER_H_
#define INCLUDE_PARSER_H_

#include "AST.h"


inline vector<string> split(const std::string& s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

inline pair<string, string> split_full_name(const std::string& ident) {
	int pos = ident.find_last_of('.');
	if(pos != string::npos) {
		string package = ident.substr(0, pos);
		string name = ident.substr(pos+1);
		return make_pair(package, name);
	} else {
		return make_pair("", ident);
	}
}


class Parser {
public:
	Parser(string filename) : file_name(filename), stream(filename, std::ifstream::in) {
		PACKAGE = 0;
		CURR_FILE = file_name;
		CURR_LINE = 0;
		IMPORT.clear();
		if(stream.fail()) {
			cout << "error: unable to open " << filename << endl;
		}
	}
	
	virtual ~Parser() {
		CURR_FILE = "<internal>";
		CURR_LINE = -1;
		stream.close();
	}
	
	virtual void first_pass() = 0;
	virtual void second_pass() = 0;
	
	string file_name;
	std::ifstream stream;
	
	
};



#endif /* INCLUDE_PARSER_H_ */
