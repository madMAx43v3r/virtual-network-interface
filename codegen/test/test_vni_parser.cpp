/*
 * test_vni_parser.cpp
 *
 *  Created on: Jul 8, 2016
 *      Author: mad
 */

#include "../VNIParser.h"
#include "../Compiler.h"


class TestCompiler : public Compiler<VNIParser> {
public:
	
	virtual void generate() {
		
	}
	
};


int main(int argc, char** argv) {
	
	TestCompiler compiler;
	compiler.root_dirs.push_back(argv[1]);
	
	compiler.compile();
	
}
