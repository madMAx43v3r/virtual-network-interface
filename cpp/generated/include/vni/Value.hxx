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

class Value : public vni::Type {
public:
	static const uint32_t VNI_HASH = 0xb43cb5b8;
	static const uint32_t NUM_FIELDS = 0;
	
	Value() {
	}
	
	~Value() {
	}
	
	static Value* create() { return vni::create<vni::Value>(); }
	
	virtual Value* clone() const { return vni::create<vni::Value>(*this); }
	virtual void destroy() { vni::destroy<vni::Value>(this); }
	
	virtual uint32_t vni_hash() const { return VNI_HASH; }
	virtual const char* type_name() const { return "vni.Value"; }
	
	virtual int num_fields() const { return NUM_FIELDS; }
	virtual int field_index(vnl::Hash32 hash) const;
	virtual const char* field_name(int index) const;
	virtual vni::Value* field_type(int index) const;
	
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
