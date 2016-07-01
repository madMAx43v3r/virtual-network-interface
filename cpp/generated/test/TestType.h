/*
 * Value.h
 *
 *  Created on: May 22, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_TEST_TESTTYPE_H_
#define CPP_GENERATED_TEST_TESTTYPE_H_

#include <vni/Class.h>
#include <test/Value.h>
#include <test/value_t.h>


namespace test {

class TestType : public vni::Class {
public:
	Value* val = 0;
	value_t val2;
	
	static const uint32_t HASH = 0x5df232ab;
	
	TestType() {
		vni_hash_ = HASH;
	}
	
	~TestType() {
		Value::destroy(val);
	}
	
	static TestType* create() {
		return create(HASH);
	}
	
	static TestType* create(uint32_t hash) {
		switch(hash) {
			case HASH: return vni::TypePool<TestType>::create();
			default: return 0;
		}
	}
	
	static void destroy(TestType* obj) {
		if(obj) {
			switch(obj->vni_hash) {
				case HASH: vni::TypePool<TestType>::destroy(obj);
			}
		}
	}
	
	virtual const char* vni_type_name() const {
		return "test.TestType";
	}
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		out.putEntry(VNL_IO_CLASS, 2);
		out.putHash(HASH);
		out.putHash(0x4786877);
		if(val) {
			val->serialize(out);
		} else {
			out.putNull();
		}
		out.putHash(0x7246790);
		val2.serialize(out);
	}
	
	virtual void deserialize(vnl::io::TypeInput& in, int size) {
		for(int i = 0; i < size && !in.error(); ++i) {
			uint32_t hash = 0;
			in.getHash(hash);
			switch(hash) {
				case 0x4786877: val = vni::Class::read<Value>(in); break;
				case 0x7246790: vni::Struct::read<value_t>(in, &val2); break;
				default: in.skip();
			}
		}
	}
	
};


}

#endif /* CPP_GENERATED_TEST_TESTTYPE_H_ */
