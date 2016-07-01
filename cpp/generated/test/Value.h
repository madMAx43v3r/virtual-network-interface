/*
 * Value.h
 *
 *  Created on: May 22, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_TEST_VALUE_H_
#define CPP_GENERATED_TEST_VALUE_H_

#include <vni/Class.h>


namespace test {

class Value : public vni::Class {
public:
	int32_t x;
	int32_t y;
	int32_t z;
	
	static const uint32_t HASH = 0x51212323;
	
	Value() {
		vni_hash_ = HASH;
		x = 0;
		y = 0;
		z = 0;
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
	
	static void read(vnl::io::TypeInput& in, Value* obj) {
		vni::Class::read<Value>(in, obj);
	}
	
	static Value* read(vnl::io::TypeInput& in) {
		return vni::Class::read<Value>(in);
	}
	
	virtual const char* vni_type_name() const {
		return "test.Value";
	}
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		out.putEntry(VNL_IO_CLASS, 3);
		out.putHash(HASH);
		out.putHash(0x4354534);
		out.putInt(x);
		out.putHash(0x4345534);
		out.putInt(y);
		out.putHash(0x4356544);
		out.putInt(z);
	}
	
	virtual void deserialize(vnl::io::TypeInput& in, uint32_t size) {
		for(int i = 0; i < size && !in.error(); ++i) {
			uint32_t hash = 0;
			in.getHash(hash);
			switch(hash) {
				case 0x4354534: in.getInteger(x); break;
				case 0x4345534: in.getInteger(y); break;
				case 0x4356544: in.getInteger(z); break;
				default: in.skip();
			}
		}
	}
	
};


}

#endif /* CPP_GENERATED_TEST_VALUE_H_ */
