/*
 * Type.hxx
 *
 *  Created on: Jul 12, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_TYPE_HXX_
#define INCLUDE_VNI_TYPE_HXX_

#include <vnl/Vector.h>
#include <vni/Type.h>
#include <vni/Pool.h>

#include <vni/Value.hxx>
#include "Interface.h"


namespace vni {

/*
 * Function to create any Value based on the type hash.
 */
Value* create(vnl::Hash32 hash);

/*
 * Generic reader functions
 */
inline Value* read(vnl::io::TypeInput& in) {
	Value* obj = 0;
	int size = 0;
	int id = in.getEntry(size);
	switch(id) {
		case VNL_IO_CLASS: {
			uint32_t hash = 0;
			in.getHash(hash);
			obj = vni::create(hash);
			if(obj) {
				obj->deserialize(in, size);
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
		case VNL_IO_CLASS: {
			uint32_t hash = 0;
			in.getHash(hash);
			obj.deserialize(in, size);
			break;
		}
		default: in.skip(id, size);
	}
}

inline void read(vnl::io::TypeInput& in, Value* obj) {
	if(obj) {
		read(in, *obj);
	} else {
		in.skip();
	}
}

inline void read(vnl::io::TypeInput& in, Interface& obj) {
	int size = 0;
	int id = in.getEntry(size);
	switch(id) {
		case VNL_IO_INTERFACE: {
			uint32_t hash = 0;
			in.getHash(hash);
			obj.deserialize(in, size);
			break;
		}
		default: in.skip(id, size);
	}
}

inline void read(vnl::io::TypeInput& in, bool& val) { in.getValue(val); }
inline void read(vnl::io::TypeInput& in, int8_t& val) { in.getValue(val); }
inline void read(vnl::io::TypeInput& in, int16_t& val) { in.getValue(val); }
inline void read(vnl::io::TypeInput& in, int32_t& val) { in.getValue(val); }
inline void read(vnl::io::TypeInput& in, int64_t& val) { in.getValue(val); }
inline void read(vnl::io::TypeInput& in, float& val) { in.getValue(val); }
inline void read(vnl::io::TypeInput& in, double& val) { in.getValue(val); }

template<typename T, int N>
void read(vnl::io::TypeInput& in, vnl::Vector<T, N>& vec) { in.getArray(vec); }


/*
 * Generic writer functions
 */
inline void write(vnl::io::TypeOutput& out, const Value& obj) {
	obj.serialize(out);
}

inline void write(vnl::io::TypeOutput& out, const Value* obj) {
	if(obj) {
		obj->serialize(out);
	} else {
		out.putNull();
	}
}

inline void write(vnl::io::TypeOutput& out, const Interface& obj) {
	obj.serialize(out);
}

inline void write(vnl::io::TypeOutput& out, const bool& val) { out.putValue(val); }
inline void write(vnl::io::TypeOutput& out, const int8_t& val) { out.putValue(val); }
inline void write(vnl::io::TypeOutput& out, const int16_t& val) { out.putValue(val); }
inline void write(vnl::io::TypeOutput& out, const int32_t& val) { out.putValue(val); }
inline void write(vnl::io::TypeOutput& out, const int64_t& val) { out.putValue(val); }
inline void write(vnl::io::TypeOutput& out, const float& val) { out.putValue(val); }
inline void write(vnl::io::TypeOutput& out, const double& val) { out.putValue(val); }

template<typename T, int N>
void write(vnl::io::TypeOutput& out, const vnl::Vector<T, N>& vec) { out.putArray(vec); }


/*
 * Generic to_string functions
 */
inline void to_string(vnl::String& str, const Value& obj) {
	return obj.to_string(str);
}

inline void to_string(vnl::String& str, const Value* obj) {
	if(obj) {
		obj->to_string(str);
	} else {
		str << "{}";
	}
}

inline void to_string(vnl::String& str, const Interface& obj) {
	return obj.to_string(str);
}

inline void to_string(vnl::String& str, const bool& val) { str << (val ? "true" : "false"); }
inline void to_string(vnl::String& str, const int8_t& val) { str << vnl::dec(val); }
inline void to_string(vnl::String& str, const int16_t& val) { str << vnl::dec(val); }
inline void to_string(vnl::String& str, const int32_t& val) { str << vnl::dec(val); }
inline void to_string(vnl::String& str, const int64_t& val) { str << vnl::dec(val); }
inline void to_string(vnl::String& str, const float& val) { str << vnl::def(val, 6); }
inline void to_string(vnl::String& str, const double& val) { str << vnl::def(val, 12); }

template<typename T, int N>
void to_string(vnl::String& str, const vnl::Vector<T, N>& vec) {
	str << "[";
	for(int i = 0; i < vec.size(); ++i) {
		to_string(str, vec[i]);
		if(i < vec.size()-1) {
			str << ", ";
		}
	}
	str << "]";
}

template<typename T>
vnl::String to_string(const T& ref) {
	vnl::String str;
	to_string(str, ref);
	return str;
}


/*
 * Generic from_string functions
 */
inline void from_string(vnl::io::ByteInput& in, const Value& obj) {
	return obj.from_string(in);
}

inline void from_string(vnl::io::ByteInput& in, const Value* obj) {
	if(obj) {
		obj->from_string(in);
	}
}

inline void from_string(vnl::io::ByteInput& in, const Interface& obj) {
	return obj.from_string(in);
}

inline void from_string(vnl::io::ByteInput& in, const bool& val) { /* TODO */ }
inline void from_string(vnl::io::ByteInput& in, const int8_t& val) { /* TODO */ }
inline void from_string(vnl::io::ByteInput& in, const int16_t& val) { /* TODO */ }
inline void from_string(vnl::io::ByteInput& in, const int32_t& val) { /* TODO */ }
inline void from_string(vnl::io::ByteInput& in, const int64_t& val) { /* TODO */ }
inline void from_string(vnl::io::ByteInput& in, const float& val) { /* TODO */ }
inline void from_string(vnl::io::ByteInput& in, const double& val) { /* TODO */ }

template<typename T, int N>
void from_string(vnl::io::ByteInput& in, const vnl::Vector<T, N>& vec) {
	// TODO
}

template<typename T>
void from_string(const vnl::String& str, const T& ref) {
	// TODO
}






} // vni

#endif /* INCLUDE_VNI_TYPE_HXX_ */
