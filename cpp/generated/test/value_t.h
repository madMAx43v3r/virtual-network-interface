/*
 * Value.h
 *
 *  Created on: May 22, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_TEST_VALUE_T_H_
#define CPP_GENERATED_TEST_VALUE_T_H_

#include <vni/Type.h>


namespace test {

class value_t : public vni::Type {
public:
	float pos[3];
	
	static const uint32_t HASH = 0x51677428;
	
	value_t() {
		vni_hash_ = HASH;
		for(int i = 0; i < 3; ++i) { pos[i] = 0; }
	}
	
	static void read(vnl::io::TypeInputStream& in, value_t* obj) {
		uint32_t size = 0;
		int id = in.getEntry(size);
		if(id == VNL_IO_STRUCT) {
			obj->deserialize(in, size);
		} else {
			in.skip(id, size);
		}
	}
	
	virtual const char* vni_type_name() const {
		return "test.value_t";
	}
	
	virtual void serialize(vnl::io::TypeOutputStream& out) const {
		out.putEntry(VNL_IO_STRUCT, 1);
		out.putHash(0x8356785);
		out.putArray(3, pos);
	}
	
	virtual void deserialize(vnl::io::TypeInputStream& in, uint32_t num_entries) {
		uint32_t hash = 0;
		uint32_t size = 0;
		for(int i = 0; i < num_entries; ++i) {
			in.getHash(hash);
			int id = in.getEntry(size);
			switch(hash) {
				case 0x8356785: in.getArray(3, pos, size); break;
				default: in.skip(id, size);
			}
		}
	}
	
};


}

#endif /* CPP_GENERATED_TEST_VALUE_T_H_ */
