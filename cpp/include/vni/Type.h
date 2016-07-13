/*
 * Type.h
 *
 *  Created on: Jun 27, 2016
 *      Author: mad
 */

#ifndef CPP_VNI_TYPE_H_
#define CPP_VNI_TYPE_H_

#include <vnl/io.h>
#include <vnl/Vector.h>


namespace vni {

class Type : public vnl::io::Serializable {
public:
	Type() {}
	Type(const Type& other) = delete;
	virtual ~Type() {}
	
	virtual uint32_t vni_hash() const = 0;
	virtual const char* type_name() const = 0;
	virtual int num_fields() const = 0;
	virtual int field_index(vnl::Hash32 hash) const = 0;
	virtual const char* field_name(int index) const = 0;
	virtual void get_field(int index, vnl::String& str) const = 0;
	virtual void set_field(int index, const vnl::String& str) = 0;
	
	virtual void from_string(const vnl::String& str) const {
		// TODO
	}
	
	virtual void to_string(vnl::String& str) const {
		str << "{";
		int N = num_fields();
		for(int i = 0; i < N; ++i) {
			str << field_name(i) << ": ";
			get_field(i, str);
			if(i < N-1) {
				str << ", ";
			}
		}
		str << "}";
	}
	
	vnl::String to_string() const {
		vnl::String str;
		to_string(str);
		return str;
	}
	
};



}

#endif /* CPP_VNI_TYPE_H_ */
