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

class Class : public Type {
public:
	
	static Class* create(uint32_t hash);
	
	static void destroy(Class* obj);
	
protected:
	static void read(vnl::io::TypeInput& in, Class* obj) {
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

#endif /* INCLUDE_VNI_CLASS_H_ */
