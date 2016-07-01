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
		while(!in.error()) {
			uint32_t hash = 0;
			int size = 0;
			int id = in.getEntry(size);
			if(id == VNL_IO_CALL) {
				in.getHash(hash);
				if(!call(in, hash, size)) {
					for(uint32_t i = 0; i < size; ++i) {
						in.skip();
					}
				}
			} else if(id == VNL_IO_INTERFACE && size == VNL_IO_END) {
				break;
			} else {
				in.skip(id, size);
			}
		}
	}
	
protected:
	virtual bool call(vnl::io::TypeInput& in, uint32_t hash, int num_args) = 0;
	virtual bool const_call(vnl::io::TypeInput& in, uint32_t hash, int num_args, vnl::io::TypeOutput& out) = 0;
	
};




}

#endif /* INCLUDE_VNI_INTERFACE_H_ */
