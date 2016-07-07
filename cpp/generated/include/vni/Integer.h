/*
 * Integer.h
 *
 *  Created on: Jul 6, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_INTEGER_H_
#define INCLUDE_VNI_INTEGER_H_

#include <vni/Value.h>


namespace vni {

class Integer : public vni::Value {
public:
	uint64_t value;
	
	static const uint32_t VNI_HASH = 0x5df232ac;
	static const int VNI_NUM_FIELDS = 1;
	
	Integer() {
		vni_hash_ = VNI_HASH;
		value = 0;
	}
	
	static Integer* create() {
		return create(VNI_HASH);
	}
	
	static Integer* create(uint32_t hash) {
		switch(hash) {
			case VNI_HASH: return vni::Pool<Integer>::create();
			default: return 0;
		}
	}
	
	static void destroy(Integer* obj) {
		if(obj) {
			switch(obj->vni_hash_) {
				case VNI_HASH: vni::Pool<Integer>::destroy(obj);
			}
		}
	}
	
	virtual const char* vni_type_name() const {
		return "vni.Integer";
	}
	
};


}

#endif /* INCLUDE_VNI_INTEGER_H_ */
