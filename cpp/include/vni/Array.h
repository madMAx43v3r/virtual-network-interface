/*
 * Array.h
 *
 *  Created on: Jul 5, 2016
 *      Author: mad
 */

#ifndef CPP_INCLUDE_VNI_ARRAY_H_
#define CPP_INCLUDE_VNI_ARRAY_H_

#include <vni/ArraySupport.hxx>
#include <vnl/Array.h>


namespace vni {

template<typename T>
class Array : public vni::ArrayBase<T>, public vnl::Array<T> {
public:
	virtual ~Array() {}
	
	Array& operator=(const vnl::Array<T>& other) {
		vnl::Array<T>::clear();
		vnl::Array<T>::append(other);
		return *this;
	}
	
	virtual void to_string(vnl::String& str) const {
		// TODO
	}
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		typename vnl::Array<T>::Writer wr(out);
		out.putEntry(VNL_IO_ARRAY, vnl::Array<T>::size());
		for(typename vnl::Array<T>::const_iterator iter = vnl::Array<T>::begin();
				iter != vnl::Array<T>::end() && !out.error(); ++iter)
		{
			vni::write(out, *iter);
		}
	}
	
	virtual void deserialize(vnl::io::TypeInput& in, int size) {
		int id = in.getEntry(size);
		if(id == VNL_IO_ARRAY) {
			for(int i = 0; i < size && !in.error(); ++i) {
				vni::read(in, push_back());
			}
		} else {
			in.skip(id, size);
		}
		in.skip(VNL_IO_INTERFACE, 0, vni::ArrayBase<T>::VNI_HASH);
	}
	
};




}

#endif /* CPP_INCLUDE_VNI_ARRAY_H_ */
