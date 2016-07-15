/*
 * List.h
 *
 *  Created on: May 22, 2016
 *      Author: mad
 */

#ifndef CPP_VNI_LIST_H_
#define CPP_VNI_LIST_H_

#include <vni/ListBase.hxx>
#include <vnl/List.h>


namespace vni {

template<typename T>
class List : public vni::ListBase<T>, public vnl::List<T> {
public:
	virtual ~List() {}
	
	List& operator=(const vnl::List<T>& other) {
		vnl::List<T>::clear();
		append(other);
		return *this;
	}
	
	virtual void to_string(vnl::String& str) const {
		// TODO
	}
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		typename vni::ListBase<T>::Writer wr(out);
		out.putEntry(VNL_IO_ARRAY, vnl::List<T>::size());
		for(typename vnl::List<T>::const_iterator iter = vnl::List<T>::begin();
				iter != vnl::List<T>::end() && !out.error(); ++iter)
		{
			vni::write(out, *iter);
		}
	}
	
	virtual void deserialize(vnl::io::TypeInput& in, int size) {
		int id = in.getEntry(size);
		if(id == VNL_IO_ARRAY) {
			for(int i = 0; i < size && !in.error(); ++i) {
				vni::read(in, vnl::List<T>::push_back(T()));
			}
		} else {
			in.skip(id, size);
		}
		in.skip(VNL_IO_INTERFACE, 0, vni::ListBase<T>::VNI_HASH);
	}
	
};


}

#endif /* CPP_VNI_LIST_H_ */
