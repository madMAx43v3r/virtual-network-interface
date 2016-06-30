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

class Client {
public:
	Client() {
		data = vnl::Page::alloc();
	}
	
	~Client() {
		data->free_all();
	}
	
	void connect(vnl::Engine* engine, vnl::Address dst_) {
		stream.open(engine);
		dst = dst_;
	}
	
	void set_timeout(int64_t millis) {
		timeout = millis;
	}
	
	void set_max_retries(int num) {
		max_retries = num;
	}
	
	Frame* call(vnl::io::PageBuffer& data, bool is_const) {
		data.flip();
		Frame frame;
		frame.dst_addr = dst;
		frame.data = data;
		frame.size = data.limit();
		frame.seq = next_seq++;
		frame.is_const = is_const;
		int left = max_retries;
		while(left > 0 || left < 0) {
			stream.send(&frame, vnl::Router::instance);
			// TODO
			left--;
		}
		return 0;
	}
	
private:
	vnl::Address dst;
	vnl::Stream stream;
	vnl::Page* data;
	uint32_t next_seq = 0;
	int64_t timeout = 3000;
	int max_retries = -1;
	
};



}

#endif /* INCLUDE_VNI_CLIENT_H_ */
