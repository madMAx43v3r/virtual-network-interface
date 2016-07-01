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
class List : public vni::ListBase<T> {
public:
	
	virtual void push_back(T* elem) {
		list.push_back(elem);
	}
	
	virtual void clear() {
		list.clear();
	}
	
	virtual int32_t test(int32_t val) const {
		return val*1337;
	}
	
	T& front() {
		return list.front();
	}
	T& back() {
		return list.back();
	}
	
	const T& front() const {
		return list.front();
	}
	const T& back() const {
		return list.back();
	}
	
	size_t size() const {
		return list.size();
	}
	
	bool empty() const {
		return list.empty();
	}
	
	typedef vnl::List<T*>::iterator iterator;
	typedef vnl::List<T*>::const_iterator const_iterator;
	
	iterator begin() { return list.begin(); }
	const_iterator begin() const { return list.begin(); }
	const_iterator cbegin() const { return list.cbegin(); }
	
	iterator end() { return list.end(); }
	const_iterator end() const { return list.end(); }
	const_iterator cend() const { return list.cend(); }
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		Writer wr(out);
		for(T* obj : list) {
			wr.push_back(obj);
		}
	}
	
private:
	vnl::List<T*> list;
	
};


}

#endif /* CPP_VNI_LIST_H_ */
