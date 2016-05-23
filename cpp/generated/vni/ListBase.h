/*
 * List.h
 *
 *  Created on: May 22, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_VNI_LISTBASE_H_
#define CPP_GENERATED_VNI_LISTBASE_H_

#include <vnl/io.h>
#include <vnl/List.h>


namespace vni {

template<typename T>
class ListBase : public vnl::List<T>, public vnl::io::Serializable {
public:
	virtual ~ListBase() {}
	
	virtual bool deserialize(vnl::io::TypeInput<vnl::io::PageBuffer>& stream) {
		
	}
	
protected:
	class Writer {
	public:
		Writer(vnl::io::TypeOutput<vnl::io::PageBuffer>& stream) : stream(stream) {}
		void push_back(T* elem) {
			stream.putFunc(0x1337, 1);
			stream.putType(0x1234);
			elem->serialize(stream, tmp);
		}
		void clear() {
			stream.putFunc(0x1338, 0);
		}
	private:
		vnl::io::TypeOutput<vnl::io::PageBuffer>& stream;
	};
	
};


}

#endif /* CPP_GENERATED_VNI_LISTBASE_H_ */
