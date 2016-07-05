/*
 * Interface.h
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_INTERFACE_H_
#define INCLUDE_VNI_INTERFACE_H_

#include <vni/Type.h>


namespace vni {

class Interface : public Type {
public:
	virtual void deserialize(vnl::io::TypeInput& in, int size) {
		int stack = 1;
		while(!in.error()) {
			int size = 0;
			int id = in.getEntry(size);
			if(id == VNL_IO_CALL) {
				uint32_t hash = 0;
				in.getHash(hash);
				if(!vni_call(in, hash, size)) {
					for(uint32_t i = 0; i < size; ++i) {
						in.skip();
					}
				}
			} else if(id == VNL_IO_INTERFACE) {
				if(size == VNL_IO_BEGIN) {
					uint32_t hash = 0;
					in.getHash(hash);
					stack++;
				} else if(size == VNL_IO_END) {
					stack--;
					if(stack == 0) {
						break;
					}
				}
			} else {
				in.skip(id, size);
			}
		}
	}
	
	static void read(vnl::io::TypeInput& in, Interface* obj) {
		int size = 0;
		int id = in.getEntry(size);
		if(id == VNL_IO_INTERFACE) {
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
	
protected:
	virtual bool vni_call(vnl::io::TypeInput& in, uint32_t hash, int num_args) = 0;
	virtual bool vni_const_call(vnl::io::TypeInput& in, uint32_t hash, int num_args, vnl::io::TypeOutput& out) = 0;
	
};




}

#endif /* INCLUDE_VNI_INTERFACE_H_ */
