/*
 * GlobalPool.h
 *
 *  Created on: Jul 6, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_GLOBALPOOL_H_
#define INCLUDE_VNI_GLOBALPOOL_H_

#include <vnl/Pool.h>


namespace vni {


template<typename T>
T* create() {
	return vnl::global_pool->create<T>();
}

template<typename T>
T* clone(const T& other) {
	return new(vnl::global_pool->alloc(sizeof(T))) T(other);
}

template<typename T>
void destroy(T* obj) {
	if(obj) {
		obj->destroy();
	}
}


}

#endif /* INCLUDE_VNI_GLOBALPOOL_H_ */
