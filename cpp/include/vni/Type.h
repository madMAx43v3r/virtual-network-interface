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


#define VNI_SAMPLE(type) typedef vnl::PacketType<type, vni::PID_SAMPLE> sample_t;


namespace vni {

static const uint32_t PID_SAMPLE = 0xa37b95eb;


class Type : public vnl::io::Serializable {
public:
	virtual ~Type() {}
	
	uint32_t vni_hash() const {
		return vni_hash_;
	}
	
	virtual const char* vni_type_name() const = 0;
	
protected:
	uint32_t vni_hash_;
	
};


class Interface : public Type {
public:
	virtual void deserialize(vnl::io::TypeInputStream& in, uint32_t num_entries) {
		while(!in.error()) {
			uint32_t hash = 0;
			uint32_t size = 0;
			int id = in.getEntry(size);
			if(id == VNL_IO_CALL) {
				in.getHash(hash);
				if(!call(in, hash, size)) {
					for(uint32_t i = 0; i < size; ++i) {
						in.skip();
					}
				}
			} else if(id == VNL_IO_INTERFACE && size == VNL_IO_END) {
				break;
			} else {
				in.skip(id, size);
			}
		}
	}
	
protected:
	virtual bool call(vnl::io::TypeInputStream& in, uint32_t hash, uint32_t num_args) = 0;
	virtual bool const_call(vnl::io::TypeInputStream& in, uint32_t hash, uint32_t num_args, vnl::io::TypeOutputStream& out) = 0;
	
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
		assert(obj->vni_hash == T::HASH);
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
