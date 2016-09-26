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
	Object* p_object = dynamic_cast<Object*>(source);


namespace vni {
namespace codegen {
namespace cpp {

class Generator : public Backend {
public:
	bool make_shared = false;
	
	Generator() {
		whitelist.insert("CMakeFiles");
		whitelist.insert("libvni.a");
		whitelist.insert("libvni_shared.so");
	}
	
	virtual void generate_all() {
		Backend::generate_all();
		out.str("");
		out << "cmake_minimum_required(VERSION 2.4)" << endl << endl;
		out << "include_directories(include/)" << endl << endl;
		if(make_shared) {
			out << "ADD_LIBRARY(vni_shared SHARED" << endl;
		} else {
			out << "ADD_LIBRARY(vni STATIC" << endl;
		}
		for(string file : source_files) {
			out << "\t" << file << endl;
		}
		out << ")" << endl << endl;
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
		
		// generate client files
		if(p_iface && p_iface->get_full_name() != "vnl.Client") {
			out.str("");
			generate_client(p_iface);
			string header_dir = "include/" + subs(type->package->name, ".", "/") + "/";
			string header_name = type->name + "Client.hxx";
			update(header_dir, header_name, out.str());
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
			out << "vnl::Address";
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
		if(!p_primitive || !add_const) {
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
	
	void echo_includes(const set<string>& imports) {
		for(string import : imports) {
			out << "#include <" << subs(import, ".", "/");
			if(import == "vnl.Interface" || import == "vnl.Enum" || dynamic_cast<Interface*>(resolve(import))) {
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
		
		set<string> imports = p_type->imports;
		string super;
		is_base_value = false;
		if(p_class && p_class->super) {
			super = p_class->super->get_full_name();
		} else if(p_object && p_object->super) {
			super = p_object->super->get_full_name();
		} else if(p_iface && p_iface->super) {
			super = p_iface->super->get_full_name();
		} else if(p_struct && p_struct->super) {
			super = p_struct->super->get_full_name();
		} else if(p_iface) {
			super = "vnl.Interface";
		} else if(p_enum) {
			super = "vnl.Enum";
		} else {
			super = "vnl.Type";
			is_base_value = true;
		}
		if(super != "vnl.Type") {
			imports.insert(super);
		}
		echo_includes(imports);
		out << "#include <";
		if(is_base_value || p_type->get_full_name() == "vnl.BinaryValue") {
			out << "vnl/Type.h";
		} else {
			out << "vnl/Type.hxx";
		}
		out << ">" << endl << endl;
		
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
			if(p_iface) {
				out << "static const uint32_t NUM_FIELDS = " << p_iface->all_fields.size() << ";" << endl;
			}
			out << endl;
			
			if(p_iface) {
				out << "typedef " << subs(super, ".", "::") << " Super;" << endl << endl;
			}
			
			vector<Field*> constants;
			if(p_struct) {
				constants = p_struct->constants;
			} else if(p_iface) {
				constants = p_iface->constants;
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
				out << endl << "operator uint32_t() const {@" << endl << "return value;" << endl << "$}" << endl << endl;
				out << p_enum->name << "& operator=(const uint32_t& val) {@" << endl;
				out << "value = val;" << endl << "return *this;" << endl << "$}";
			}
			out << endl;
			
			vector<Field*> fields;
			if(p_struct) {
				fields = p_struct->fields;
			} else if(p_iface) {
				fields = p_iface->fields;
			}
			for(Field* field : fields) {
				echo_type(field);
				out << " " << field->name << ";" << endl;
			}
			
			out << endl;
			if(p_object) {
				out << base_name << "(const vnl::String& domain_, const vnl::String& topic_)" << endl;
				if(p_object->get_full_name() != "vnl.Object") {
					out << "\t:\t" << subs(super, ".", "::") << "(domain_, topic_)" << endl << "{@" << endl;
				} else {
					out << "{@" << endl;
				}
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
			if(p_object) {
				for(Field* field : fields) {
					out << "vnl::read_config(domain_, topic_, \"" << field->name << "\", " << field->name << ");" << endl;
				}
			}
			out << "$}" << endl << endl;
		}
		
		if(p_struct) {
			out << "static " << type_name << "* create();" << endl;
			out << "virtual " << type_name << "* clone() const;" << endl;
			out << "virtual void destroy();" << endl << endl;
			out << "virtual void serialize(vnl::io::TypeOutput& _out) const;" << endl;
			out << "virtual void deserialize(vnl::io::TypeInput& _in, int _size);" << endl << endl;
		}
		if(p_enum || p_struct || p_iface) {
			out << "virtual uint32_t vni_hash() const { return VNI_HASH; }" << endl;
			out << "virtual const char* type_name() const { return \"" << p_type->get_full_name() << "\"; }" << endl << endl;
		}
		if(p_struct) {
			out << "virtual int type_size() const { return sizeof(" << type_name << "); }" << endl;
		}
		if(p_struct || p_iface) {
			out << "virtual int num_fields() const { return NUM_FIELDS; }" << endl;
			out << "virtual int field_index(vnl::Hash32 _hash) const;" << endl;
			out << "virtual const char* field_name(int _index) const;" << endl;
			out << "virtual void get_field(int _index, vnl::String& _str) const;" << endl;
			out << "virtual void set_field(int _index, const vnl::String& _str);" << endl << endl;
		}
		if(p_enum) {
			out << "virtual void to_string_ex(vnl::String& str) const;" << endl;
		}
		
		if(p_iface) {
			out << "$protected:@" << endl;
			for(Method* method : p_iface->methods) {
				if(method->is_handle) {
					out << "virtual void handle(";
					echo_method_params(method, true);
					out << ", const vnl::Packet& packet) { handle(" << method->params[0]->name << "); }" << endl;
					
					out << "virtual void handle(";
					echo_method_params(method, true);
					out << ") {}" << endl;
				} else {
					out << "virtual ";
					echo_type(method);
					out << " " << method->name << "(";
					echo_method_params(method, true);
					out << ")" << (method->is_const ? " const" : "") << " = 0;" << endl;
				}
			}
			out << endl;
			out << "virtual bool vni_call(vnl::io::TypeInput& _in, uint32_t _hash, int _num_args);" << endl;
			out << "virtual bool vni_const_call(vnl::io::TypeInput& _in, uint32_t _hash, int _num_args, vnl::io::TypeOutput& _out);" << endl;
			if(p_object) {
				out << "virtual bool handle_switch(vnl::Value* _sample, vnl::Packet* _packet);" << endl;
			}
			
			out << endl << "template<class W>" << endl << "void write_fields(W& _writer) const {@" << endl;
			for(Field* field : p_iface->all_fields) {
				out << "_writer.set_" << field->name << "(" << field->name << ");" << endl;
			}
			out << "$}" << endl;
		}
		
		out << endl << "$};" << endl;
		
		if(is_template) {
			out << endl;
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
			out << "#include <vnl/Type.hxx>" << endl;
			
			set<Class*> sub_classes;
			if(is_base_value && p_class) {
				sub_classes = p_class->sub_types;
				for(Class* sub : sub_classes) {
					if(sub) {
						out << "#include <" << subs(sub->get_full_name(), ".", "/") << ".hxx>" << endl;
						sub_classes.insert(sub);
					}
				}
			}
			
			namespace_begin();
			if(is_base_value) {
				out << "GlobalPool* global_pool = 0;" << endl << endl;
				out << "vnl::Value* create(vnl::Hash32 hash) {@" << endl;
				out << "switch(hash) {@" << endl;
				for(Class* sub : sub_classes) {
					out << "case " << hash32_of(sub) << ": return vnl::create<" << full(sub) << ">();" << endl;
				}
				out << "default: return 0;" << endl;
				out << "$}" << endl << "$}" << endl << endl;
				
				out << "vnl::Array<vnl::String> get_class_names() {@" << endl;
				out << "vnl::Array<vnl::String> res;" << endl;
				for(Class* sub : sub_classes) {
					out << "res.push_back(\"" << sub->get_full_name() << "\");" << endl;
				}
				out << "return res;" << endl << "$}" << endl << endl;
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
			out << header << "const uint32_t " << scope << "NUM_FIELDS;" << endl;
			vector<Field*> constants;
			if(p_struct) {
				constants = p_struct->constants;
			} else if(p_iface) {
				constants = p_iface->constants;
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
		} else if(p_iface) {
			all_fields = p_iface->all_fields;
		}
		if(p_struct) {
			out << header << "void " << scope << "serialize(vnl::io::TypeOutput& _out) const {@" << endl;
			if(p_class) {
				out << "_out.putEntry(VNL_IO_CLASS, NUM_FIELDS);" << endl;
				out << "_out.putHash(VNI_HASH);" << endl;
			} else {
				out << "_out.putEntry(VNL_IO_STRUCT, NUM_FIELDS);" << endl;
			}
			for(Field* field : all_fields) {
				out << "_out.putHash(" << hash32_of(field) << "); ";
				out << "vnl::write(_out, " << field->name << ");" << endl;
			}
			out << "$}" << endl << endl;
			
			out << header << "void " << scope << "deserialize(vnl::io::TypeInput& _in, int _size) {@" << endl;
			out << "for(int i = 0; i < _size && !_in.error(); ++i) {@" << endl;
			out << "uint32_t _hash = 0;" << endl << "_in.getHash(_hash);" << endl;
			out << "switch(_hash) {@" << endl;
			for(Field* field : all_fields) {
				out << "case " << hash32_of(field) << ": vnl::read(_in, " << field->name << "); break;" << endl;
			}
			out << "default: _in.skip();" << endl;
			out << "$}" << endl << "$}" << endl << "$}" << endl << endl;
		}
		
		if(p_struct || p_iface) {
			out << header << "int " << scope << "field_index(vnl::Hash32 _hash) const {@" << endl;
			out << "switch(_hash) {@" << endl;
			int index = 0;
			for(Field* field : all_fields) {
				out << "case " << hash32_of(field) << ": return " << index++ << ";" << endl;
			}
			out << "default: return -1;" << endl;
			out << "$}" << endl << "$}" << endl << endl;
			
			out << header << "const char* " << scope << "field_name(int _index) const {@" << endl;
			out << "switch(_index) {@" << endl;
			index = 0;
			for(Field* field : all_fields) {
				out << "case " << index++ << ": return \"" << field->name << "\";" << endl;
			}
			out << "default: return \"?\";" << endl;
			out << "$}" << endl << "$}" << endl << endl;
			
			out << header << "void " << scope << "get_field(int _index, vnl::String& _str) const {@" << endl;
			out << "switch(_index) {@" << endl;
			index = 0;
			for(Field* field : all_fields) {
				out << "case " << index++ << ": vnl::to_string(_str, " << field->name << "); break;" << endl;
			}
			out << "default: _str << \"{}\";" << endl;
			out << "$}" << endl << "$}" << endl << endl;
			
			out << header << "void " << scope << "set_field(int _index, const vnl::String& _str) {@" << endl;
			out << "switch(_index) {@" << endl;
			index = 0;
			for(Field* field : all_fields) {
				out << "case " << index++ << ": vnl::from_string(_str, " << field->name << "); break;" << endl;
			}
			out << "$}" << endl << "$}" << endl << endl;
		}
		
		if(p_iface) {
			out << header << "bool " << scope << "vni_call(vnl::io::TypeInput& _in, uint32_t _hash, int _num_args) {@" << endl;
			echo_call_switch(p_iface, false);
			out << "return Super::vni_call(_in, _hash, _num_args);" << endl << "$}" << endl << endl;
			out << header << "bool " << scope << "vni_const_call(vnl::io::TypeInput& _in, uint32_t _hash, int _num_args, vnl::io::TypeOutput& _out) {@" << endl;
			echo_call_switch(p_iface, true);
			out << "return Super::vni_const_call(_in, _hash, _num_args, _out);" << endl << "$}" << endl << endl;
		}
		
		if(p_object) {
			out << header << "bool " << scope << "handle_switch(vnl::Value* _sample, vnl::Packet* _packet) {@" << endl;
			echo_handle_switch(p_object);
			out << "return Super::handle_switch(_sample, _packet);" << endl << "$}" << endl << endl;
		}
		
		if(p_enum) {
			for(string& value : p_enum->values) {
				out << "const uint32_t " << scope << value << ";" << endl;
			}
			out << endl << header << "void " << scope << "to_string_ex(vnl::String& str) const {@" << endl;
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
	
	void generate_writer(Interface* p_iface, Interface* p_super) {
		out << "class Writer ";
		if(p_super) {
			out << ": public " << subs(p_super->get_full_name()+"Client::Writer", ".", "::") << " ";
		}
		out << "{" << endl << "public:@" << endl;
		out << "Writer(vnl::io::TypeOutput& _out, bool _top_level = true) " << endl << "\t:\t";
		if(p_super) {
			out << p_super->get_name() << "Client::Writer(_out, false), ";
		}
		out << "_out(_out), _top_level(_top_level)" << endl << "{@" << endl << "if(_top_level) {@" << endl;
		out << "_out.putEntry(VNL_IO_INTERFACE, VNL_IO_BEGIN);" << endl;
		out << "_out.putHash(" << hash32_of(p_iface) << ");" << endl << "$}" << endl << "$}" << endl;
		out << "~Writer() {@" << endl << "if(_top_level) {@" << endl;
		out << "_out.putEntry(VNL_IO_INTERFACE, VNL_IO_END);" << endl << "$}" << endl << "$}" << endl;
		
		for(Method* method : p_iface->methods) {
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
		for(Field* field : p_iface->fields) {
			out << "void set_" << field->name << "(";
			echo_ref_to(field);
			out << " _value) {@" << endl;
			out << "_out.putEntry(VNL_IO_CALL, 1);" << endl;
			out << "_out.putHash(" << hash32_of(field) << ");" << endl;
			out << "vnl::write(_out, _value);" << endl;
			out << "$}" << endl;
		}
		for(Field* field : p_iface->fields) {
			out << "void get_" << field->name << "() {@" << endl;
			out << "_out.putEntry(VNL_IO_CONST_CALL, 0);" << endl;
			out << "_out.putHash(" << hash32_of(field) << ");" << endl;
			out << "$}" << endl;
		}
		
		out << "$protected:@" << endl;
		out << "vnl::io::TypeOutput& _out;" << endl;
		out << "bool _top_level;" << endl;
		out << "$};" << endl;
	}
	
	void generate_client(Interface* p_iface) {
		string guard_sym = "INCLUDE_VNI_GENERATED_" + subs(p_iface->package->name, ".", "_") + "_" + p_iface->name + "_CLIENT_HXX_";
		out << endl << "#ifndef " << guard_sym << endl;
		out << "#define " << guard_sym << endl << endl;
		out << "// AUTO GENERATED by virtual-network-interface codegen" << endl << endl;
		
		Object* p_object = dynamic_cast<Object*>(p_iface);
		string client_name = p_iface->name + "Client";
		string super;
		Interface* p_super = 0;
		bool is_base_object = false;
		if(p_object && p_object->super) {
			super = p_object->super->get_full_name() + "Client";
			p_super = p_object->super;
		} else if(p_iface && p_iface->super) {
			super = p_iface->super->get_full_name() + "Client";
			p_super = p_iface->super;
		} else {
			super = "vnl.Client";
			is_base_object = true;
		}
		
		set<string> imports = p_iface->imports;
		imports.erase(super.substr(0, super.size()-6));
		echo_includes(imports);
		out << "#include <" << subs(super, ".", "/") << (is_base_object ? ".h>" : ".hxx>") << endl << endl;
		namespace_begin();
		
		
		out << "class " << client_name << " : public " << subs(super, ".", "::") << " {" << endl;
		out << "public:@" << endl;
		generate_writer(p_iface, p_super);
		out << endl << client_name << "() {}" << endl;
		out << endl << client_name << "(const " << client_name << "& other) {@" << endl;
		out << "set_address(other.get_address());" << endl << "$}" << endl;
		out << endl << client_name << "(const vnl::Address& addr) {@" << endl;
		out << "set_address(addr);" << endl << "$}" << endl;
		out << endl << client_name << "& operator=(const vnl::Address& addr) {@" << endl;
		out << "set_address(addr);" << endl << "return *this;" << endl << "$}" << endl << endl;
		for(Method* method : p_iface->methods) {
			if(method->is_handle) {
				continue;
			}
			echo_client_call(p_iface, method);
		}
		for(Field* field : p_iface->fields) {
			Method method;
			method.name = std::string("set_") + field->name;
			method.type = resolve("void");
			method.is_const = false;
			method.params.push_back(field);
			echo_client_call(p_iface, &method);
		}
		for(Field* field : p_iface->fields) {
			Method method;
			method.name = std::string("get_") + field->name;
			method.type = field;
			method.is_const = true;
			echo_client_call(p_iface, &method);
		}
		out << "$};" << endl << endl;
		
		namespace_end();
		out << endl << "#endif // " << guard_sym << endl;
	}
	
	void echo_client_call(Interface* p_iface, Method* method) {
		out << "int " << method->name << "(";
		echo_method_params(method, true);
		if(method->is_const) {
			if(method->params.size()) {
				out << ", ";
			}
			echo_type(method);
			out << "& _result";
		}
		out << ") {@" << endl;
		out << "_buf.wrap(_data);" << endl;
		out << "{@" << endl << "Writer _wr(_out);" << endl;
		out << "_wr." << method->name << "(";
		echo_method_args(method);
		out << ");" << endl << "$}" << endl;
		out << "vnl::Packet* _pkt = _call();" << endl;
		out << "if(_pkt) {@" << endl;
		if(method->is_const) {
			out << "vnl::read(_in, _result);" << endl;
		}
		out << "_pkt->ack();" << endl;
		out << "$}" << endl << "return _error;";
		out << endl << "$}" << endl << endl;
	}
	
	void echo_call_switch(Interface* p_iface, bool is_const) {
		out << "switch(_hash) {" << endl;
		if(is_const) {
			for(Field* field : p_iface->fields) {
				out << "case " << hash32_of(field) << ": @" << endl;
				out << "if(_num_args == 0) {@" << endl;
				out << "vnl::write(_out, " << field->name << ");\nreturn true;" << endl;
				out << "$}" << endl << "break;$" << endl;
			}
		} else {
			for(Field* field : p_iface->fields) {
				out << "case " << hash32_of(field) << ": @" << endl;
				out << "if(_num_args == 1) {@" << endl;
				out << "vnl::read(_in, " << field->name << ");\nreturn true;" << endl;
				out << "$}" << endl << "break;$" << endl;
			}
		}
		map<uint32_t, vector<Method*> > call_map;
		for(Method* method : p_iface->methods) {
			if(method->is_const == is_const && !method->is_handle) {
				call_map[method->get_hash()].push_back(method);
			}
		}
		for(auto& entry : call_map) {
			out << "case " << hash32_to_str(entry.first) << ": @" << endl;
			out << "switch(_num_args) {@" << endl;
			for(Method* method : entry.second) {
				out << "case " << method->params.size() << ": {@" << endl;
				for(Field* param : method->params) {
					echo_field(param, true, true);
					out << "vnl::read(_in, " << param->name << ");" << endl;
				}
				out << "if(!_in.error()) {@" << endl;
				if(is_const) {
					echo_type(method);
					out << " _res = ";
				}
				out << method->name << "(";
				echo_method_args(method);
				out << ");" << endl;
				if(is_const) {
					out << "vnl::write(_out, _res);" << endl;
				}
				out << "return true;" << endl << "$}" << endl;
				out << "$}" << endl << "break;" << endl;
			}
			out << "$}" << endl << "break;$" << endl;
		}
		out << "}" << endl;
	}
	
	void echo_handle_switch(Object* module) {
		out << "switch(_sample->vni_hash()) {" << endl;
		for(Class* p_class : module->handles) {
			out << "case " << hash32_of(p_class) << ": ";
			out << "handle(*((" << full(p_class) << "*)_sample), *_packet); return true;" << endl;
		}
		out << "}" << endl;
	}
	
	bool is_base_value = false;
	bool is_template = false;
	vector<string> name_space;
	string base_name;
	string type_name;
	
	ostringstream out;
	
};










}}}

#endif /* INCLUDE_CPP_GENERATOR_H_ */
