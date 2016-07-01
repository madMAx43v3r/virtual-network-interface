/*
 * Value.h
 *
 *  Created on: May 22, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_TEST_VALUE_T_H_
#define CPP_GENERATED_TEST_VALUE_T_H_

#include <vni/Struct.h>


namespace test {

class value_t : public vni::Struct {
public:
	float pos[3];
	
	static const uint32_t HASH = 0x51677428;
	
	value_t() {
		vni_hash_ = HASH;
		memset(pos, 0, sizeof(pos));
	}
	
	static value_t* create() {
		return vni::TypePool<value_t>::create();
	}
	
	static void destroy(value_t* obj) {
		if(obj) {
			vni::TypePool<value_t>::destroy(obj);
		}
	}
	
	static void read(vnl::io::TypeInput& in, value_t* obj) {
		vni::Struct::read<value_t>(in, obj);
	}
	
	static value_t* read(vnl::io::TypeInput& in) {
		return vni::Struct::read<value_t>(in);
	}
	
	virtual const char* vni_type_name() const {
		return "test.value_t";
	}
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		out.putEntry(VNL_IO_STRUCT, 1);
		out.putHash(0x8356785);
		out.putArray(pos, 3);
	}
	
	virtual void deserialize(vnl::io::TypeInput& in, int size) {
		for(int i = 0; i < size; ++i) {
			uint32_t hash = 0;
			in.getHash(hash);
			switch(hash) {
				case 0x8356785: in.getArray(pos, 3); break;
				default: in.skip();
			}
		}
	}
	
};


}

#endif /* CPP_GENERATED_TEST_VALUE_T_H_ */
