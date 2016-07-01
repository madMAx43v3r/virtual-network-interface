/*
 * Struct.h
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_STRUCT_H_
#define INCLUDE_VNI_STRUCT_H_

#include <vni/Type.h>


namespace vni {

class Struct : public vni::Type {
public:
	
	static Struct* create(uint32_t hash);
	
	static void destroy(Struct* obj);
	
protected:
	template<typename T>
	static void read(vnl::io::TypeInput& in, T* obj) {
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
	static T* read(vnl::io::TypeInput& in) {
		T* obj = 0;
		int size = 0;
		int id = in.getEntry(size);
		if(id == VNL_IO_STRUCT) {
			obj = T::create();
		} else if(id == VNL_IO_CLASS) {
			uint32_t hash = 0;
			in.getHash(hash);
			obj = T::create(hash);
		}
		if(obj) {
			obj->deserialize(in, size);
		} else {
			in.skip(id, size);
		}
		return obj;
	}
	
	
};



}

#endif /* INCLUDE_VNI_STRUCT_H_ */
