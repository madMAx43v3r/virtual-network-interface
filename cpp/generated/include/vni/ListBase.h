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

template<typename T>
class List;

template<typename T>
class ListBase : public vni::Interface {
public:
	static const uint32_t VNI_HASH = 0x16233789;
	
	ListBase() {
		vni_hash_ = VNI_HASH;
	}
	
	~ListBase() {
	}
	
	static List<T>* create() {
		return vni::GlobalPool<vni::List<T> >::create();
	}
	
	static List<T>* create(vnl::Hash32 hash) {
		switch(hash) {
			case vni::Value::VNI_HASH: return vni::GlobalPool<vni::List<T> >::create();
		}
		return 0;
	}
	
	virtual void destroy() {
		vni::GlobalPool<vni::Value>::destroy(this);
	}
	
	static bool is_base(vnl::Hash32 hash) {
		switch(hash) {
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
		return "vni.List";
	}
	
	class Client : public vni::Client {
	public:
	};
	
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
