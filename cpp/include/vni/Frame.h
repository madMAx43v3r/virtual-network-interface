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
	Frame() : Packet(PID_FRAME) {
		payload = this;
	}
	
	vnl::Page* data = 0;
	uint32_t size = 0;
	uint32_t seq = 0;
	bool is_const = false;
	
};



}

#endif /* INCLUDE_VNI_FRAME_H_ */
