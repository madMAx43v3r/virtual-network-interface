/*
 * Frame.h
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_FRAME_H_
#define INCLUDE_VNI_FRAME_H_

#include <vnl/Packet.h>


namespace vni {

static const uint32_t PID_FRAME = 0xde2104f2;

class Frame : public vnl::Packet {
public:
	vnl::Page* data = 0;
	int size = 0;
	uint32_t seq = 0;
	
	Frame() : Packet(PID_FRAME) {
		payload = this;
	}
	
	~Frame() {
		if(data) {
			data->free_all();
		}
	}
	
protected:
	virtual void write(vnl::io::TypeOutput& out) const {
		if(data) {
			out.putBinary(data, size);
		} else {
			out.putNull();
		}
	}
	
	virtual void read(vnl::io::TypeInput& in, int id, int size) {
		if(!data) {
			data = vnl::Page::alloc();
			in.getBinary(data, size);
		} else {
			in.skip(id, size);
		}
	}
	
};



}

#endif /* INCLUDE_VNI_FRAME_H_ */
