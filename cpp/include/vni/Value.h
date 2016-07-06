/*
 * Class.h
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_CLASS_H_
#define INCLUDE_VNI_CLASS_H_

#include <vni/Type.h>


namespace vni {

class Value : public Type {
public:
	template<typename T>
	static T* create(uint32_t hash);
	
	template<typename T>
	static void destroy(T* obj);
	
	class Writer {
	public:
		Writer(vnl::io::TypeOutput& out_, int size) : out(out_) {
			out.putEntry(VNL_IO_STRUCT, size);
		}
		
		Writer(vnl::io::TypeOutput& out_, int size, uint32_t hash) : out(out_) {
			out.putEntry(VNL_IO_CLASS, size);
			out.putHash(hash);
		}
		
		template<typename T>
		void putField(uint32_t hash, const T* value) {
			out.putHash(hash);
			out.putNull();
		}
		
		template<typename T>
		void putField(uint32_t hash, const T& value) {
			out.putHash(hash);
			out.putValue(value);
		}
		
		template<typename T>
		void putField(uint32_t hash, const T* data, int dim) {
			out.putHash(hash);
			out.putArray(data, dim);
		}
		
	protected:
		vnl::io::TypeOutput out;
		
	};
	
};


template<>
void Value::Writer::putField<Value>(uint32_t hash, const Value* value) {
	out.putHash(hash);
	value->serialize(out);
}

template<>
void Value::Writer::putField<Value>(uint32_t hash, const Value& value) {
	out.putHash(hash);
	value.serialize(out);
}


template<typename T>
inline T* read(vnl::io::TypeInput& in) {
	T* obj = 0;
	int size = 0;
	int id = in.getEntry(size);
	switch(id) {
		case VNL_IO_STRUCT:
			obj = T::create();
			break;
		case VNL_IO_CLASS: {
			uint32_t hash = 0;
			in.getHash(hash);
			obj = T::create(hash);
			break;
		}
	}
	if(obj) {
		obj->deserialize(in, size);
	} else {
		in.skip(id, size);
	}
	return obj;
}

inline void read(vnl::io::TypeInput& in, Value* obj) {
	int size = 0;
	int id = in.getEntry(size);
	if(id == VNL_IO_STRUCT) {
		obj->deserialize(in, size);
	} else if(id == VNL_IO_CLASS) {
		uint32_t hash = 0;
		in.getHash(hash);
		if(hash == obj->vni_hash_) {
			obj->deserialize(in, size);
		} else {
			in.skip(id, size);
		}
	} else {
		in.skip(id, size);
	}
}


template<typename T>
inline void write(vnl::io::TypeOutput& out, const T& obj) {
	// TODO
}

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







}

#endif /* INCLUDE_VNI_CLASS_H_ */
