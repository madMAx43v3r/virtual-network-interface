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
	
	template<typename T>
	static void read(vnl::io::TypeInput& in, T* obj) {
		int size = 0;
		int id = in.getEntry(size);
		if(id == VNL_IO_STRUCT) {
			obj->deserialize(in, size);
		} else {
			in.skip(id, size);
		}
	}
	
	
};



}

#endif /* INCLUDE_VNI_STRUCT_H_ */
