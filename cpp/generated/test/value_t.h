/*
 * Value.h
 *
 *  Created on: May 22, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_TEST_VALUE_T_H_
#define CPP_GENERATED_TEST_VALUE_T_H_

#include <vni/Value.h>


namespace test {

class value_t : public vni::Value {
public:
	float pos[3];
	
	static const uint32_t VNI_HASH = 0x51677428;
	static const int VNI_NUM_FIELDS = 1;
	
	value_t() {
		vni_hash_ = VNI_HASH;
		memset(pos, 0, sizeof(pos));
	}
	
	static value_t* create() {
		return vni::GlobalPool<value_t>::create();
	}
	
	static void destroy(value_t* obj) {
		if(obj) {
			vni::GlobalPool<value_t>::destroy(obj);
		}
	}
	
	virtual const char* vni_type_name() const {
		return "test.value_t";
	}
	
	static void write_header(vnl::io::TypeOutput& out) {
		out.putHash(0x8356785);
	}
	
	void write_body(vnl::io::TypeOutput& out) const {
		out.putArray(pos, 3);
	}
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		out.putEntry(VNL_IO_STRUCT, VNI_NUM_FIELDS);
		write_header(out);
		write_body(out);
	}
	
	virtual void deserialize(vnl::io::TypeInput& in, int size) {
		if(!in.inc_stack()) {
			return;
		}
		uint32_t hash[VNL_IO_MAX_NUM_FIELDS];
		int num = std::min(size, VNL_IO_MAX_NUM_FIELDS);
		for(int i = 0; i < num; ++i) {
			in.getHash(hash[i]);
		}
		for(int i = num; i < size && !in.error(); ++i) {
			uint32_t tmp; in.getHash(tmp);
		}
		for(int i = 0; i < num; ++i) {
			switch(hash[i]) {
				case 0x8356785: in.getArray(pos, 3); break;
				default: in.skip();
			}
		}
		for(int i = num; i < size && !in.error(); ++i) {
			in.skip();
		}
		in.dec_stack();
	}
	
};


}

#endif /* CPP_GENERATED_TEST_VALUE_T_H_ */
