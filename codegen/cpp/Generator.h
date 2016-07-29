/*
 * Generator.h
 *
 *  Created on: Jul 8, 2016
 *      Author: mad
 */

#ifndef INCLUDE_CPP_GENERATOR_H_
#define INCLUDE_CPP_GENERATOR_H_

#include "../Backend.h"


#define TYPE_TREE(source) \
	Void* p_void = dynamic_cast<Void*>(source); \
	Primitive* p_primitive = dynamic_cast<Primitive*>(source); \
	Enum* p_enum = dynamic_cast<Enum*>(source); \
	Struct* p_struct = dynamic_cast<Struct*>(source); \
	Class* p_class = dynamic_cast<Class*>(source); \
	Interface* p_iface = dynamic_cast<Interface*>(source); \
	Object* p_object = dynamic_cast<Object*>(source); \
	Module* p_module = dynamic_cast<Module*>(source);


namespace vni {
namespace codegen {
namespace cpp {

class Generator : public Backend {
public:
	Generator() {
		whitelist.insert("CMakeFiles");
		whitelist.insert("libinterface.a");
	}
	
	virtual void generate_all() {
		Backend::generate_all();
		out.str("");
		out << "cmake_minimum_required(VERSION 2.4)" << endl << endl;
		out << "include_directories(include/)" << endl << endl;
		out << "ADD_LIBRARY(interface STATIC" << endl;
		for(string file : source_files) {
			out << "\t" << file << endl;
		}
		out << ")" << endl;
		update("", "CMakeLists.txt", out.str());
	}
	
	vector<string> source_files;
	
	virtual void generate(Type* type) {
		Interface* p_iface = dynamic_cast<Interface*>(type);
		Object* p_object = dynamic_cast<Object*>(type);
		is_template = p_iface && p_iface->generic.size();
		
		// generate header files
		out.str("");
		generate_header(type);
		string header_dir = "include/" + subs(type->package->name, ".", "/") + "/";
		string header_name = type->name + (p_iface ? "Support" : "") + ".hxx";
		update(header_dir, header_name, out.str());
		
		// generate source files
		if(!is_template) {
			out.str("");
			generate_source(type);
			string source_dir = "src/" + subs(type->package->name, ".", "/") + "/";
			string source_name = type->name + (p_iface ? "Support" : "") + ".cxx";
			source_files.push_back(source_dir + source_name);
			update(source_dir, source_name, out.str());
		}
	}
	
	static string full(string name) {
		return subs(name, ".", "::");
	}
	
	static string full(Type* type) {
		return subs(type->get_full_name(), ".", "::");
	}
	
	static string hash32_of(Base* type) {
		return hash32_to_str(type->get_hash());
	}
	
	static string hash32_of(string str) {
		return hash32_to_str(hash64(str));
	}
	
	static string hash32_to_str(uint32_t hash) {
		char buf[64];
		int len = snprintf(buf, 64, "0x%x", hash);
		return string(buf, len);
	}
	
	void namespace_begin() {
		out << endl;
		for(string space : name_space) {
			out << "namespace " << space << " {" << endl;
		}
		out << endl;
	}
	
	void namespace_end() {
		out << endl;
		for(string space : name_space) {
			out << "} // namespace" << endl;
		}
	}
	
