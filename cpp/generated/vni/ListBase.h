/*
 * List.h
 *
 *  Created on: May 22, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_VNI_LISTBASE_H_
#define CPP_GENERATED_VNI_LISTBASE_H_

#include <vni/Interface.h>


namespace vni {

template<typename T>
class ListBase : public vni::Interface {
public:
	virtual void push_back(T* elem) = 0;
	virtual void clear() = 0;
	virtual int32_t test(int32_t val) const = 0;
	
	static const uint32_t HASH = 0x16233789;
	
	ListBase() {
		vni_hash_ = HASH;
	}
	
	virtual const char* vni_type_name() const {
		return "vni.List";
	}
	
	class Client : public vni::Client {
	public:
		void push_back(T* elem) {
			vni::Client::buf.wrap(vni::Client::data);
			Writer _wr(vni::Client::out);
			_wr.push_back(elem);
			vnl::Packet* _pkt = vni::Client::call();
			if(_pkt) {
				_pkt->ack();
			}
		}
		void clear() {
			// same
		}
		int32_t test(int32_t val) {
			vni::Client::buf.wrap(vni::Client::data);
			Writer _wr(vni::Client::out);
			_wr.test(val);
			vnl::Packet* _pkt = vni::Client::call();
			int32_t _res = 0;
			if(_pkt) {
				vni::Client::in.getInteger(_res);
				_pkt->ack();
			}
			return _res;
		}
	};
	
protected:
	class Writer {
	public:
		Writer(vnl::io::TypeOutput& out) : _out(out) {
			_out.putEntry(VNL_IO_INTERFACE, VNL_IO_BEGIN);
			_out.putHash(HASH);
		}
		~Writer() {
			_out.putEntry(VNL_IO_INTERFACE, VNL_IO_END);
		}
		void push_back(T* elem) {
			_out.putEntry(VNL_IO_CALL, 1);
			_out.putHash(0x1337);
			elem->serialize(_out);
		}
		void clear() {
			_out.putEntry(VNL_IO_CALL, 0);
			_out.putHash(0x1338);
		}
		void test(int32_t val) {
			_out.putEntry(VNL_IO_CALL, 1);
			_out.putHash(0x1339);
			_out.putInt(val);
		}
	protected:
		vnl::io::TypeOutput& _out;
	};
	
	virtual bool vni_call(vnl::io::TypeInput& in, uint32_t hash, int num_args) {
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
	
	virtual bool vni_const_call(vnl::io::TypeInput& in, uint32_t hash, int num_args, vnl::io::TypeOutput& out) {
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
