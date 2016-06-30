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
#include <test/value_t.h>


namespace test {

class TestType : public vni::Type {
public:
	Value* val = 0;
	value_t val2;
	
	static const uint32_t HASH = 0x5df232ab;
	
	VNI_SAMPLE(TestType);
	
	TestType() {
		vni_hash_ = HASH;
	}
	
	~TestType() {
		Value::destroy(val);
	}
	
	virtual void serialize(vnl::io::TypeOutputStream& out) const {
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
	
	virtual void deserialize(vnl::io::TypeInputStream& in, uint32_t num_entries) {
		for(int i = 0; i < num_entries && !in.error(); ++i) {
			uint32_t hash = 0;
			in.getHash(hash);
			switch(hash) {
				case 0x4786877: val = Value::read(in); break;
				case 0x7246790: value_t::read(in, &val2); break;
				default: in.skip();
			}
		}
	}
	
};


}

#endif /* CPP_GENERATED_TEST_TESTTYPE_H_ */
