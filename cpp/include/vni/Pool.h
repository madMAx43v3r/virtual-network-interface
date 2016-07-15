/*
 * GlobalPool.h
 *
 *  Created on: Jul 6, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_GLOBALPOOL_H_
#define INCLUDE_VNI_GLOBALPOOL_H_

#include <vnl/Pool.h>
#include <vnl/util/spinlock.h>


namespace vni {

template<typename T>
class Pool {
public:
	static T* create() {
		sync.lock();
		T* obj = pool.create();
		sync.unlock();
		return obj;
	}
	
	static void destroy(T* obj) {
		if(obj) {
			assert(obj->vni_hash() == T::VNI_HASH);
			sync.lock();
			pool.destroy(obj);
			sync.unlock();
		}
	}
	
private:
	static vnl::util::spinlock sync;
	static vnl::Pool<T> pool;
};

template<typename T> vnl::util::spinlock Pool<T>::sync;
template<typename T> vnl::Pool<T> Pool<T>::pool;


template<typename T>
T* create() {
	return vni::Pool<T>::create();
}

template<typename T>
T* clone(const T& other) {
	T* obj = vni::Pool<T>::create();
	*obj = other;
	return obj;
}

template<typename T>
void destroy(T* obj) {
	if(obj) {
		obj->destroy();
	}
}


}

#endif /* INCLUDE_VNI_GLOBALPOOL_H_ */
