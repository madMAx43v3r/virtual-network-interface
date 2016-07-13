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
	Bool* p_bool = dynamic_cast<Bool*>(source); \
	Integer* p_int = dynamic_cast<Integer*>(source); \
	Real* p_real = dynamic_cast<Real*>(source); \
	Array* p_array = dynamic_cast<Array*>(source); \
	Enum* p_enum = dynamic_cast<Enum*>(source); \
	Struct* p_struct = dynamic_cast<Struct*>(source); \
	Class* p_class = dynamic_cast<Class*>(source); \
	Interface* p_iface = dynamic_cast<Interface*>(source); \
	Object* p_object = dynamic_cast<Object*>(source); \
	Node* p_node = dynamic_cast<Node*>(source);


namespace vni {
namespace codegen {
namespace cpp {

class Generator : public Backend {
public:
	virtual void generate(Type* type) {
		Interface* p_iface = dynamic_cast<Interface*>(type);
		
		// generate header
		out.str("");
		generate_header(type);
		string header_dir = "include/" + subs(type->package->name, ".", "/") + "/";
		update(header_dir, type->name + (p_iface ? "Base" : "") + ".hxx", out.str());
		
		// generate source
		out.str("");
		generate_source(type);
		string source_dir = "src/";
		string source_name = type->package->name + "." + type->name;
		if(p_iface) {
			source_dir = header_dir;
			source_name = type->name + ".hxx";
		} else {
			source_name += ".cxx";
		}
		update(source_dir, source_name, out.str());
	}
	
	static string full(string name) {
		return subs(name, ".", "::");
	}
	
	static string full(Base* type) {
		return subs(type->get_full_name(), ".", "::");
	}
	
	static string hash_of(Base* type) {
		char buf[64];
		int len = snprintf(buf, 64, "0x%x", (uint32_t)type->get_hash());
		return string(buf, len);
	}
	
	static string hash_of(string str) {
		char buf[64];
		int len = snprintf(buf, 64, "0x%x", (uint32_t)hash64(str));
		return string(buf, len);
	}
	
	void namespace_begin() {
		for(string space : name_space) {
			out << "namespace " << space << " {" << endl;
		}
	}
	
	void namespace_end() {
		for(string space : name_space) {
			out << "} // namespace" << endl;
		}
	}
	
	void echo_type(Base* type) {
		TYPE_TREE(type);
		if(p_object) {
			out << full(type) << "::Client";
		} else if(p_struct) {
			out << full(type);
		} else if(p_enum) {
			out << full(type);
		} else if(p_array) {
			out << "vnl::Vector<";
			echo_type(p_array->type);
			out << ", " << p_array->size << ">";
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
		} else if(p_void) {
			out << "void";
		}
	}
	
	void echo_tmpl(vector<Base*>& params) {
		if(params.size()) {
			out << "<";
			for(int i = 0; i < params.size(); ++i) {
				out << full(params[i]);
				if(i < params.size()-1) {
					out << ", ";
				}
			}
			out << " >";
		}
	}
	
