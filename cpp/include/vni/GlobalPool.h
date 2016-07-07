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
class GlobalPool {
public:
	static T* create() {
		sync.lock();
		T* obj = pool.create();
		sync.unlock();
		return obj;
	}
	
	static void destroy(T* obj) {
		if(obj) {
			assert(obj->vni_hash() == T::HASH);
			sync.lock();
			pool.destroy(obj);
			sync.unlock();
		}
	}
	
private:
	static vnl::util::spinlock sync;
	static vnl::Pool<T> pool;
};

template<typename T> vnl::util::spinlock GlobalPool<T>::sync;
template<typename T> vnl::Pool<T> GlobalPool<T>::pool;


}

#endif /* INCLUDE_VNI_GLOBALPOOL_H_ */
