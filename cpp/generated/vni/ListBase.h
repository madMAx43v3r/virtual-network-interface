/*
 * List.h
 *
 *  Created on: May 22, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_VNI_LISTBASE_H_
#define CPP_GENERATED_VNI_LISTBASE_H_

#include <vni/Type.h>
#include <vni/Frame.h>
#include <vnl/Node.h>
#include <vnl/List.h>


namespace vni {

template<typename T>
class ListBase : public vnl::List<T*>, public vni::Interface {
protected:
	virtual void push_back(T* elem) = 0;
	virtual void clear() = 0;
	
public:
	static const uint32_t HASH = 0x16233789;
	
	virtual ~ListBase() {}
	
	virtual bool call(vnl::io::TypeInputStream& in, uint32_t hash, uint32_t num_args) {
		switch(hash) {
		case 0x1337:
			if(num_args == 1) {
				T* obj = T::read(in);
				push_back(obj);
				return true;
			}
			break;
		case 0x1338:
			if(num_args == 0) {
				clear();
				return true;
			}
			break;
		}
		return false;
	}
	
	virtual bool const_call(vnl::io::TypeInputStream& in, uint32_t hash, uint32_t num_args, vnl::io::TypeOutputStream& out) {
		switch(hash) {
		}
		return false;
	}
	
	class Client : public vni::Client {
	public:
		void push_back(T* elem) {
			vnl::io::PageBuffer buf(data);
			vnl::io::TypeOutputStream out(buf);
			Writer wr(out);
			wr.push_back(elem);
			Frame* res = call(buf, false);
			if(res) {
				// nothing
			}
		}
		void clear() {
			// same
		}
	};
	
	class Writer {
	public:
		Writer(vnl::io::TypeOutputStream& out) : out(out) {}
		void push_back(T* elem) {
			out.putEntry(VNL_IO_CALL, 1);
			out.putHash(0x1337);
			elem->serialize(out);
		}
		void clear() {
			out.putEntry(VNL_IO_CALL, 0);
			out.putHash(0x1338);
		}
	protected:
		vnl::io::TypeOutputStream& out;
	};
	
	class Serializer : public Writer {
	public:
		Serializer(vnl::io::TypeOutputStream& out) : Writer(out) {
			out.putEntry(VNL_IO_INTERFACE, VNL_IO_BEGIN);
			out.putHash(HASH);
		}
		~Serializer() {
			out.putEntry(VNL_IO_INTERFACE, VNL_IO_END);
		}
	};
	
};


}

#endif /* CPP_GENERATED_VNI_LISTBASE_H_ */
