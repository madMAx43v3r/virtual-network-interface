/*
 * Type.h
 *
 *  Created on: Jun 27, 2016
 *      Author: mad
 */

#ifndef CPP_VNI_TYPE_H_
#define CPP_VNI_TYPE_H_

#include <vnl/io.h>
#include <vnl/Packet.h>
#include <vnl/Pool.h>
#include <vnl/util/spinlock.h>

#include <vni/Sample.h>
#include <vni/Frame.h>


namespace vni {

class Type : public vnl::io::Serializable {
public:
	virtual ~Type() {}
	
	virtual const char* vni_type_name() const = 0;
	
	uint32_t vni_hash() const {
		return vni_hash_;
	}
	
protected:
	uint32_t vni_hash_;
	
};


template<typename T>
class TypePool {
public:
	static T* create() {
		sync.lock();
		T* obj = pool.create();
		sync.unlock();
		return obj;
	}
	
	static void destroy(T* obj) {
		assert(obj->vni_hash() == T::HASH);
		sync.lock();
		pool.destroy(obj);
		sync.unlock();
	}
	
private:
	static vnl::util::spinlock sync;
	static vnl::Pool<T> pool;
};

template<typename T> vnl::util::spinlock TypePool<T>::sync;
template<typename T> vnl::Pool<T> TypePool<T>::pool;


}

#endif /* CPP_VNI_TYPE_H_ */
