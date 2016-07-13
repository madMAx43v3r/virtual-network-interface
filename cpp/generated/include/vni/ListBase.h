/*
 * List.h
 *
 *  Created on: May 22, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_VNI_LISTBASE_H_
#define CPP_GENERATED_VNI_LISTBASE_H_

#include <vni/Interface.h>


namespace vni {

template<class T>
class ListBase : public vni::Interface {
public:
	static const uint32_t VNI_HASH = 0x16233789;
	
	ListBase() {
	}
	
	~ListBase() {
	}
	
	virtual const char* vni_type_name() const {
		return "vni.List";
	}
	
protected:
	class Writer {
	public:
		Writer(vnl::io::TypeOutput& out_) : _out(out_) {
			_out.putEntry(VNL_IO_INTERFACE, VNL_IO_BEGIN);
			_out.putHash(VNI_HASH);
		}
		~Writer() {
			_out.putEntry(VNL_IO_INTERFACE, VNL_IO_END);
		}
	protected:
		vnl::io::TypeOutput& _out;
	};
	
	virtual bool vni_call(vnl::io::TypeInput& _in, uint32_t _hash, int _num_args) {
		switch(_hash) {
		}
		return false;
	}
	
	virtual bool vni_const_call(vnl::io::TypeInput& _in, uint32_t _hash, int _num_args, vnl::io::TypeOutput& _out) {
		switch(_hash) {
		}
		return false;
	}
	
};


}

#endif /* CPP_GENERATED_VNI_LISTBASE_H_ */