	void echo_type(Base* type, bool constant = false) {
		TYPE_TREE(type);
		Bool* p_bool = dynamic_cast<Bool*>(type);
		Integer* p_int = dynamic_cast<Integer*>(type);
		Real* p_real = dynamic_cast<Real*>(type);
		Vector* p_vector = dynamic_cast<Vector*>(type);
		Binary* p_binary = dynamic_cast<Binary*>(type);
		String* p_string = dynamic_cast<String*>(type);
		Array* p_array = dynamic_cast<Array*>(type);
		List* p_list = dynamic_cast<List*>(type);
		TypeName* p_typename = dynamic_cast<TypeName*>(type);
		if(p_typename) {
			echo_type(p_typename->type, constant);
			if(p_typename->tmpl.size()) {
				out << "<";
				for(int i = 0; i < p_typename->tmpl.size(); ++i) {
					echo_type(p_typename->tmpl[i]);
					if(i < p_typename->tmpl.size()-1) {
						out << ", ";
					}
				}
				out << " >";
			}
		} else if(p_object) {
			out << full(p_object) << "Client";
		} else if(p_iface) {
			out << full(p_iface);
		} else if(p_struct) {
			out << full(p_struct);
		} else if(p_enum) {
			out << full(p_enum);
		} else if(p_vector) {
			out << "vnl::Vector<";
			echo_type(p_vector->type);
			out << ", " << p_vector->size << ">";
		} else if(p_bool) {
			out << "bool";
		} else if(p_int) {
			switch(p_int->size) {
				case 1: out << "int8_t"; break;
				case 2: out << "int16_t"; break;
				case 4: out << "int32_t"; break;
				case 8: out << "int64_t"; break;
				default: out << "?"; break;
			}
		} else if(p_real) {
			switch(p_real->size) {
				case 4: out << "float"; break;
				case 8: out << "double"; break;
				default: out << "?"; break;
			}
		} else if(p_binary) {
			out << "vnl::Binary";
		} else if(p_string) {
			if(constant) {
				out << "char*";
			} else {
				out << "vnl::String";
			}
		} else if(p_array) {
			out << "vnl::Array";
		} else if(p_list) {
			out << "vnl::List";
		} else if(p_void) {
			out << "void";
		}
	}
	
	void echo_ref_to(Field* field, bool add_const = true) {
		TYPE_TREE(field->type);
		if(!p_primitive && add_const) {
			out << "const ";
		}
		echo_type(field);
		if(!p_primitive) {
			out << "&";
		}
	}
	
	void echo_field(Field* field, bool def, bool init) {
		TYPE_TREE(field->type);
		if(def) {
			echo_type(field);
			out << " ";
		}
		out << field->name;
		if(init) {
			if(!field->value.empty()) {
				out << " = " << field->value;
			} else if(p_primitive) {
				out << " = 0";
			}
		}
		out << ";" << endl;
	}
	
	void echo_method_params(Method* method, bool add_const = true) {
		for(int i = 0; i < method->params.size(); ++i) {
			Field* param = method->params[i];
			echo_ref_to(param, add_const);
			out << " " << param->name;
			if(i < method->params.size()-1) {
				out << ", ";
			}
		}
	}
	
	void echo_method_args(Method* method) {
		int i = 0;
		for(Field* param : method->params) {
			out << param->name;
			if(i++ < method->params.size()-1) {
				out << ", ";
			}
		}
	}
	
	void echo_includes(Type* p_type) {
		for(string import : p_type->imports) {
			if(import == "vnl.Type") {
				if(is_base_value) {
					out << "#include <vnl/Type.h>" << endl;
				} else {
					out << "#include <vnl/Type.hxx>" << endl;
				}
				if(is_base_iface) {
					out << "#include <vnl/Interface.h>" << endl;
				}
				if(is_base_enum) {
					out << "#include <vnl/Enum.h>" << endl;
				}
				continue;
			}
			out << "#include <" << subs(import, ".", "/");
			if(dynamic_cast<Interface*>(resolve(import))) {
				out << ".h>" << endl;
			} else {
				out << ".hxx>" << endl;
			}
		}
		out << endl;
	}
	
