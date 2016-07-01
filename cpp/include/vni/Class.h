/*
 * Class.h
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_CLASS_H_
#define INCLUDE_VNI_CLASS_H_

#include <vni/Struct.h>


namespace vni {

class Class : public Struct {
public:
	
	static Class* create(uint32_t hash);
	
	static void destroy(Class* obj);
	
	template<typename T>
	static T* read(vnl::io::TypeInput& in) {
		T* obj = 0;
		uint32_t hash = 0;
		int size = 0;
		int id = in.getEntry(size);
		if(id == VNL_IO_CLASS) {
			in.getHash(hash);
			obj = T::create(hash);
			if(obj) {
				obj->deserialize(in, size);
			}
		}
		if(!obj) {
			in.skip(id, size);
		}
		return obj;
	}
	
	
};



}

#endif /* INCLUDE_VNI_CLASS_H_ */