	void echo_field(Field* field, bool def, bool init) {
		TYPE_TREE(field->type);
		if(def) {
			echo_type(field->type);
			echo_tmpl(field->tmpl_types);
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
	
	void generate_header(Type* p_type) {
		TYPE_TREE(p_type);
		string base_name = p_type->name;
		if(p_iface) {
			base_name += "Base";
		}
		
		string guard_sym = "INCLUDE_VNI_GENERATED_" + subs(p_type->package->name, ".", "_") + "_" + base_name + "_HXX_";
		out << endl << "#ifndef " << guard_sym << endl;
		out << "#define " << guard_sym << endl << endl;
		out << "// AUTO GENERATED by virtual-network-interface codegen" << endl << endl;
		
		is_base = p_type->get_full_name() == "vni.Value";
		for(string import : p_type->imports) {
			if(is_base && import == "vni.Value") {
				import = "vni.Type";
			}
			out << "#include <" << subs(import, ".", "/") << ".hxx>" << endl;
		}
		out << endl;
		
		name_space = split(p_type->package->name, '.');
		namespace_begin();
		out << endl;
		
		string type_name = p_type->name;
		if(p_iface) {
			string template_str;
			if(p_iface->generic.size()) {
				type_name += "<";
				template_str = "template<";
				for(int i = 0; i < p_iface->generic.size(); ++i) {
					type_name += p_iface->generic[i];
					template_str += "class " + p_iface->generic[i];
					if(i < p_iface->generic.size()-1) {
						template_str += ", ";
						type_name += ", ";
					}
				}
				type_name += ">";
				template_str += ">";
			}
			out << template_str << endl;
		}
		if(p_struct) {
			out << "class " << base_name << " ";
		}
		if(p_class) {
			out << ": public ";
			if(p_class->super) {
				out << full(p_class->super);
			} else if(!is_base) {
				if(p_node && p_type->name != "Node") {
					out << "vni::Node ";
				} else if(p_object && p_type->name != "Object") {
					out << "vni::Object ";
				} else if(p_iface && p_type->name != "Interface") {
					out << "vni::Interface ";
				} else {
					out << "vni::Value ";
				}
			} else {
				out << "vni::Type ";
			}
		}
		
		if(p_struct) {
			out << "{" << endl;
			out << "public:@" << endl;
			out << "static const uint32_t VNI_HASH = " << hash_of(p_type) << ";" << endl;
			out << "static const uint32_t NUM_FIELDS = " << p_struct->all_fields.size() << ";" << endl;
			out << "static const char* VNI_NAME = \"" << p_type->get_full_name() << "\";" << endl << endl;
			
			for(Field* field : p_struct->fields) {
				echo_type(field->type);
				echo_tmpl(field->tmpl_types);
				out << " " << field->name << ";" << endl;
			}
			
			out << endl << base_name << "() {@" << endl;
			for(Field* field : p_struct->fields) {
				if(!field->value.empty()) {
					out << field->name << " = " << field->value << ";" << endl;
				} else if(dynamic_cast<Primitive*>(field->type)) {
					out << field->name << " = 0;" << endl;
				}
			}
			out << "$}" << endl << endl;
		}
		
		if(p_struct) {
			if(!p_iface) {
				out << "static " << type_name << "* create();" << endl;
				out << "virtual " << type_name << "* clone() const;" << endl;
				out << "virtual void destroy();" << endl << endl;
			}
			out << "virtual uint32_t vni_hash() const { return VNI_HASH; }" << endl;
			out << "virtual const char* type_name() const { return VNI_NAME; }" << endl;
			out << "virtual int num_fields() const { return NUM_FIELDS; }" << endl;
			out << "virtual int field_index(vnl::Hash32 hash) const;" << endl;
			out << "virtual const char* field_name(int index) const;" << endl;
			out << "virtual void get_field(int index, vnl::String& str) const;" << endl;
			out << "virtual void set_field(int index, const vnl::String& str);" << endl << endl;
			out << "virtual void serialize(vnl::io::TypeOutput& out) const;" << endl;
			out << "virtual void deserialize(vnl::io::TypeInput& in, int size);" << endl;
		}
		
		if(p_enum) {
			out << "enum " << type_name << " {@" << endl << endl;
			for(string& value : p_enum->values) {
				out << value << " = " << hash_of(value) << "," << endl;
			}
		}
		
		out << endl << "$};" << endl << endl;
		namespace_end();
		out << endl << "#endif // " << guard_sym;
	}
	
	void generate_source(Type* p_type) {
		TYPE_TREE(p_type);
		string guard_sym = "INCLUDE_VNI_GENERATED_" + subs(p_type->get_full_name(), ".", "_") + "_HXX_";
		if(p_iface) {
			out << endl << "#ifndef " << guard_sym << endl;
			out << "#define " << guard_sym << endl;
		}
		out << endl << "// AUTO GENERATED by virtual-network-interface codegen" << endl;
		out << endl << "#include <" << subs(p_type->get_full_name(), ".", "/") << (p_iface ? ".h" : ".hxx") << ">" << endl;
		
		vector<Struct*> sub_classes;
		if(is_base) {
			for(auto entry : INDEX) {
				Struct* p_struct = dynamic_cast<Struct*>(entry.second);
				if(p_struct && !dynamic_cast<Interface*>(entry.second)) {
					out << "#include <" << subs(p_struct->get_full_name(), ".", "/") << ".hxx>" << endl;
					sub_classes.push_back(p_struct);
				}
			}
		}
		
		out << endl;
		namespace_begin();
		out << endl;
		
		// TODO
		
		if(is_base) {
			out << "vni::Value* create(vnl::Hash32 hash) {@" << endl;
			out << "switch(hash) {" << endl;
			for(Struct* sub : sub_classes) {
				out << "case " << hash_of(sub) << ": return " << full(sub) << "::create(); break;" << endl;
			}
			out << "default: return 0;" << endl;
			out << "}" << endl << "$}" << endl;
		}
		
		out << endl << endl;
		namespace_end();
		if(p_iface) {
			out << endl << "#endif // " << guard_sym;
		}
	}
	
	bool is_base = false;
	vector<string> name_space;
	ostringstream out;
	
};










}}}

#endif /* INCLUDE_CPP_GENERATOR_H_ */