	void generate_header(Type* p_type) {
		TYPE_TREE(p_type);
		type_name = p_type->name;
		base_name = p_type->name;
		if(p_iface) {
			base_name += "Base";
		}
		
		string guard_sym = "INCLUDE_VNI_GENERATED_" + subs(p_type->package->name, ".", "_") + "_" + base_name + "_HXX_";
		out << endl << "#ifndef " << guard_sym << endl;
		out << "#define " << guard_sym << endl << endl;
		out << "// AUTO GENERATED by virtual-network-interface codegen" << endl << endl;
		
		string super;
		is_base_enum = false;
		is_base_value = false;
		is_base_iface = false;
		if(p_class && p_class->super) {
			super = p_class->super->get_full_name();
		} else if(p_iface && p_iface->super) {
			super = p_iface->super->get_full_name();
		} else if(p_object && p_object->super) {
			super = p_object->super->get_full_name();
		} else if(p_module && p_type->name != "Module") {
			super = "vnl.Module";
		} else if(p_object && p_type->name != "Object") {
			super = "vnl.Object";
		} else if(p_iface && p_type->name != "Interface") {
			super = "vnl.Interface";
			is_base_iface = true;
		} else if(p_struct && p_type->name != "Value") {
			super = "vnl.Value";
		} else if(p_enum) {
			super = "vnl.Enum";
			is_base_enum = true;
		} else {
			super = "vnl.Type";
			is_base_value = true;
		}
		if(super.size() && super != "vnl.Interface" && super != "vnl.Enum") {
			p_type->imports.insert(super);
		}
		if(p_object && p_object->name == "Object") {
			p_type->imports.insert("vnl.Client");
		}
		p_type->imports.insert("vnl.Type");
		echo_includes(p_type);
		
		name_space = split(p_type->package->name, '.');
		namespace_begin();
		
		if(p_iface) {
			string template_str;
			if(p_iface->generic.size()) {
				template_str = "template<";
				for(int i = 0; i < p_iface->generic.size(); ++i) {
					template_str += "class " + p_iface->generic[i];
					if(i < p_iface->generic.size()-1) {
						template_str += ", ";
					}
				}
				template_str += ">";
			}
			out << template_str << endl;
		}
		if(p_enum || p_struct || p_iface) {
			out << "class " << base_name << " : public " << subs(super, ".", "::") << " {" << endl;
			out << "public:@" << endl;
			out << "static const uint32_t VNI_HASH = " << hash32_of(p_type) << ";" << endl;
			if(p_struct) {
				out << "static const uint32_t NUM_FIELDS = " << p_struct->all_fields.size() << ";" << endl;
			}
			if(p_object) {
				out << "static const uint32_t NUM_FIELDS = " << p_object->all_fields.size() << ";" << endl;
			}
			out << endl;
			
			if(p_iface) {
				out << "typedef " << subs(super, ".", "::") << " Super;" << endl << endl;
			}
			
			vector<Field*> constants;
			if(p_struct) {
				constants = p_struct->constants;
			} else if(p_object) {
				constants = p_object->constants;
			}
			for(Field* field : constants) {
				out << "static const ";
				echo_type(field, true);
				out << " " << field->name;
				if(dynamic_cast<Primitive*>(field->type)) {
					out << " = " << field->value;
				}
				out << ";" << endl;
			}
			if(p_enum) {
				for(string& value : p_enum->values) {
					out << "static const uint32_t " << value << " = " << hash32_of(value) << ";" << endl;
				}
			}
			out << endl;
			
			vector<Field*> fields;
			if(p_struct) {
				fields = p_struct->fields;
			} else if(p_object) {
				fields = p_object->fields;
			}
			for(Field* field : fields) {
				echo_type(field);
				out << " " << field->name << ";" << endl;
			}
			
			out << endl;
			if(p_object && p_object->name != "Object") {
				out << base_name << "(const vnl::String& domain_, const vnl::String& topic_)" << endl;
				out << "\t:\t" << subs(super, ".", "::") << "(domain_, topic_)" << endl << "{@" << endl;
			} else {
				out << base_name << "() {@" << endl;
			}
			for(Field* field : fields) {
				if(!field->value.empty()) {
					out << field->name << " = " << field->value << ";" << endl;
				} else if(dynamic_cast<Primitive*>(field->type)) {
					out << field->name << " = 0;" << endl;
				}
			}
			out << "$}" << endl << endl;
		}
		
		if(p_struct) {
			out << "static " << type_name << "* create();" << endl;
			out << "virtual " << type_name << "* clone() const;" << endl;
			out << "virtual void destroy();" << endl << endl;
			out << "virtual void serialize(vnl::io::TypeOutput& out_) const;" << endl;
			out << "virtual void deserialize(vnl::io::TypeInput& in_, int size_);" << endl << endl;
		}
		if(p_module) {
			out << "virtual void serialize(vnl::io::TypeOutput& out_) const;" << endl << endl;
		}
		if(p_enum || p_struct || p_iface) {
			out << "virtual uint32_t vni_hash() const { return VNI_HASH; }" << endl;
			out << "virtual const char* type_name() const { return \"" << p_type->get_full_name() << "\"; }" << endl << endl;
		}
		if(p_struct) {
			out << "virtual int type_size() const { return sizeof(" << type_name << "); }" << endl;
		}
		if(p_struct || p_object) {
			out << "virtual int num_fields() const { return NUM_FIELDS; }" << endl;
			out << "virtual int field_index(vnl::Hash32 hash_) const;" << endl;
			out << "virtual const char* field_name(int index_) const;" << endl;
			out << "virtual void get_field(int index_, vnl::String& str_) const;" << endl;
			out << "virtual void set_field(int index_, vnl::io::ByteInput& in_);" << endl << endl;
		}
		if(p_enum) {
			out << "virtual void to_string_ex(vnl::String& str) const;" << endl;
		}
		
		if(p_iface) {
			out << "$protected:@" << endl;
			for(Method* method : p_iface->methods) {
				out << "virtual ";
				echo_type(method);
				out << " " << method->name << "(";
				echo_method_params(method, true);
				if(method->is_handle) {
					out << ", const vnl::Packet& packet";
				}
				out << ")" << (method->is_const ? " const" : "");
				out << " = 0;" << endl;
			}
			out << endl;
			out << "virtual bool vni_call(vnl::io::TypeInput& in_, uint32_t hash_, int num_args_);" << endl;
			out << "virtual bool vni_const_call(vnl::io::TypeInput& in_, uint32_t hash_, int num_args_, vnl::io::TypeOutput& out_);" << endl;
			if(p_module) {
				out << "virtual bool handle_switch(vnl::Value* sample_, vnl::Packet* packet_);" << endl;
			}
			out << endl << "$public:@" << endl;
			generate_writer(p_iface);
		}
		
		out << endl << "$};" << endl;
		
		if(p_object) {
			generate_client(p_object);
		}
		
		if(is_template) {
			generate_source(p_type);
		}
		
		namespace_end();
		out << endl << "#endif // " << guard_sym << endl;
	}
	
