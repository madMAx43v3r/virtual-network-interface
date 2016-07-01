/*
 * Client.h
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_CLIENT_H_
#define INCLUDE_VNI_CLIENT_H_

#include <vni/Frame.h>
#include <vnl/Stream.h>
#include <vnl/Router.h>


namespace vni {

class Client : public vnl::Stream {
public:
	Client()
		:	in(&buf), out(&buf)
	{
		data = vnl::Page::alloc();
	}
	
	~Client() {
		if(is_connected) {
			Stream::close(dst);
		}
		data->free_all();
	}
	
	void vni_connect(vnl::Engine* engine, vnl::Address dst_) {
		if(is_connected) {
			Stream::close(dst);
		}
		dst = dst_;
		Stream::connect(engine);
		Stream::open(dst);
		is_connected = true;
	}
	
	void vni_set_timeout(int64_t millis) {
		timeout = millis;
	}
	
protected:
	vnl::Packet* call() {
		out.flush();
		next_seq++;
		Frame* ret = 0;
		while(true) {
			Frame frame;
			frame.data = data;
			frame.size = buf.position();
			frame.seq = next_seq;
			send(&frame, dst);
			frame.data = 0;
			// TODO
		}
		if(ret) {
			buf.wrap(ret->data, ret->size);
		}
		return ret;
	}
	
private:
	vnl::Address dst;
	vnl::Page* data;
	vnl::io::ByteBuffer buf;
	vnl::io::TypeInput in;
	vnl::io::TypeOutput out;
	uint32_t next_seq = 0;
	int64_t timeout = 1000;
	bool is_connected = false;
	
};



}

#endif /* INCLUDE_VNI_CLIENT_H_ */
