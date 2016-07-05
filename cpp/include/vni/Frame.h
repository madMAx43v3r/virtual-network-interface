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
	
	Frame() : Packet(PID_FRAME), out(&buf) {
		payload = this;
	}
	
	~Frame() {
		if(data) {
			data->free_all();
		}
	}
	
	virtual void deserialize(vnl::io::TypeInput& in, int size) {
		if(!data) {
			data = vnl::Page::alloc();
		}
		buf.wrap(data);
		Packet::deserialize(in, size);
		out.flush();
	}
	
protected:
	virtual void write(vnl::io::TypeOutput& out) const {
		if(data) {
			out.writeBinary(data, size);
		}
	}
	
	virtual void read(vnl::io::TypeInput& in, int id, int size) {
		in.copy(id, size, &out);
	}
	
private:
	vnl::io::ByteBuffer buf;
	vnl::io::TypeOutput out;
	
};



}

#endif /* INCLUDE_VNI_FRAME_H_ */