	void generate_source(Type* p_type) {
		TYPE_TREE(p_type);
		if(!is_template) {
			out << endl << "// AUTO GENERATED by virtual-network-interface codegen" << endl;
			out << endl << "#include <" << subs(p_type->get_full_name(), ".", "/") << (p_iface ? "Support" : "") << ".hxx" << ">" << endl;
			out << "#include <vnl/Pool.h>" << endl;
			
			set<Struct*> sub_classes;
			if(is_base_value) {
				for(auto entry : INDEX) {
					Struct* p_struct = dynamic_cast<Struct*>(entry.second);
					if(p_struct && !dynamic_cast<Interface*>(entry.second)) {
						out << "#include <" << subs(p_struct->get_full_name(), ".", "/") << ".hxx>" << endl;
						sub_classes.insert(p_struct);
					}
				}
			}
			
			namespace_begin();
			if(is_base_value) {
				out << "GlobalPool* global_pool = 0;" << endl << endl;
				out << "vnl::Value* create(vnl::Hash32 hash) {@" << endl;
				out << "switch(hash) {@" << endl;
				for(Struct* sub : sub_classes) {
					out << "case " << hash32_of(sub) << ": return vnl::create<" << full(sub) << ">();" << endl;
				}
				out << "default: return 0;" << endl;
				out << "$}" << endl << "$}" << endl << endl;
			}
		}
		
		string scope = base_name;
		string header = "";
		if(is_template) {
			scope += "<";
			header += "template<";
			int i = 0;
			for(string param : p_iface->generic) {
				scope += param;
				header += "class " + param;
				if(i++ < p_iface->generic.size()-1) {
					scope += ", ";
					header += ", ";
				}
			}
			scope += ">";
			header += ">\n";
		}
		scope += "::";
		
		if(p_struct || p_iface) {
			out << header << "const uint32_t " << scope << "VNI_HASH;" << endl;
		}
		if(p_struct || p_object) {
			out << header << "const uint32_t " << scope << "NUM_FIELDS;" << endl;
			vector<Field*> constants;
			if(p_struct) {
				constants = p_struct->constants;
			} else if(p_object) {
				constants = p_object->constants;
			}
			for(Field* field : constants) {
				out << header << "const ";
				echo_type(field, true);
				out << " " << scope << field->name;
				if(dynamic_cast<Primitive*>(field->type) == 0) {
					out << " = " << field->value;
				}
				out << ";" << endl;
			}
		}
		out << endl;
		
		if(p_struct) {
			out << type_name << "* " << scope << "create() {@" << endl;
			out << "return vnl::create<" << type_name << ">();" << endl << "$}" << endl << endl;
			out << type_name << "* " << scope << "clone() const {@" << endl;
			out << "return vnl::clone<" << type_name << ">(*this);" << endl << "$}" << endl << endl;
			out << "void " << scope << "destroy() {@" << endl;
			out << "this->" << type_name << "::~" << type_name << "();" << endl;
			out << "return vnl::global_pool->push_back(this, sizeof(" << type_name << "));" << endl << "$}" << endl << endl;
		}
		
		vector<Field*> all_fields;
		if(p_struct) {
			all_fields = p_struct->all_fields;
		} else if(p_object) {
			all_fields = p_object->all_fields;
		}
		if(p_struct) {
			out << header << "void " << scope << "serialize(vnl::io::TypeOutput& out_) const {@" << endl;
			if(p_class) {
				out << "out_.putEntry(VNL_IO_CLASS, NUM_FIELDS);" << endl;
				out << "out_.putHash(VNI_HASH);" << endl;
			} else {
				out << "out_.putEntry(VNL_IO_STRUCT, NUM_FIELDS);" << endl;
			}
			for(Field* field : all_fields) {
				out << "out_.putHash(" << hash32_of(field) << "); ";
				out << "vnl::write(out_, " << field->name << ");" << endl;
			}
			out << "$}" << endl << endl;
			
			out << header << "void " << scope << "deserialize(vnl::io::TypeInput& in_, int size_) {@" << endl;
			out << "for(int i = 0; i < size_ && !in_.error(); ++i) {@" << endl;
			out << "uint32_t hash_ = 0;" << endl << "in_.getHash(hash_);" << endl;
			out << "switch(hash_) {@" << endl;
			for(Field* field : all_fields) {
				out << "case " << hash32_of(field) << ": vnl::read(in_, " << field->name << "); break;" << endl;
			}
			out << "default: in_.skip();" << endl;
			out << "$}" << endl << "$}" << endl << "$}" << endl << endl;
		}
		if(p_module) {
			out << header << "void " << scope << "serialize(vnl::io::TypeOutput& out_) const {@" << endl;
			out << "Writer wr_(out_, this);" << endl;
			out << "$}" << endl << endl;
		}
		
		if(p_struct || p_object) {
			out << header << "int " << scope << "field_index(vnl::Hash32 hash_) const {@" << endl;
			out << "switch(hash_) {@" << endl;
			int index = 0;
			for(Field* field : all_fields) {
				out << "case " << hash32_of(field) << ": return " << index++ << ";" << endl;
			}
			out << "default: return -1;" << endl;
			out << "$}" << endl << "$}" << endl << endl;
			
			out << header << "const char* " << scope << "field_name(int index_) const {@" << endl;
			out << "switch(index_) {@" << endl;
			index = 0;
			for(Field* field : all_fields) {
				out << "case " << index++ << ": return \"" << field->name << "\";" << endl;
			}
			out << "default: return \"?\";" << endl;
			out << "$}" << endl << "$}" << endl << endl;
			
			out << header << "void " << scope << "get_field(int index_, vnl::String& str_) const {@" << endl;
			out << "switch(index_) {@" << endl << endl;
			index = 0;
			for(Field* field : all_fields) {
				out << "case " << index++ << ": vnl::to_string(str_, " << field->name << "); break;" << endl;
			}
			out << "default: str_ << \"{}\";" << endl;
			out << "$}" << endl << "$}" << endl << endl;
			
			out << header << "void " << scope << "set_field(int index_, vnl::io::ByteInput& in_) {@" << endl;
			out << "switch(index_) {@" << endl;
			index = 0;
			for(Field* field : all_fields) {
				out << "case " << index++ << ": vnl::from_string(in_, " << field->name << "); break;" << endl;
			}
			out << "default: break;" << endl;
			out << "$}" << endl << "$}" << endl << endl;
		}
		
		if(p_iface) {
			out << header << "bool " << scope << "vni_call(vnl::io::TypeInput& in_, uint32_t hash_, int num_args_) {@" << endl;
			echo_call_switch(p_iface, false);
			out << "return false;" << endl << "$}" << endl << endl;
			out << header << "bool " << scope << "vni_const_call(vnl::io::TypeInput& in_, uint32_t hash_, int num_args_, vnl::io::TypeOutput& out_) {@" << endl;
			echo_call_switch(p_iface, true);
			out << "return false;" << endl << "$}" << endl << endl;
		}
		
		if(p_module) {
			out << header << "bool " << scope << "handle_switch(vnl::Value* sample_, vnl::Packet* packet_) {@" << endl;
			echo_handle_switch(p_module);
			out << "return false;" << endl << "$}" << endl << endl;
		}
		
		if(p_enum) {
			out << header << "void " << scope << "to_string_ex(vnl::String& str) const {@" << endl;
			out << "switch(value) {@" << endl;
			for(string& field : p_enum->values) {
				out << "case " << hash32_of(field) << ": str << \"\\\"" << field << "\\\"\"; break;" << endl;
			}
			out << "default: str << \"\\\"?\\\"\";" << endl;
			out << "$}" << endl << "$}" << endl << endl;
		}
		
		if(!is_template) {
			namespace_end();
		}
	}
	
