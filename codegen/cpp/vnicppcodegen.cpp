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
	
	init_type_system();
	
	Compiler<VNIParser> compiler;
	cpp::Generator generator;
	
	for(int i = 1; i < argc; ++i) {
		if(argv[i][0] == '-') {
			if(argv[i][1] == 'c') {
				generator.cmake_only = true;
				cout << "Option: cmake_only = true" << endl;
			}
			if(argv[i][1] == 'i') {
				generator.headers_only = true;
				cout << "Option: headers_only = true" << endl;
			}
			if(argv[i][1] == 'w') {
				generator.overwrite = true;
				cout << "Option: overwrite = true" << endl;
			}
			if(argv[i][1] == 'o') {
				generator.root = argv[i+1];
				cout << "Option: root = " << generator.root << endl;
				i++;
			}
			continue;
		}
		compiler.root_dirs.push_back(argv[i]);
	}
	
	compiler.compile();
	
	generator.generate_all();
	
}
