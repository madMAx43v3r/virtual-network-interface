/*
 * test_vni_parser.cpp
 *
 *  Created on: Jul 8, 2016
 *      Author: mad
 */

#include "../VNIParser.h"
#include "../Compiler.h"

#include "../cpp/Generator.h"

using namespace vni::codegen;


class TestCompiler : public Compiler<VNIParser> {
public:
	
	virtual void generate() {
		
	}
	
};


int main(int argc, char** argv) {
	
	TestCompiler compiler;
	compiler.root_dirs.push_back(argv[1]);
	
	compiler.compile();
	
	cpp::Generator gen;
	gen.generate_header(resolve<Type>("vni.Value"));
	cout << endl << gen.out.str();
	
	gen.update("test.h", gen.out.str());
	
}