	void generate_writer(Interface* p_iface) {
		Object* p_object = dynamic_cast<Object*>(p_iface);
		out << "class Writer {" << endl << "public:@" << endl;
		out << "Writer(vnl::io::TypeOutput& out_) : _out(out_) {@" << endl;
		out << "_out.putEntry(VNL_IO_INTERFACE, VNL_IO_BEGIN);" << endl;
		out << "_out.putHash(VNI_HASH);" << endl << "$}" << endl;
		if(p_object) {
			out << "Writer(vnl::io::TypeOutput& out_, const " << base_name << "* obj_) : _out(out_) {@" << endl;
			out << "_out.putEntry(VNL_IO_INTERFACE, VNL_IO_BEGIN);" << endl;
			out << "_out.putHash(VNI_HASH);" << endl;
			for(Field* field : p_object->all_fields) {
				out << "set_" << field->name << "(obj_->" << field->name << ");" << endl;
			}
			out << "$}" << endl;
		}
		out << "~Writer() {@" << endl << "_out.putEntry(VNL_IO_INTERFACE, VNL_IO_END);" << endl << "$}" << endl;
		
		for(Method* method : p_iface->all_methods) {
			if(method->name == "handle") {
				continue;
			}
			out << "void " << method->name << "(";
			echo_method_params(method);
			out << ") {@" << endl;
			out << "_out.putEntry(" << (method->is_const ? "VNL_IO_CONST_CALL" : "VNL_IO_CALL") << ", " << method->params.size() << ");" << endl;
			out << "_out.putHash(" << hash32_of(method) << ");" << endl;
			for(Field* param : method->params) {
				out << "vnl::write(_out, " << param->name << ");" << endl;
			}
			out << "$}" << endl;
		}
		if(p_object) {
			for(Field* field : p_object->all_fields) {
				out << "void set_" << field->name << "(";
				echo_ref_to(field);
				out << " value_) {@" << endl;
				out << "_out.putEntry(VNL_IO_CALL, 1);" << endl;
				out << "_out.putHash(" << hash32_of(field) << ");" << endl;
				out << "vnl::write(_out, value_);" << endl;
				out << "$}" << endl;
			}
		}
		
		out << "$protected:@" << endl;
		out << "vnl::io::TypeOutput& _out;" << endl;
		out << "$};" << endl;
	}
	
