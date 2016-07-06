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
	int32_t x;
	int32_t y;
	int32_t z;
	
	static const uint32_t VNI_HASH = 0x51212323;
	static const int VNI_NUM_FIELDS = 3;
	
	TestValue() {
		vni_hash_ = VNI_HASH;
		x = 0;
		y = 0;
		z = 0;
	}
	
	static TestValue* create() {
		return create(VNI_HASH);
	}
	
	static TestValue* create(uint32_t hash) {
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
	
	static void write_header(vnl::io::TypeOutput& out) {
		out.putHash(0x4354534);
		out.putHash(0x4345534);
		out.putHash(0x4356544);
	}
	
	void write_body(vnl::io::TypeOutput& out) const {
		out.putValue(x);
		out.putValue(y);
		out.putValue(z);
	}
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		out.putEntry(VNL_IO_CLASS, VNI_NUM_FIELDS);
		out.putHash(VNI_HASH);
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
				case 0x4354534: in.getValue(x); break;
				case 0x4345534: in.getValue(y); break;
				case 0x4356544: in.getValue(z); break;
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

#endif /* CPP_GENERATED_TEST_VALUE_H_ */
