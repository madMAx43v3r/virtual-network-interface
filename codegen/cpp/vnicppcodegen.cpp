/*
 * vnicppcodegen.cpp
 *
 *  Created on: Jul 8, 2016
 *      Author: mad
 */

#include "VNIParser.h"
#include "Compiler.h"

#include "cpp/Generator.h"

using namespace vni::codegen;


int main(int argc, char** argv) {
	
	if(argc < 2) {
		cout << "Usage: vnicppcodegen interface-dir ..." << endl;
		exit(-1);
	}
	
	Compiler<VNIParser> compiler;
	for(int i = 1; i < argc; ++i) {
		compiler.root_dirs.push_back(argv[i]);
	}
	
	compiler.compile();
	
	cpp::Generator gen;
	gen.generate_all();
	
}
