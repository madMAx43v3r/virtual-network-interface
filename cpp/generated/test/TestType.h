/*
 * Value.h
 *
 *  Created on: May 22, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_TEST_TESTTYPE_H_
#define CPP_GENERATED_TEST_TESTTYPE_H_

#include <vni/Type.h>
#include <test/Value.h>


namespace test {

class TestType : public vni::Type {
public:
	static const uint32_t HASH = 0x5df232ab;
	
	Value* val = 0;
	Value val2;
	
	TestType() {
		vni_hash_ = HASH;
	}
	
	~TestType() {
		Value::destroy(val);
	}
	
	virtual void serialize(vnl::io::TypeOutput<vnl::io::PageBuffer>& out) {
		out.putSize(-2);
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
	
	virtual void deserialize(vnl::io::TypeInput<vnl::io::PageBuffer>& in, int num_entries) {
		uint32_t hash = 0;
		int32_t size = 0;
		for(int i = 0; i < num_entries; ++i) {
			in.getHash(hash);
			in.getSize(size);
			switch(hash) {
				case 0x4786877:
					in.getHash(hash);
					val = Value::create(hash);
					if(val) {
						val->deserialize(in, -size);
					} else {
						in.skip(size);
					}
					break;
				case 0x7246790:
					in.getHash(hash);
					if(hash == Value::HASH) {
						val2.deserialize(in, -size);
					} else {
						in.skip(size);
					}
					break;
				default: in.skip(size);
			}
		}
	}
	
};


}

#endif /* CPP_GENERATED_TEST_TESTTYPE_H_ */
