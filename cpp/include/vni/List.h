/*
 * List.h
 *
 *  Created on: May 22, 2016
 *      Author: mad
 */

#ifndef CPP_VNI_LIST_H_
#define CPP_VNI_LIST_H_

#include <vni/ListBase.h>
#include <vnl/List.h>


namespace vni {

template<typename T>
class List : public vni::ListBase<T>, public vnl::List<T*> {
public:
	List() {}
	
	List(const List& other) {
		*this = other;
	}
	
	~List() {
		clear();
	}
	
	List& operator=(const List& other) {
		clear();
		// TODO
	}
	
	void clear() {
		for(vnl::List<T*>::const_iterator iter = begin(); iter != end(); ++iter) {
			T::destroy(*iter);
		}
	}
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		Writer wr(out);
		for(vnl::List<T*>::const_iterator iter = begin(); iter != end(); ++iter) {
			out.putEntry(VNL_IO_CALL, 1);
			out.putHash(call_push_back);
			iter->serialize(out);
		}
	}
	
protected:
	virtual bool vni_call(vnl::io::TypeInput& _in, uint32_t _hash, int _num_args) {
		switch(_hash) {
		case call_push_back:
			T* obj = vni::read<T>(_in);
			if(obj) {
				push_back(obj);
			}
			break;
		}
		return false;
	}
	
private:
	static const uint32_t call_push_back = 0xbfe7101c;
	
};


}

#endif /* CPP_VNI_LIST_H_ */
