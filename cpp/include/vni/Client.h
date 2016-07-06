/*
 * Client.h
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_CLIENT_H_
#define INCLUDE_VNI_CLIENT_H_

#include <vni/ClientBase.h>
#include <vni/Frame.h>
#include <vnl/Stream.h>
#include <vnl/Router.h>


namespace vni {

class Client : public ClientBase, public vnl::Stream {
public:
	Client()
		:	_error(0), _in(&_buf), _out(&_buf)
	{
		_data = vnl::Page::alloc();
	}
	
	~Client() {
		if(is_connected) {
			Stream::close(dst);
		}
		_data->free_all();
	}
	
	virtual void vni_set_address(int64_t A, int64_t B) {
		vni_set_address(vnl::Address(A, B));
	}
	
	void vni_set_address(vnl::Address dst_) {
		dst = dst_;
	}
	
	void vni_connect(vnl::Engine* engine) {
		if(is_connected) {
			Stream::close(dst);
		}
		Stream::connect(engine);
		Stream::open(dst);
		is_connected = true;
	}
	
	void vni_set_timeout(int64_t timeout_ms) {
		timeout = timeout_ms;
	}
	
	void vni_set_fail(bool fail_if_timeout) {
		do_fail_if_timeout = fail_if_timeout;
	}
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		Writer _wr(out);
		_wr.vni_set_address(dst.A, dst.B);
	}
	
protected:
	int _error;
	
	vnl::Packet* _call() {
		_out.flush();
		next_seq++;
		int64_t ts_begin = vnl::currentTimeMillis();
		Frame* ret = 0;
		while(true) {
			Frame frame;
			frame.data = _data;
			frame.size = _buf.position();
			frame.seq = next_seq;
			send(&frame, dst);
			frame.data = 0;
			// TODO
			int64_t ts_now = vnl::currentTimeMillis();
			// TODO
		}
		if(ret) {
			_buf.wrap(ret->data, ret->size);
			_error = VNI_SUCCESS;
		} else {
			_error = VNI_ERROR;
		}
		return ret;
	}
	
private:
	vnl::Address dst;
	vnl::Page* _data;
	vnl::io::ByteBuffer _buf;
	vnl::io::TypeInput _in;
	vnl::io::TypeOutput _out;
	uint32_t next_seq = 0;
	int64_t timeout = 1000;
	bool do_fail_if_timeout = false;
	bool is_connected = false;
	
};



}

#endif /* INCLUDE_VNI_CLIENT_H_ */
