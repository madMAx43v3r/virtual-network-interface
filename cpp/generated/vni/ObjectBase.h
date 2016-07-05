/*
 * ObjectBase.h
 *
 *  Created on: Jul 4, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_VNI_OBJECTBASE_H_
#define CPP_GENERATED_VNI_OBJECTBASE_H_

#include <vni/Interface.h>
#include <vni/Binary.h>


namespace vni {

class ObjectBase : public vni::Interface {
public:
	virtual vni::Binary vni_serialize() const = 0;
	virtual void vni_deserialize(vni::Binary& blob) = 0;
	
	static const uint32_t HASH = 0x16238449;
	
	ObjectBase() {
		vni_hash_ = HASH;
	}
	
	virtual const char* vni_type_name() const {
		return "vni.Object";
	}
	
	class Client : public vni::Client {
	public:
		vni::Binary vni_serialize() {
			
		}
		void vni_deserialize(vni::Binary& blob) {
			
		}
	};
	
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
		vni::Binary vni_serialize() {
			
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


}

#endif /* CPP_GENERATED_VNI_OBJECTBASE_H_ */