	void generate_client(Object* p_object) {
		string client_name = p_object->name + "Client";
		string super;
		if(p_object->super) {
			super = p_object->super->get_full_name() + "Client";
		} else if(p_object->name != "Object") {
			super = "vnl.ObjectClient";
		} else {
			super = "vnl.Client";
		}
		out << endl << endl;
		out << "class " << client_name << " : public " << subs(super, ".", "::") << " {" << endl;
		out << "public:@" << endl;
		for(Method* method : p_object->methods) {
			if(method->is_handle) {
				continue;
			}
			echo_client_call(p_object, method);
		}
		for(Field* field : p_object->fields) {
			Method method;
			method.name = "set_" + field->name;
			method.type = resolve("int");
			method.is_const = false;
			method.params.push_back(field);
			echo_client_call(p_object, &method);
		}
		out << "$};" << endl << endl;
	}
	
	void echo_client_call(Object* p_object, Method* method) {
		out << "int " << method->name << "(";
		echo_method_params(method, true);
		if(method->is_const) {
			if(method->params.size()) {
				out << ", ";
			}
			echo_type(method);
			out << "& result_";
		}
		out << ") {@" << endl;
		out << "_buf.wrap(_data);" << endl;
		out << "{ " << full(p_object) << "Base::Writer _wr(_out); ";
		out << "_wr." << method->name << "(";
		echo_method_args(method);
		out << "); }" << endl;
		out << "vnl::Packet* _pkt = _call();" << endl;
		out << "if(_pkt) {@" << endl;
		if(method->is_const) {
			out << "vnl::read(_in, result_);" << endl;
		}
		out << "_pkt->ack();" << endl;
		out << "$}" << endl << "return _error;";
		out << endl << "$}" << endl;
	}
	
