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
			case VNI_HASH: return vni::Pool<TestValue>::create();
			default: return 0;
		}
	}
	
	static void destroy(TestValue* obj) {
		if(obj) {
			switch(obj->vni_hash_) {
				case VNI_HASH: vni::Pool<TestValue>::destroy(obj);
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
		vni::write(out, x);
		out.putHash(0x4345534);
		vni::write(out, y);
		out.putHash(0x4356544);
		vni::write(out, z);
	}
	
	virtual void deserialize(vnl::io::TypeInput& in, int size) {
		// see TestType.h
	}
	
};


}

#endif /* CPP_GENERATED_TEST_VALUE_H_ */
