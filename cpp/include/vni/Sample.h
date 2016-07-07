/*
 * Sample.h
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_SAMPLE_H_
#define INCLUDE_VNI_SAMPLE_H_

#include <vni/Value.h>
#include <vnl/Packet.h>


namespace vni {

static const uint32_t PID_SAMPLE = 0xa37b95eb;

class Sample : public vnl::Packet {
public:
	Sample() : Packet(PID_SAMPLE) {
		payload = &data;
	}
	
	~Sample() {
		vni::destroy(data);
	}
	
	Value* data = 0;
	
protected:
	virtual void write(vnl::io::TypeOutput& out) const {
		vni::write(out, data);
	}
	
	virtual void read(vnl::io::TypeInput& in) {
		data = vni::read<vni::Value>(in);
	}
	
	
};


}

#endif /* INCLUDE_VNI_SAMPLE_H_ */
