/*
 * Value.h
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_VALUE_H_
#define INCLUDE_VNI_VALUE_H_

#include <vni/Type.h>


namespace vni {

namespace reflect {

class Value {
public:
	static const char* type_name() { return "vni.Value"; }
	static bool is_base(vnl::Hash32 hash);
	static bool is_instance(vnl::Hash32 hash);
};

}


class Value : public vni::Type {
public:
	static const uint32_t VNI_HASH = 0xb43cb5b8;
	
	Value() {
		vni_hash_ = VNI_HASH;
	}
	
	~Value() {
	}
	
	static Value* create(vnl::Hash32 hash);
	static Value* create() { return vni::GlobalPool<vni::Value>::create(); }
	
	virtual void destroy() { vni::GlobalPool<vni::Value>::destroy(this); }
	
	virtual bool is_base(vnl::Hash32 hash) const { return reflect::Value::is_base(hash); }
	virtual bool is_instance(vnl::Hash32 hash) const { return reflect::Value::is_instance(hash); }
	
	virtual const char* type_name() const { return reflect::Value::type_name(); }
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		out.putEntry(VNL_IO_CLASS, 0);
		out.putHash(VNI_HASH);
	}
	
	virtual void deserialize(vnl::io::TypeInput& in, int size) {
		for(int i = 0; i < size && !in.error(); ++i) {
			uint32_t hash = 0;
			in.getHash(hash);
			switch(hash) {
				default: in.skip();
			}
		}
	}
	
	
};


}

#endif /* INCLUDE_VNI_VALUE_H_ */
