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
class List : public vni::ListBase<T>, public vnl::List<T> {
public:
	virtual ~List() {}
	
	List& operator=(const vnl::List<T>& other) {
		clear();
		append(other);
		return *this;
	}
	
	virtual void to_string(vnl::String& str) const {
		// TODO
	}
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		Writer wr(out);
		out.putEntry(VNL_IO_ARRAY, size());
		for(const_iterator iter = begin(); iter != end() && !out.error(); ++iter) {
			vni::write(out, *iter);
		}
	}
	
	virtual void deserialize(vnl::io::TypeInput& in, int size) {
		int id = in.getEntry(size);
		if(id == VNL_IO_ARRAY) {
			for(int i = 0; i < size && !in.error(); ++i) {
				vni::read(in, push_back(T()));
			}
		} else {
			in.skip(id, size);
		}
		in.skip(VNL_IO_INTERFACE, 0, VNI_HASH);
	}
	
};


}

#endif /* CPP_VNI_LIST_H_ */
