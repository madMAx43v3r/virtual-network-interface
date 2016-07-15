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
	}
	
	Compiler<VNIParser> compiler;
	compiler.root_dirs.push_back(argv[1]);
	
	compiler.compile();
	
	cpp::Generator gen;
	gen.generate_all();
	
}
