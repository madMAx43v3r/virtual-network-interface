/*
 * Array.h
 *
 *  Created on: Jul 5, 2016
 *      Author: mad
 */

#ifndef CPP_INCLUDE_VNI_ARRAY_H_
#define CPP_INCLUDE_VNI_ARRAY_H_

#include <vni/ArrayBase.h>
#include <vnl/Array.h>


namespace vni {

template<typename T>
class Array : public vni::ArrayBase<T>, public vnl::Array<T> {
public:
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		Writer wr(out);
		out.putEntry(VNL_IO_ARRAY, size());
		out.putEntry(VNL_IO_STRUCT, T::VNI_NUM_FIELDS);
		for(vnl::Array<T>::const_iterator iter = begin(); iter != end(); ++iter) {
			iter->serialize_body(out);
		}
	}
	
	virtual void deserialize(vnl::io::TypeInput& in, int size) {
		int id = in.getEntry(size);
		if(id == VNL_IO_ARRAY) {
			int num = size;
			id = in.getEntry(size);
			if(id == VNL_IO_STRUCT) {
				for(int i = 0; i < num && !in.error(); ++i) {
					T elem;
					elem.deserialize(in, size);
					push_back(elem);
				}
			} else {
				for(int i = 0; i < num && !in.error(); ++i) {
					in.skip(id, size);
				}
			}
		} else {
			in.skip(id, size);
		}
		in.skip(VNL_IO_INTERFACE, 0);
	}
	
};


}

#endif /* CPP_INCLUDE_VNI_ARRAY_H_ */
