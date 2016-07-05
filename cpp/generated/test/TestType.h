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
#include <vni/List.h>


namespace test {

class TestType : public vni::Class {
public:
	Value* val = 0;
	value_t val2;
	
	vni::List<Value> list;
	vni::List<value_t> list2;
	
	static const uint32_t VNI_HASH = 0x5df232ab;
	static const int VNI_NUM_FIELDS = 4;
	
	TestType() {
		vni_hash_ = VNI_HASH;
	}
	
	~TestType() {
		Value::destroy(test::TestType::val);
	}
	
	static TestType* create() {
		return create(VNI_HASH);
	}
	
	static TestType* create(uint32_t hash) {
		switch(hash) {
			case test::TestType::VNI_HASH: return vni::TypePool<TestType>::create();
			default: return 0;
		}
	}
	
	static void destroy(TestType* obj) {
		if(obj) {
			switch(obj->vni_hash_) {
				case test::TestType::VNI_HASH: vni::TypePool<TestType>::destroy(obj);
			}
		}
	}
	
	static void read(vnl::io::TypeInput& in, TestType* obj) {
		vni::Class::read<TestType>(in, obj);
	}
	
	static TestType* read(vnl::io::TypeInput& in) {
		return vni::Class::read<TestType>(in);
	}
	
	virtual const char* vni_type_name() const {
		return "test.TestType";
	}
	
	void serialize_body(vnl::io::TypeOutput& out) const {
		out.putHash(0x4786877);
		if(test::TestType::val) {
			test::TestType::val->serialize(out);
		} else {
			out.putNull();
		}
		out.putHash(0x7246790);
		test::TestType::val2.serialize(out);
		out.putHash(0x3674473);
		test::TestType::list.serialize(out);
		out.putHash(0x3674474);
		test::TestType::list2.serialize(out);
	}
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		out.putEntry(VNL_IO_CLASS, 3);
		out.putHash(VNI_HASH);
		serialize_body(out);
	}
	
	virtual void deserialize(vnl::io::TypeInput& in, int size) {
		for(int i = 0; i < size && !in.error(); ++i) {
			uint32_t hash = 0;
			in.getHash(hash);
			switch(hash) {
				case 0x4786877: test::TestType::val = test::Value::read(in); break;
				case 0x7246790: test::value_t::read(in, &test::TestType::val2); break;
				case 0x3674473: vni::List<Value>::read(in, &test::TestType::list); break;
				case 0x3674474: vni::List<value_t>::read(in, &test::TestType::list2); break;
				default: in.skip();
			}
		}
	}
	
};


}

#endif /* CPP_GENERATED_TEST_TESTTYPE_H_ */
