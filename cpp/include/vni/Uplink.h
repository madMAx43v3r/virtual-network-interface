/*
 * Uplink.h
 *
 *  Created on: Jul 1, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_UPLINK_H_
#define INCLUDE_VNI_UPLINK_H_

#include <vnl/Module.h>
#include <vnl/Map.h>
#include <vnl/io/Socket.h>


namespace vni {

class Uplink : public vnl::Module {
public:
	Uplink(Port* port)
		:	Module(port->uplink.mac),
			port(port), socket(port->fd), out(&socket)
	{
		epoch = vnl::currentTime();
	}
	
	static const int beat_interval = 1000;
	static const int maintain_interval = 10000;
	static const int fwd_timeout = 100;
	
	typedef vnl::PacketType<vnl::Address, 0x6245e4cc> open_t;
	typedef vnl::PacketType<vnl::Address, 0xc1d15cc6> close_t;
	
	typedef vnl::PacketType<vnl::Address, 0x3273cc98> forward_t;
	typedef vnl::PacketType<vnl::Address, 0xf64ddc0f> heart_beat_t;
	
protected:
	virtual void main(vnl::Engine* engine) {
		set_timeout(beat_interval*1000, std::bind(&Uplink::heart_beat, this), vnl::Timer::REPEAT);
		set_timeout(maintain_interval*1000, std::bind(&Uplink::maintain, this), vnl::Timer::REPEAT);
		run();
	}
	
	virtual bool handle(vnl::Packet* pkt) {
		if(pkt->dst_addr == port->uplink) {
			if(pkt->pkt_id == open_t::PID) {
				vnl::Address addr = ((open_t*)pkt)->data;
				out.putEntry(VNL_IO_INTERFACE, VNL_IO_BEGIN);
				out.putHash(open_t::PID);
				addr.serialize(out);
				out.putEntry(VNL_IO_INTERFACE, VNL_IO_END);
				out.flush();
			} else if(pkt->pkt_id == close_t::PID) {
				vnl::Address addr = ((close_t*)pkt)->data;
				out.putEntry(VNL_IO_INTERFACE, VNL_IO_BEGIN);
				out.putHash(close_t::PID);
				addr.serialize(out);
				out.putEntry(VNL_IO_INTERFACE, VNL_IO_END);
				out.flush();
			} else if(pkt->pkt_id == forward_t::PID) {
				vnl::Address addr = ((forward_t*)pkt)->data;
				forward[addr] = vnl::currentTime() - epoch;
				open(addr);
			}
		} else if(pkt->pkt_id == vni::PID_SAMPLE || pkt->pkt_id == vni::PID_FRAME) {
			pkt->serialize(out);
			out.flush();
		}
		if(out.error()) {
			log(ERROR).out << "output error, closing.";
			die();
		}
		return false;
	}
	
	void heart_beat() {
		out.putEntry(VNL_IO_INTERFACE, VNL_IO_BEGIN);
		out.putHash(heart_beat_t::PID);
		out.putEntry(VNL_IO_INTERFACE, VNL_IO_END);
		out.flush();
	}
	
	void maintain() {
		// TODO
	}
	
private:
	Port* port;
	vnl::io::Socket socket;
	vnl::io::TypeOutput out;
	vnl::Map<vnl::Address, int> forward;
	
	int64_t epoch;
	
};




}

#endif /* INCLUDE_VNI_UPLINK_H_ */
