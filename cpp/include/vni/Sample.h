/*
 * Sample.h
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_SAMPLE_H_
#define INCLUDE_VNI_SAMPLE_H_

#include <vni/Class.h>
#include <vnl/Packet.h>


namespace vni {

static const uint32_t PID_SAMPLE = 0xa37b95eb;

class Sample : public vnl::Packet {
public:
	Sample() : Packet(PID_SAMPLE) {
		payload = &data;
	}
	
	~Sample() {
		vni::Class::destroy(data);
	}
	
	Class* data = 0;
	
protected:
	virtual void write(vnl::io::TypeOutput& out) const {
		if(data) {
			data->serialize(out);
		} else {
			out.putNull();
		}
	}
	
	virtual void read(vnl::io::TypeInput& in) {
		data = vni::Class::read<vni::Class>(in);
	}
	
	
};


}

#endif /* INCLUDE_VNI_SAMPLE_H_ */
