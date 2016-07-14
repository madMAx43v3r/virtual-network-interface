/*
 * ObjectBase.h
 *
 *  Created on: Jul 4, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_VNI_OBJECTBASE_H_
#define CPP_GENERATED_VNI_OBJECTBASE_H_

#include <vni/Binary.hxx>
#include "../../../include/vni/Interface.hxx"


namespace vni {

class ObjectBase : public vni::Interface {
public:
	static const uint32_t HASH = 0x16238449;
	
	ObjectBase() {
		vni_hash_ = HASH;
	}
	
	virtual const char* vni_type_name() const {
		return "vni.Object";
	}
	
protected:
	virtual vni::Binary vni_serialize() const = 0;
	virtual void vni_deserialize(vni::Binary& blob) = 0;
	
protected:
	class Writer {
	public:
		Writer(vnl::io::TypeOutput& out) : _out(out) {
			_out.putEntry(VNL_IO_INTERFACE, VNL_IO_BEGIN);
			_out.putHash(HASH);
		}
		~Writer() {
			_out.putEntry(VNL_IO_INTERFACE, VNL_IO_END);
		}
		void vni_serialize() {
			
		}
		void vni_deserialize(vni::Binary& blob) {
			
		}
	protected:
		vnl::io::TypeOutput& _out;
	};
	
	virtual bool vni_call(vnl::io::TypeInput& in, uint32_t hash, int num_args) {
		switch(hash) {
		}
		return false;
	}
	
	virtual bool vni_const_call(vnl::io::TypeInput& in, uint32_t hash, int num_args, vnl::io::TypeOutput& out) {
		switch(hash) {
		}
		return false;
	}
	
};


class ObjectClient : public vni::Client {
public:
	int vni_serialize(vni::Binary& _result) {
		
	}
	int vni_deserialize(const vni::Binary& blob) {
		
	}
};



}

#endif /* CPP_GENERATED_VNI_OBJECTBASE_H_ */
