/*
 * List.h
 *
 *  Created on: May 22, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_VNI_LISTBASE_H_
#define CPP_GENERATED_VNI_LISTBASE_H_

#include <vni/Type.h>
#include <vnl/List.h>


namespace vni {

template<typename T>
class ListBase : public vnl::List<T>, public vni::Type {
public:
	virtual ~ListBase() {}
	
	virtual void push_back(T* elem) = 0;
	virtual void clear() = 0;
	
	virtual bool deserialize(TypeInput& in) {
		
	}
	
protected:
	class Writer {
	public:
		Writer(TypeOutput& out) : out(out) {}
		void push_back(T* elem) {
			out.putFunc(0x1337, 1);
			out.putType(0x1234);
			elem->serialize(out, tmp);
		}
		void clear() {
			out.putFunc(0x1338, 0);
		}
	private:
		TypeOutput& out;
	};
	
};


}

#endif /* CPP_GENERATED_VNI_LISTBASE_H_ */
