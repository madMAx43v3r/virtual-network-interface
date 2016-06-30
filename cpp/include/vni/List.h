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
class List : public vni::ListBase<T> {
public:
	
	virtual void push_back(T* elem) {
		
	}
	
	virtual void clear() {
		
	}
	
private:
	vnl::List<T*> list;
	
};


}

#endif /* CPP_VNI_LIST_H_ */
