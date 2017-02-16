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
	bool headers_only = false;
	bool cmake_only = false;
	
	Generator() {
		whitelist.insert("CMakeLists.txt");
	}
	
	virtual void generate_all() {
		Backend::generate_all();
		if(!headers_only) {
			out.str("");
			out << "set(VNI_INPUT_FILES@" << endl;
			for(string file : vni_files) {
				out << file << endl;
			}
			out << "$)" << endl << endl;
			out << "set(VNI_OUTPUT_FILES_HEADER@" << endl;
			for(string file : header_files) {
				out << root << file << endl;
			}
			out << "$)" << endl << endl;
			out << "set(VNI_OUTPUT_FILES_SOURCE@" << endl;
			for(string file : source_files) {
				out << root << file << endl;
			}
			out << "$)" << endl << endl;
			update("", "vni_input_output.cmake", out.str());
		}
	}
	
	set<string> vni_files;
	vector<string> header_files;
	vector<string> source_files;
	
	virtual void generate(Type* type) {
		Interface* p_iface = dynamic_cast<Interface*>(type);
		Object* p_object = dynamic_cast<Object*>(type);
		is_template = p_iface && p_iface->generic.size();
		vni_files.insert(type->file);
		
		// generate header files
		out.str("");
		generate_header(type);
		string header_dir = "include/" + subs(type->package->name, ".", "/") + "/";
		string header_name = type->name + (p_iface ? "Support" : "") + ".hxx";
		header_files.push_back(header_dir + header_name);
		if(!cmake_only) {
			update(header_dir, header_name, out.str());
		}
		
		// generate source files
		if(!is_template && !headers_only) {
			out.str("");
			generate_source(type);
			string source_dir = "src/" + subs(type->package->name, ".", "/") + "/";
			string source_name = type->name + (p_iface ? "Support" : "") + ".cxx";
			source_files.push_back(source_dir + source_name);
			if(!cmake_only) {
				update(source_dir, source_name, out.str());
			}
		}
		
		// generate client files
		if(p_iface) {
			out.str("");
			generate_client(p_iface);
			string header_dir = "include/" + subs(type->package->name, ".", "/") + "/";
			string header_name = type->name + "Client.hxx";
			header_files.push_back(header_dir + header_name);
			if(!cmake_only) {
				update(header_dir, header_name, out.str());
			}
		}
	}
	
	static string full(string name) {
		return subs(name, ".", "::");
	}
	
	static string full(Base* base, bool constant = false) {
		TYPE_TREE(base);
		Bool* p_bool = dynamic_cast<Bool*>(base);
		Integer* p_int = dynamic_cast<Integer*>(base);
		Real* p_real = dynamic_cast<Real*>(base);
		Vector* p_vector = dynamic_cast<Vector*>(base);
		Type* p_type = dynamic_cast<Type*>(base);
		TypeName* p_typename = dynamic_cast<TypeName*>(base);
		Generic* p_generic = dynamic_cast<Generic*>(base);
		string res = constant ? "const " : "";
		if(p_typename) {
			bool is_const = p_typename->is_const && !dynamic_cast<Method*>(base);
			res += subs(full(p_typename->type, is_const), ".", "::") + get_tmpl(p_typename);
		} else if(p_vector) {
			res += "vnl::Vector";
		} else if(p_bool) {
			res += "bool";
		} else if(p_int) {
			switch(p_int->size) {
				case 1: res += "int8_t"; break;
				case 2: res += "int16_t"; break;
				case 4: res += "int32_t"; break;
				case 8: res += "int64_t"; break;
				default: res += "?"; break;
			}
		} else if(p_real) {
			switch(p_real->size) {
				case 4: res += "float"; break;
				case 8: res += "double"; break;
				default: res += "?"; break;
			}
		} else if(p_void) {
			res += "void";
		} else if(p_generic) {
			res += p_generic->name;
		} else if(p_type) {
			if(p_type->get_full_name() == "vnl.String" && constant) {
				res += "char*";
			} else {
				res += subs(p_type->get_full_name(), ".", "::");
			}
		}
		return res;
	}
	
	static string get_full_name(Base* type) {
		if(dynamic_cast<Type*>(type)) {
			return ((Type*)type)->get_full_name();
		}
		return type->get_name();
	}
	
	static string get_tmpl(TypeName* name) {
		string str;
		if(name && name->tmpl.size()) {
			str += "<";
			for(int i = 0; i < name->tmpl.size(); ++i) {
				if(i > 0) {
					str += ", ";
				}
				str += full(name->tmpl[i]);
			}
			str += " >";
		}
		return str;
	}
	
	static string get_tmpl_header(Interface* p_iface) {
		string str;
		if(p_iface->generic.size()) {
			str = "template<";
			for(int i = 0; i < p_iface->generic.size(); ++i) {
				str += "class " + p_iface->generic[i]->name;
				if(i < p_iface->generic.size()-1) {
					str += ", ";
				}
			}
			str += ">";
		}
		return str;
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
	
	void echo_type(Base* type) {
		out << full(type);
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
	
	void echo_includes(const set<Type*>& imports) {
		vector<Type*> sorted(imports.begin(), imports.end());
		sort(sorted.begin(), sorted.end(), [](Type* a, Type* b) -> bool
		{
			return a->get_full_name() < b->get_full_name(); 
		});
		for(Type* import : sorted) {
			string full_name = import->get_full_name();
			out << "#include <" << subs(full_name, ".", "/");
			if(full_name == "vnl.Interface" || full_name == "vnl.Enum"
				|| dynamic_cast<Extern*>(import) || dynamic_cast<Interface*>(import))
			{
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
		
		p_super = 0;
		is_base_value = false;
		if(p_type && p_type->super) {
			p_super = p_type->super;
		} else {
			p_super = new TypeName(resolve("vnl.Type"));
			is_base_value = true;
		}
		echo_includes(p_type->imports);
		out << "#include <";
		if(is_base_value) {
			out << "vnl/Type.h";
		} else {
			out << "vnl/Type.hxx";
		}
		out << ">" << endl << endl;
		
		name_space = split(p_type->package->name, '.');
		namespace_begin();
		
		if(p_iface && !p_iface->generic.empty()) {
			out << get_tmpl_header(p_iface) << endl;
		}
		if(p_enum || p_struct || p_iface) {
			out << "class " << base_name << " : public " << full(p_super) << " {" << endl;
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
				out << "typedef " << full(p_super) << " Super;" << endl << endl;
			}
			
			vector<Field*> constants;
			if(p_struct) {
				constants = p_struct->constants;
			} else if(p_iface) {
				constants = p_iface->constants;
			}
			for(Field* field : constants) {
				out << "static ";
				echo_type(field);
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
				out << base_name << "(const vnl::String& domain_, const vnl::String& topic_);" << endl;
			} else {
				out << base_name << "();" << endl;
			}
			out << endl;
		}
		
		if(p_struct) {
			out << "static " << type_name << "* create();" << endl;
			out << "static " << type_name << "* create(vnl::Hash32 hash);" << endl;
			out << "virtual " << type_name << "* clone() const;" << endl;
			out << "virtual bool is_assignable(vnl::Hash32 hash);" << endl;
			out << "virtual bool assign(const vnl::Value& _value);" << endl;
			out << "virtual void raise() const { throw *this; }" << endl;
			out << "virtual void destroy();" << endl << endl;
			out << "virtual void serialize(vnl::io::TypeOutput& _out) const;" << endl;
			out << "virtual void deserialize(vnl::io::TypeInput& _in, int _size);" << endl << endl;
		}
		if(p_enum || p_struct || p_iface) {
			out << "virtual uint32_t get_vni_hash() const { return VNI_HASH; }" << endl;
			out << "virtual const char* get_type_name() const { return \"" << p_type->get_full_name() << "\"; }" << endl << endl;
		}
		if(p_struct) {
			out << "virtual int get_type_size() const { return sizeof(" << type_name << "); }" << endl;
		}
		if(p_struct || p_iface) {
			out << "virtual int get_num_fields() const { return NUM_FIELDS; }" << endl;
			out << "virtual int get_field_index(vnl::Hash32 _hash) const;" << endl;
			out << "virtual const char* get_field_name(int _index) const;" << endl;
			out << "virtual void get_field(int _index, vnl::String& _str) const;" << endl;
			out << "virtual void set_field(int _index, const vnl::String& _str);" << endl;
			out << "virtual void get_field(int _index, vnl::io::TypeOutput& _out) const;" << endl;
			out << "virtual void set_field(int _index, vnl::io::TypeInput& _in);" << endl;
			out << "virtual void get_field(int _index, vnl::Var& _var) const;" << endl;
			out << "virtual void set_field(int _index, const vnl::Var& _var);" << endl << endl;
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
					out << ", vnl::Basic* input) { handle(" << method->params[0]->name << "); }" << endl;
					
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
				out << "virtual bool handle_switch(vnl::Value* _sample, vnl::Basic* _input);" << endl;
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
		
		if(is_base_value && p_class) {
			vector<Type*> extern_types;
			for(auto& entry : INDEX) {
				Extern* type = dynamic_cast<Extern*>(entry.second);
				if(type) {
					extern_types.push_back(type);
				}
			}
			sort(extern_types.begin(), extern_types.end(), [](Type* a, Type* b) -> bool
			{
				return a->get_full_name() < b->get_full_name(); 
			});
			out << endl;
			for(Type* p_extern : extern_types) {
				vector<string> scope = split(p_extern->package->name, '.');
				for(string& s : scope) {
					out << "namespace " << s << " { ";
				}
				out << "class " << p_extern->name << ";";
				for(string& s : scope) {
					out << " }";
				}
				out << endl;
			}
			out << endl << "namespace vnl {" << endl << endl;
			for(Type* p_extern : extern_types) {
				out << "void read(vnl::io::TypeInput& in, " << full(p_extern) << "& obj);" << endl;
				out << "void write(vnl::io::TypeOutput& out, const " << full(p_extern) << "& obj);" << endl;
				out << "void from_string(const vnl::String& str, " << full(p_extern) << "& obj);" << endl;
				out << "void to_string(vnl::String& str, const " << full(p_extern) << "& obj);" << endl << endl;
			}
			out << "} // vnl" << endl;
		}
		
		out << endl << "#endif // " << guard_sym << endl;
	}
	
	void generate_source(Type* p_type) {
		TYPE_TREE(p_type);
		vector<Class*> sub_classes;
		if(p_class) {
			sub_classes.assign(p_class->sub_types.begin(), p_class->sub_types.end());
			sort(sub_classes.begin(), sub_classes.end(), [](Class* a, Class* b) -> bool
			{
				return a->get_full_name() < b->get_full_name(); 
			});
		}
		
		if(!is_template) {
			out << endl << "// AUTO GENERATED by virtual-network-interface codegen" << endl;
			out << endl << "#include <" << subs(p_type->get_full_name(), ".", "/") << (p_iface ? "Support" : "") << ".hxx" << ">" << endl;
			out << "#include <vnl/Type.hxx>" << endl;
			out << "#include <vnl/Var.h>" << endl;
			
			if(p_class) {
				for(Class* sub : sub_classes) {
					out << "#include <" << subs(sub->get_full_name(), ".", "/") << ".hxx>" << endl;
				}
			}
			
			namespace_begin();
		}
		if(p_class) {
			sub_classes.push_back(p_class);
		}
		
		string scope = base_name;
		string header = "";
		if(is_template) {
			scope += "<";
			header += "template<";
			int i = 0;
			for(Generic* param : p_iface->generic) {
				scope += param->name;
				header += "class " + param->name;
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
				out << header;
				echo_type(field);
				out << " " << scope << field->name;
				if(dynamic_cast<Primitive*>(field->type) == 0) {
					out << " = " << field->value;
				}
				out << ";" << endl;
			}
		}
		out << endl;
		
		vector<Field*> fields;
		if(p_struct) {
			fields = p_struct->fields;
		} else if(p_iface) {
			fields = p_iface->fields;
		}
		if(p_object) {
			out << header << scope << base_name << "(const vnl::String& domain_, const vnl::String& topic_)" << endl;
			if(p_object->get_full_name() != "vnl.Object") {
				out << "\t:\t" << full(p_super) << "::" << p_super->get_name() << "(domain_, topic_)" << endl << "{@" << endl;
			} else {
				out << "{@" << endl;
			}
		} else if(p_struct || p_iface || p_enum) {
			out << scope << base_name << "() {@" << endl;
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
		if(p_struct || p_iface || p_enum) {
			out << "$}" << endl << endl;
		}
		
		if(p_struct) {
			out << type_name << "* " << scope << "create() {@" << endl;
			out << "return vnl::create<" << type_name << ">();" << endl << "$}" << endl << endl;
			
			out << type_name << "* " << scope << "create(vnl::Hash32 hash) {@" << endl;
			out << "switch(hash) {@" << endl;
			for(Class* sub : sub_classes) {
				out << "case " << hash32_of(sub) << ": return vnl::create<" << full(sub) << ">();" << endl;
			}
			out << "default: return 0;" << endl;
			out << "$}" << endl << "$}" << endl << endl;
			
			out << type_name << "* " << scope << "clone() const {@" << endl;
			out << "return vnl::clone<" << type_name << ">(*this);" << endl << "$}" << endl << endl;
			
			out << "void " << scope << "destroy() {@" << endl;
			out << "this->" << type_name << "::~" << type_name << "();" << endl;
			out << "return vnl::internal::global_pool_->push_back(this, sizeof(" << type_name << "));" << endl << "$}" << endl << endl;
			
			out << "bool " << scope << "is_assignable(vnl::Hash32 hash) {@" << endl;
			out << "switch(hash) {@" << endl;
			for(Class* sub : sub_classes) {
				out << "case " << hash32_of(sub) << ": return true;" << endl;
			}
			out << "default: return false;" << endl;
			out << "$}" << endl << "$}" << endl << endl;
			
			out << "bool " << scope << "assign(const vnl::Value& _value) {@" << endl;
			out << "switch(_value.get_vni_hash()) {@" << endl;
			for(Class* sub : sub_classes) {
				out << "case " << hash32_of(sub) << ":" << endl;
			}
			out << "\t*this = (const " << type_name << "&)_value; return true;" << endl;
			out << "default: return false;" << endl;
			out << "$}" << endl << "$}" << endl << endl;
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
			out << header << "int " << scope << "get_field_index(vnl::Hash32 _hash) const {@" << endl;
			out << "switch(_hash) {@" << endl;
			int index = 0;
			for(Field* field : all_fields) {
				out << "case " << hash32_of(field) << ": return " << index++ << ";" << endl;
			}
			out << "default: return -1;" << endl;
			out << "$}" << endl << "$}" << endl << endl;
			
			out << header << "const char* " << scope << "get_field_name(int _index) const {@" << endl;
			out << "switch(_index) {@" << endl;
			index = 0;
			for(Field* field : all_fields) {
				out << "case " << index++ << ": return \"" << field->name << "\";" << endl;
			}
			out << "default: return 0;" << endl;
			out << "$}" << endl << "$}" << endl << endl;
			
			out << header << "void " << scope << "get_field(int _index, vnl::String& _str) const {@" << endl;
			out << "switch(_index) {@" << endl;
			index = 0;
			for(Field* field : all_fields) {
				out << "case " << index++ << ": vnl::to_string(_str, " << field->name << "); break;" << endl;
			}
			out << "$}" << endl << "$}" << endl << endl;
			
			out << header << "void " << scope << "set_field(int _index, const vnl::String& _str) {@" << endl;
			out << "switch(_index) {@" << endl;
			index = 0;
			for(Field* field : all_fields) {
				out << "case " << index++ << ": vnl::from_string(_str, " << field->name << "); break;" << endl;
			}
			out << "$}" << endl << "$}" << endl << endl;
			
			out << header << "void " << scope << "get_field(int _index, vnl::io::TypeOutput& _out) const {@" << endl;
			out << "switch(_index) {@" << endl;
			index = 0;
			for(Field* field : all_fields) {
				out << "case " << index++ << ": vnl::write(_out, " << field->name << "); break;" << endl;
			}
			out << "default: _out.putNull();" << endl;
			out << "$}" << endl << "$}" << endl << endl;
			
			out << header << "void " << scope << "set_field(int _index, vnl::io::TypeInput& _in) {@" << endl;
			out << "switch(_index) {@" << endl;
			index = 0;
			for(Field* field : all_fields) {
				out << "case " << index++ << ": vnl::read(_in, " << field->name << "); break;" << endl;
			}
			out << "$}" << endl << "$}" << endl << endl;
			
			out << header << "void " << scope << "get_field(int _index, vnl::Var& _var) const {@" << endl;
			out << "switch(_index) {@" << endl;
			index = 0;
			for(Field* field : all_fields) {
				out << "case " << index++ << ": _var = " << field->name << "; break;" << endl;
			}
			out << "default: _var.clear();" << endl;
			out << "$}" << endl << "$}" << endl << endl;
			
			out << header << "void " << scope << "set_field(int _index, const vnl::Var& _var) {@" << endl;
			out << "switch(_index) {@" << endl;
			index = 0;
			for(Field* field : all_fields) {
				out << "case " << index++ << ": _var.to(" << field->name << "); break;" << endl;
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
			echo_handle_switch(p_object, "*_packet");
			out << "return Super::handle_switch(_sample, _packet);" << endl << "$}" << endl << endl;
			
			out << header << "bool " << scope << "handle_switch(vnl::Value* _sample, vnl::Basic* _input) {@" << endl;
			echo_handle_switch(p_object, "_input");
			out << "return Super::handle_switch(_sample, _input);" << endl << "$}" << endl << endl;
		}
		
		if(p_enum) {
			for(string& value : p_enum->values) {
				out << "const uint32_t " << scope << value << ";" << endl;
			}
			out << endl << header << "void " << scope << "to_string_ex(vnl::String& str) const {@" << endl;
			out << "str.push_back('\"');" << endl;
			out << "switch(value) {@" << endl;
			for(string& field : p_enum->values) {
				out << "case " << hash32_of(field) << ": str << \"" << field << "\"; break;" << endl;
			}
			out << "default: str << \"?\";" << endl;
			out << "$}" << endl;
			out << "str.push_back('\"');" << endl;
			out << "$}" << endl << endl;
		}
		
		if(is_base_value && p_class) {
			out << "vnl::Array<vnl::String> get_class_names() {@" << endl;
			out << "vnl::Array<vnl::String> res;" << endl;
			for(Class* sub : sub_classes) {
				out << "res.push_back(\"" << sub->get_full_name() << "\");" << endl;
			}
			out << "return res;" << endl << "$}" << endl << endl;
			
			generate_type_info();
		}
		
		if(!is_template) {
			namespace_end();
		}
	}
	
	void generate_type_info() {
		vector<Struct*> all_structs;
		vector<Enum*> all_enums;
		vector<Interface*> all_interfaces;
		for(auto entry : INDEX) {
			Struct* p_struct = dynamic_cast<Struct*>(entry.second);
			Enum* p_enum = dynamic_cast<Enum*>(entry.second);
			Interface* p_interface = dynamic_cast<Interface*>(entry.second);
			if(p_struct) {
				all_structs.push_back(p_struct);
			}
			if(p_enum) {
				all_enums.push_back(p_enum);
			}
			if(p_interface) {
				all_interfaces.push_back(p_interface);
			}
		}
		out << "vnl::Map<vnl::Hash32, vnl::info::Type> get_type_info() {@" << endl;
		out << "vnl::Map<vnl::Hash32, vnl::info::Type> res;" << endl;
		for(Struct* sub : all_structs) {
			out << "{@" << endl << "vnl::info::Type& info = res[\"" << sub->get_full_name() << "\"];" << endl;
			out << "info.hash = " << hash32_of(sub) << ";" << endl;
			out << "info.name = \"" << sub->get_full_name() << "\";" << endl;
			out << "info.is_struct = true;" << endl;
			TypeName* super = sub->super;
			if(dynamic_cast<Class*>(sub)) {
				out << "info.is_class = true;" << endl;
				super = ((Class*)sub)->super;
			}
			if(super) {
				out << "info.super.hash = " << hash32_of(super->type) << ";" << endl;
				out << "info.super.name = \"" << get_full_name(super->type) << "\";" << endl;
				for(Base* tmpl : super->tmpl) {
					out << "info.super.generics.push_back(\"" << get_full_name(tmpl) << "\");" << endl;
				}
			}
			for(Field* field : sub->all_fields) {
				out << "{@" << endl << "vnl::info::Field& field = *info.fields.push_back();" << endl;
				out << "field.hash = " << hash32_of(field) << ";" << endl;
				out << "field.name = \"" << field->name << "\";" << endl;
				out << "field.type.hash = " << hash32_of(field->type) << ";" << endl;
				out << "field.type.name = \"" << get_full_name(field->type) << "\";" << endl;
				for(Base* tmpl : field->tmpl) {
					out << "field.type.generics.push_back(\"" << get_full_name(tmpl) << "\");" << endl;
				}
				if(!field->value.empty()) {
					out << "vnl::to_string(field.value, " << field->value << ");" << endl;
				}
				out << "$}" << endl;
			}
			out << "$}" << endl;
		}
		for(Enum* sub : all_enums) {
			out << "{@" << endl << "vnl::info::Type& info = res[\"" << sub->get_full_name() << "\"];" << endl;
			out << "info.hash = " << hash32_of(sub) << ";" << endl;
			out << "info.name = \"" << sub->get_full_name() << "\";" << endl;
			out << "info.is_enum = true;" << endl;
			for(string& value : sub->values) {
				out << "info.symbols.push_back(\"" << value << "\");" << endl;
			}
			out << "$}" << endl;
		}
		for(Interface* sub : all_interfaces) {
			out << "{@" << endl << "vnl::info::Type& info = res[\"" << sub->get_full_name() << "\"];" << endl;
			out << "info.hash = " << hash32_of(sub) << ";" << endl;
			out << "info.name = \"" << sub->get_full_name() << "\";" << endl;
			out << "info.is_interface = true;" << endl;
			TypeName* super = sub->super;
			if(dynamic_cast<Object*>(sub)) {
				out << "info.is_object = true;" << endl;
				super = ((Object*)sub)->super;
			}
			if(super) {
				out << "info.super.hash = " << hash32_of(super->type) << ";" << endl;
				out << "info.super.name = \"" << get_full_name(super->type) << "\";" << endl;
				for(Base* tmpl : super->tmpl) {
					out << "info.super.generics.push_back(\"" << get_full_name(tmpl) << "\");" << endl;
				}
			}
			for(Method* method : sub->methods) {
				out << "{@" << endl << "vnl::info::Method& method = *info.methods.push_back();" << endl;
				out << "method.hash = " << hash32_of(method) << ";" << endl;
				out << "method.name = \"" << method->name << "\";" << endl;
				out << "method.is_const = " << (method->is_const ? "true" : "false") << ";" << endl;
				out << "method.type.hash = " << hash32_of(method->type) << ";" << endl;
				out << "method.type.name = \"" << get_full_name(method->type) << "\";" << endl;
				for(Base* tmpl : method->tmpl) {
					out << "method.type.generics.push_back(\"" << get_full_name(tmpl) << "\");" << endl;
				}
				// TODO: params
				out << "$}" << endl;
			}
			out << "$}" << endl;
		}
		out << "return res;" << endl << "$}" << endl << endl;
	}
	
	void generate_writer(Interface* p_iface, string super) {
		out << "class Writer ";
		if(super.size()) {
			out << ": public " << super << "::Writer ";
		}
		out << "{" << endl << "public:@" << endl;
		out << "Writer(vnl::io::TypeOutput& _out, bool _top_level = false) " << endl << "\t:\t";
		if(super.size()) {
			out << super << "::Writer(_out, false)," << endl << "\t\t";
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
		
		string client_name = p_iface->name + "Client";
		string super;
		string import;
		bool is_base_object = false;
		if(p_iface->super) {
			import = full(p_iface->super->type) + "Client";
			super = import + get_tmpl(p_iface->super);
		}
		if(import == "vnl::InterfaceClient") {
			super = "vnl::Client";
			import = "vnl::Client";
			is_base_object = true;
		}
		
		set<Type*> imports = p_iface->imports;
		imports.erase((Type*)p_iface->super->type);
		echo_includes(imports);
		out << "#include <" << subs(import, "::", "/") << (is_base_object ? ".h>" : ".hxx>") << endl << endl;
		namespace_begin();
		
		if(!p_iface->generic.empty()) {
			out << get_tmpl_header(p_iface) << endl;
		}
		out << "class " << client_name << " : public " << super << " {" << endl;
		out << "public:@" << endl;
		generate_writer(p_iface, (is_base_object ? "" : super));
		out << endl << client_name << "() {}" << endl;
		out << endl << client_name << "(const " << client_name << "& other) {@" << endl;
		out << "vnl::Client::set_address(other.vnl::Client::get_address());" << endl << "$}" << endl;
		out << endl << client_name << "(const vnl::Address& addr) {@" << endl;
		out << "vnl::Client::set_address(addr);" << endl << "$}" << endl;
		out << endl << client_name << "& operator=(const vnl::Address& addr) {@" << endl;
		out << "vnl::Client::set_address(addr);" << endl << "return *this;" << endl << "$}" << endl << endl;
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
		echo_type(method);
		out << " " << method->name << "(";
		echo_method_params(method, true);
		out << ") {@" << endl;
		out << "_buf.wrap(_data);" << endl;
		out << "{@" << endl << "Writer _wr(_out);" << endl;
		out << "_wr." << method->name << "(";
		echo_method_args(method);
		out << ");" << endl << "$}" << endl;
		out << "vnl::Packet* _pkt = _call(vnl::Frame::" << (method->is_const ? "CONST_CALL" : "CALL") << ");" << endl;
		if(method->is_const) {
			echo_type(method);
			out << " _result;" << endl;
		}
		out << "if(_pkt) {@" << endl;
		if(method->is_const) {
			out << "vnl::read(_in, _result);" << endl;
		}
		out << "_pkt->ack();" << endl;
		out << "$} else {@" << endl << "throw vnl::IOException();" << endl << "$}" << endl;
		if(method->is_const) {
			out << "return _result;" << endl;
		}
		out << "$}" << endl << endl;
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
	
	void echo_handle_switch(Object* module, string payload) {
		vector<Class*> types(module->handles.begin(), module->handles.end());
		sort(types.begin(), types.end(), [](Class* a, Class* b) -> bool {
			return a->get_full_name() < b->get_full_name();
		});
		out << "switch(_sample->get_vni_hash()) {" << endl;
		for(Class* p_class : types) {
			out << "case " << hash32_of(p_class) << ": ";
			out << "handle(*((" << full(p_class) << "*)_sample), " << payload << "); return true;" << endl;
		}
		out << "}" << endl;
	}
	
	bool is_base_value = false;
	bool is_template = false;
	vector<string> name_space;
	string base_name;
	string type_name;
	TypeName* p_super = 0;
	
	ostringstream out;
	
};




}}}

#endif /* INCLUDE_CPP_GENERATOR_H_ */
