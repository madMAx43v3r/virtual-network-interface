/*
 * Value.h
 *
 *  Created on: May 22, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_TEST_VALUE_H_
#define CPP_GENERATED_TEST_VALUE_H_

#include <vni/Type.h>


namespace test {

class Value : public vni::Type {
public:
	int32_t x = 0;
	int32_t y = 0;
	int32_t z = 0;
	
	static const uint32_t HASH = 0x51212323;
	
	VNI_SAMPLE(Value);
	
	Value() {
		vni_hash_ = HASH;
	}
	
	static Value* create() {
		return create(HASH);
	}
	
	static Value* create(uint32_t hash) {
		switch(hash) {
			case HASH: return vni::TypePool<Value>::create();
			default: return 0;
		}
	}
	
	static void destroy(Value* obj) {
		if(obj) {
			switch(obj->vni_hash) {
				case HASH: vni::TypePool<Value>::destroy(obj);
			}
		}
	}
	
	const char* vni_type_name() const {
		return "test.Value";
	}
	
	virtual void serialize(TypeOutput& out) {
		out.putEntry(VNL_IO_TYPE, 3);
		out.putHash(HASH);
		out.putHash(0x4354534);
		out.putInt(x);
		out.putHash(0x4345534);
		out.putInt(y);
		out.putHash(0x4356544);
		out.putInt(z);
	}
	
	virtual void deserialize(TypeInput& in, uint32_t num_entries) {
		uint32_t hash = 0;
		uint32_t size = 0;
		for(int i = 0; i < num_entries; ++i) {
			in.getHash(hash);
			int id = in.getEntry(size);
			switch(hash) {
				case 0x4354534: in.getInt(x, size); break;
				case 0x4345534: in.getInt(y, size); break;
				case 0x4356544: in.getInt(z, size); break;
				default: in.skip(id, size);
			}
		}
	}
	
};


}

#endif /* CPP_GENERATED_TEST_VALUE_H_ */
