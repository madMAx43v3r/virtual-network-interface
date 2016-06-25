/*
 * Parser.h
 *
 *  Created on: May 21, 2016
 *      Author: mad
 */

#ifndef INCLUDE_PARSER_H_
#define INCLUDE_PARSER_H_

#include "AST.h"


struct init_type_system {
	init_type_system() {
		INDEX[Bool::HASH] = new Bool();
		INDEX[Char::HASH] = new Char();
		INDEX[Short::HASH] = new Short();
		INDEX[Int::HASH] = new Int();
		INDEX[Long::HASH] = new Long();
		INDEX[Float::HASH] = new Float();
		INDEX[Double::HASH] = new Double();
		INDEX[Binary::HASH] = new Binary();
		INDEX[String::HASH] = new String();
		
		for(int i = 0; i < 10; ++i) {
			uint64_t hash = TmplType::get_index_hash(i);
			TmplType* type = new TmplType();
			type->index = i;
			INDEX[hash] = type;
		}
	}
};

init_type_system init_type_system_;


vector<string> split(const std::string& s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


bool link_type(uint64_t hash, std::vector<pair<uint64_t, Base*> >& bound) {
	if(!INDEX.count(hash)) {
		cout << "error: unable to resolve " << NAMES[hash] << std::endl;
		return false;
	}
	Base* base = INDEX[hash];
	// TODO
	return true;
}

bool link_all() {
	// TODO
	return true;
}


class Parser {
public:
	Parser(string filename) : file(filename), stream(filename, std::ifstream::in) {
		if(stream.fail()) {
			cout << "error: unable to open " << filename << endl;
		}
	}
	
	virtual ~Parser() {
		stream.close();
	}
	
	virtual bool parse() = 0;
	
	string file;
	std::ifstream stream;
	
	
};



#endif /* INCLUDE_PARSER_H_ */
