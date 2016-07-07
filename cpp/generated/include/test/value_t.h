/*
 * Value.h
 *
 *  Created on: May 22, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_TEST_VALUE_T_H_
#define CPP_GENERATED_TEST_VALUE_T_H_

#include <vni/Value.h>


namespace test {

namespace reflect {

class value_t {
public:
	static const char* type_name() { return "test.value_t"; }
};

}

class value_t : public vni::Value {
public:
	static const uint32_t VNI_HASH = 0x51677428;
	
	vnl::Vector<float, 3> pos;
	
	value_t() {
		vni_hash_ = VNI_HASH;
		memset(pos, 0, sizeof(pos));
	}
	
	static value_t* create() { return vni::Pool<value_t>::create(); }
	
	virtual void destroy() { vni::Pool<value_t>::destroy(this); }
	
	virtual const char* type_name() const { return reflect::value_t::type_name(); }
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		out.putEntry(VNL_IO_STRUCT, 1);
		out.putHash(0x8356785);
		out.putArray(pos);
	}
	
	virtual void deserialize(vnl::io::TypeInput& in, int size) {
		// see TestType.h
	}
	
};


}

#endif /* CPP_GENERATED_TEST_VALUE_T_H_ */
