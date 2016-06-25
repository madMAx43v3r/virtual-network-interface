/*
 * test_AST.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: mad
 */

#include "../AST.h"




int main() {
	
	FILE = "test_AST.cpp";
	PACKAGE = new Package("test");
	
	Class* Value = new Class();
	Value->package = "test";
	Value->name = "Value";
	Value->fields.push_back(new Field());
	Value->fields[0]->type = resolve("int");
	Value->fields[0]->name = "x";
	Value->fields.push_back(new Field());
	Value->fields[1]->type = resolve("int");
	Value->fields[1]->name = "y";
	Value->fields.push_back(new Field());
	Value->fields[2]->type = resolve("int");
	Value->fields[2]->name = "z";
	
	Struct* value_t = new Struct();
	value_t->package = "test";
	value_t->name = "value_t";
	value_t->fields.push_back(new Field());
	value_t->fields[0]->type = resolve("int");
	value_t->fields[0]->name = "x";
	value_t->fields.push_back(new Field());
	value_t->fields[1]->type = resolve("int");
	value_t->fields[1]->name = "y";
	value_t->fields.push_back(new Field());
	value_t->fields[2]->type = resolve("int");
	value_t->fields[2]->name = "z";
	
	
}