	void echo_call_switch(Interface* p_iface, bool is_const) {
		Object* p_object = dynamic_cast<Object*>(p_iface);
		out << "switch(hash_) {" << endl;
		if(p_object && !is_const) {
			for(Field* field : p_object->all_fields) {
				out << "case " << hash32_of(field) << ": @" << endl;
				out << "if(num_args_ == 1) {@" << endl;
				out << "vnl::read(in_, " << field->name << ");\nreturn true;" << endl;
				out << "$}" << endl << "break;$" << endl;
			}
		}
		map<uint32_t, vector<Method*> > call_map;
		for(Method* method : p_iface->all_methods) {
			if(method->is_const == is_const && !method->is_handle) {
				call_map[method->get_hash()].push_back(method);
			}
		}
		for(auto& entry : call_map) {
			out << "case " << hash32_to_str(entry.first) << ": @" << endl;
			out << "switch(num_args_) {@" << endl;
			for(Method* method : entry.second) {
				out << "case " << method->params.size() << ": {@" << endl;
				for(Field* param : method->params) {
					echo_field(param, true, true);
					out << "vnl::read(in_, " << param->name << ");" << endl;
				}
				out << "if(!in_.error()) {@" << endl;
				if(is_const) {
					echo_type(method);
					out << " res_ = ";
				}
				out << method->name << "(";
				echo_method_args(method);
				out << ");" << endl;
				if(is_const) {
					out << "vnl::write(out_, res_);" << endl;
				}
				out << "return true;" << endl << "$}" << endl;
				out << "$}" << endl << "break;" << endl;
			}
			out << "$}" << endl << "break;$" << endl;
		}
		out << "}" << endl;
	}
	
	void echo_handle_switch(Module* module) {
		out << "switch(sample_->vni_hash()) {" << endl;
		for(Method* method : module->all_methods) {
			if(method->is_handle) {
				Class* p_class = dynamic_cast<Class*>(method->params[0]->type);
				if(p_class) {
					string name = full(p_class);
					out << "case " << name << "::VNI_HASH: ";
					out << "handle(*((" << name << "*)sample_), *packet_); return true;" << endl;
				}
			}
		}
		out << "}" << endl;
	}
	
	bool is_base_enum = false;
	bool is_base_value = false;
	bool is_base_iface = false;
	bool is_template = false;
	vector<string> name_space;
	string base_name;
	string type_name;
	
	ostringstream out;
	
};










}}}

#endif /* INCLUDE_CPP_GENERATOR_H_ */
