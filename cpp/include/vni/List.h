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
	
	virtual void push_back(T* elem) {
		vnl::List<T*>::push_back(elem);
	}
	
	virtual void clear() {
		vnl::List<T*>::clear();
	}
	
	virtual int32_t test(int32_t val) const {
		return val*1337;
	}
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		Writer wr(out);
		for(T* obj : list) {
			wr.push_back(obj);
		}
	}
	
};


}

#endif /* CPP_VNI_LIST_H_ */
