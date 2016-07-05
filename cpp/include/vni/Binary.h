/*
 * Binary.h
 *
 *  Created on: Jul 4, 2016
 *      Author: mad
 */

#ifndef CPP_INCLUDE_VNI_BINARY_H_
#define CPP_INCLUDE_VNI_BINARY_H_

#include <vni/BinaryBase.h>
#include <vnl/Memory.h>


namespace vni {

class Binary : public BinaryBase {
public:
	vnl::Page* data;
	int size;
	
	Binary() {
		data = 0;
		size = 0;
	}
	
	Binary(const Binary& other) : Binary() {
		*this = other;
	}
	
	~Binary() {
		clear();
	}
	
	Binary& operator=(const Binary& other) {
		clear();
		data = other.data ? vnl::Page::alloc() : 0;
		size = other.size;
		vnl::Page* src = other.data;
		vnl::Page* dst = data;
		while(src) {
			memcpy(dst->mem, src->mem, vnl::Page::size);
			if(src->next) {
				dst->next = vnl::Page::alloc();
			}
			src = src->next;
			dst = dst->next;
		}
		return *this;
	}
	
	void clear() {
		if(data) {
			data->free_all();
		}
		size = 0;
	}
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		Writer wr(out);
		out.putBinary(data, size);
	}
	
	virtual void deserialize(vnl::io::TypeInput& in, int size) {
		if(!data) {
			data = vnl::Page::alloc();
		}
		in.getBinary(data, size);
		in.skip(VNL_IO_INTERFACE, 0);
	}
	
};


}


#endif /* CPP_INCLUDE_VNI_BINARY_H_ */
