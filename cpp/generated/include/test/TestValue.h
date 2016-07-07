/*
 * Value.h
 *
 *  Created on: May 22, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_TEST_VALUE_H_
#define CPP_GENERATED_TEST_VALUE_H_

#include <vni/Value.h>


namespace test {

class TestValue : public vni::Value {
public:
	static const uint32_t VNI_HASH = 0x51212323;
	
	int32_t x;
	int32_t y;
	int32_t z;
	
	TestValue() {
		vni_hash_ = VNI_HASH;
		x = 0;
		y = 0;
		z = 0;
	}
	
	static TestValue* create() {
		return create(VNI_HASH);
	}
	
	static TestValue* create(vnl::Hash32 hash) {
		switch(hash) {
			case VNI_HASH: return vni::GlobalPool<TestValue>::create();
			default: return 0;
		}
	}
	
	static void destroy(TestValue* obj) {
		if(obj) {
			switch(obj->vni_hash_) {
				case VNI_HASH: vni::GlobalPool<TestValue>::destroy(obj);
			}
		}
	}
	
	virtual const char* vni_type_name() const {
		return "test.Value";
	}
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		out.putEntry(VNL_IO_CLASS, 3);
		out.putHash(VNI_HASH);
		out.putHash(0x4354534);
		out.putValue(x);
		out.putHash(0x4345534);
		out.putValue(y);
		out.putHash(0x4356544);
		out.putValue(z);
	}
	
	virtual void deserialize(vnl::io::TypeInput& in, int size) {
		// see TestType.h
	}
	
};


}

#endif /* CPP_GENERATED_TEST_VALUE_H_ */
