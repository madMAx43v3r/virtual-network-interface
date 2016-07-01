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


namespace vni {

template<typename T>
class ListBase : public vni::Interface {
public:
	static const uint32_t HASH = 0x16233789;
	
	virtual void push_back(T* elem) = 0;
	virtual void clear() = 0;
	virtual int32_t test(int32_t val) const = 0;
	
	class Client : public vni::Client {
	public:
		void push_back(T* elem) {
			buf.wrap(data);
			Writer wr(out);
			wr.push_back(elem);
			vnl::Packet* pkt = call(false);
			if(pkt) {
				pkt->ack();
			}
		}
		void clear() {
			// same
		}
		int32_t test(int32_t val) {
			buf.wrap(data);
			Writer wr(out);
			wr.test(val);
			vnl::Packet* pkt = call(true);
			int32_t res = 0;
			if(pkt) {
				in.getInteger(res);
				pkt->ack();
			}
			return res;
		}
	};
	
protected:
	class Writer {
	public:
		Writer(vnl::io::TypeOutput& out) : out(out) {}
		void push_back(T* elem) {
			out.putEntry(VNL_IO_CALL, 1);
			out.putHash(0x1337);
			elem->serialize(out);
		}
		void clear() {
			out.putEntry(VNL_IO_CALL, 0);
			out.putHash(0x1338);
		}
		void test(int32_t val) {
			out.putEntry(VNL_IO_CALL, 1);
			out.putHash(0x1339);
			out.putInt(val);
		}
	protected:
		vnl::io::TypeOutput& out;
	};
	
	class Serializer : public Writer {
	public:
		Serializer(vnl::io::TypeOutput& out) : Writer(out) {
			out.putEntry(VNL_IO_INTERFACE, VNL_IO_BEGIN);
			out.putHash(HASH);
		}
		~Serializer() {
			out.putEntry(VNL_IO_INTERFACE, VNL_IO_END);
		}
	};
	
	virtual bool call(vnl::io::TypeInput& in, uint32_t hash, int num_args) {
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
	
	virtual bool const_call(vnl::io::TypeInput& in, uint32_t hash, int num_args, vnl::io::TypeOutput& out) {
		switch(hash) {
		case 0x1339:
			if(num_args == 1) {
				int32_t val = 0;
				in.getInteger(val);
				int32_t res = test(val);
				out.putInt(res);
				return true;
			}
			break;
		}
		return false;
	}
	
};


}

#endif /* CPP_GENERATED_VNI_LISTBASE_H_ */
