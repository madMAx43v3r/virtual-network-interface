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

#include "Pool.h"


namespace vni {

class Type : public vnl::io::Serializable {
public:
	virtual ~Type() {}
	
	virtual uint32_t vni_hash() const = 0;
	virtual const char* type_name() const = 0;
	virtual void to_string(vnl::String& str) const = 0;
	virtual bool is_base(vnl::Hash32 hash) const = 0;
	virtual bool is_instance(vnl::Hash32 hash) const = 0;
	
	vnl::String to_string() const {
		vnl::String str;
		to_string(str);
		return str;
	}
	
};


template<typename T>
inline T* read(vnl::io::TypeInput& in) {
	T* obj = 0;
	int size = 0;
	int id = in.getEntry(size);
	switch(id) {
		case VNL_IO_STRUCT:
			obj = T::create();
			obj->deserialize(in, size);
			break;
		case VNL_IO_CLASS:
		case VNL_IO_INTERFACE: {
			uint32_t hash = 0;
			in.getHash(hash);
			obj = T::create(hash);
			if(obj) {
				obj->deserialize(in, size);
			} else {
				in.skip(id, size, hash);
			}
			break;
		}
		default: in.skip(id, size);
	}
	return obj;
}

inline void read(vnl::io::TypeInput& in, Value& obj) {
	int size = 0;
	int id = in.getEntry(size);
	switch(id) {
		case VNL_IO_STRUCT:
			obj.deserialize(in, size);
			break;
		case VNL_IO_CLASS:
		case VNL_IO_INTERFACE: {
			uint32_t hash = 0;
			in.getHash(hash);
			obj.deserialize(in, size);
			break;
		}
		default: in.skip(id, size);
	}
}

inline void read(vnl::io::TypeInput& in, bool& val) { in.getBool(val); }
inline void read(vnl::io::TypeInput& in, int8_t& val) { in.getValue(val); }
inline void read(vnl::io::TypeInput& in, int16_t& val) { in.getValue(val); }
inline void read(vnl::io::TypeInput& in, int32_t& val) { in.getValue(val); }
inline void read(vnl::io::TypeInput& in, int64_t& val) { in.getValue(val); }
inline void read(vnl::io::TypeInput& in, float& val) { in.getValue(val); }
inline void read(vnl::io::TypeInput& in, double& val) { in.getValue(val); }

template<typename T, int N>
void read(vnl::io::TypeInput& in, vnl::Vector<T, N>& vec) { in.getArray(vec); }


inline void write(vnl::io::TypeOutput& out, const Value* obj) {
	if(obj) {
		obj->serialize(out);
	} else {
		out.putNull();
	}
}

inline void write(vnl::io::TypeOutput& out, const Value& obj) {
	obj.serialize(out);
}

inline void write(vnl::io::TypeOutput& out, const bool& val) { out.putBool(val); }
inline void write(vnl::io::TypeOutput& out, const int8_t& val) { out.putValue(val); }
inline void write(vnl::io::TypeOutput& out, const int16_t& val) { out.putValue(val); }
inline void write(vnl::io::TypeOutput& out, const int32_t& val) { out.putValue(val); }
inline void write(vnl::io::TypeOutput& out, const int64_t& val) { out.putValue(val); }
inline void write(vnl::io::TypeOutput& out, const float& val) { out.putValue(val); }
inline void write(vnl::io::TypeOutput& out, const double& val) { out.putValue(val); }

template<typename T, int N>
void write(vnl::io::TypeOutput& out, const vnl::Vector<T, N>& vec) { out.putArray(vec); }


inline vnl::String to_string(const Value* obj) {
	if(obj) {
		return obj->to_string();
	} else {
		return "null";
	}
}

inline vnl::String to_string(const Value& obj) {
	return obj.to_string();
}



}

#endif /* CPP_VNI_TYPE_H_ */
