/*
 * Value.h
 *
 *  Created on: May 22, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_TEST_TESTTYPE_H_
#define CPP_GENERATED_TEST_TESTTYPE_H_

#include <vni/Value.h>
#include <test/TestValue.h>
#include <test/value_t.h>
#include <vni/List.h>


namespace test {

class TestType : public vni::Value {
public:
	static const uint32_t VNI_HASH = 0x5df232ab;
	
	TestValue* val = 0;
	value_t val2;
	
	vni::List<TestValue> list;
	vni::List<value_t> list2;
	
	TestType() {
		vni_hash_ = VNI_HASH;
	}
	
	~TestType() {
		vni::destroy(val);
	}
	
	static TestType* create() {
		return create(VNI_HASH);
	}
	
	static TestType* create(vnl::Hash32 hash) {
		switch(hash) {
			case test::TestType::VNI_HASH: return vni::Pool<TestType>::create();
		}
		return 0;
	}
	
	static void destroy(TestType* obj) {
		if(obj) {
			switch(obj->vni_hash_) {
				case test::TestType::VNI_HASH: vni::Pool<TestType>::destroy(obj);
			}
		}
	}
	
	static bool is_base(vnl::Hash32 hash) {
		switch(hash) {
			case vni::Value::VNI_HASH: return true;
		}
		return false;
	}
	
	static bool is_instance(vnl::Hash32 hash) {
		switch(hash) {
			case test::TestType::VNI_HASH: return true;
		}
		return false;
	}
	
	virtual bool vni_is_base(vnl::Hash32 hash) {
		return is_base(hash);
	}
	
	virtual bool vni_is_instance(vnl::Hash32 hash) {
		return is_instance(hash);
	}
	
	virtual const char* vni_type_name() const {
		return "test.TestType";
	}
	
	virtual void to_string(vnl::String& str) const {
		str << "{";
		str << "val:" << vni::to_string(val) << ",";
		
		str << "}";
	}
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		out.putEntry(VNL_IO_CLASS, 4);
		out.putHash(VNI_HASH);
		out.putHash(0x4786877);
		vni::write(out, val);
		out.putHash(0x7246790);
		vni::write(out, val2);
		out.putHash(0x3674473);
		vni::write(out, list);
		out.putHash(0x3674474);
		vni::write(out, list2);
	}
	
	virtual void deserialize(vnl::io::TypeInput& in, int size) {
		for(int i = 0; i < size && !in.error(); ++i) {
			uint32_t hash = 0;
			in.getHash(hash);
			switch(hash) {
				case 0x4786877: val = vni::read<test::TestValue>(in); break;
				case 0x7246790: vni::read(in, val2); break;
				case 0x3674473: vni::read(in, list); break;
				case 0x3674474: vni::read(in, list2); break;
				default: in.skip();
			}
		}
	}
	
};


}

#endif /* CPP_GENERATED_TEST_TESTTYPE_H_ */
