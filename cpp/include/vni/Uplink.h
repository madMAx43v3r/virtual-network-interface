/*
 * Uplink.h
 *
 *  Created on: Jul 1, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_UPLINK_H_
#define INCLUDE_VNI_UPLINK_H_

#include <vni/Port.h>
#include <vnl/Module.h>
#include <vnl/Map.h>
#include <vnl/io/Socket.h>


namespace vni {

class Uplink : public vnl::Module {
public:
	Uplink(const vnl::String& name)
		:	Module(name),
			socket(-1), out(&socket)
	{
		my_addr = vnl::Address(vnl::get_local_domain(), get_mac());
	}
	
	typedef vnl::MessageType<vnl::Address, 0x6245e4cc> open_t;
	typedef vnl::MessageType<vnl::Address, 0xc1d15cc6> close_t;
	
	typedef vnl::MessageType<int, 0x3273cc98> enable_t;
	
protected:
	virtual void main(vnl::Engine* engine) {
		open(my_addr);
		run();
		close(my_addr);
	}
	
	virtual bool handle(vnl::Message* msg) {
		if(msg->msg_id == enable_t::MID) {
			int fd = ((enable_t*)msg)->data;
			socket = vnl::io::Socket(fd);
			out = vnl::io::TypeOutput(&socket);
			for(vnl::Address addr : table.keys()) {
				out.putEntry(VNL_IO_INTERFACE, VNL_IO_BEGIN);
				out.putHash(open_t::MID);
				addr.serialize(out);
				out.putEntry(VNL_IO_INTERFACE, VNL_IO_END);
			}
			out.flush();
			log(DEBUG).out << "Enabled on socket " << vnl::dec(fd) << vnl::endl;
		} else if(msg->msg_id == open_t::MID) {
			vnl::Address addr = ((open_t*)msg)->data;
			out.putEntry(VNL_IO_INTERFACE, VNL_IO_BEGIN);
			out.putHash(open_t::MID);
			addr.serialize(out);
			out.putEntry(VNL_IO_INTERFACE, VNL_IO_END);
			out.flush();
			table[addr] = 1;
		} else if(msg->msg_id == close_t::MID) {
			vnl::Address addr = ((close_t*)msg)->data;
			out.putEntry(VNL_IO_INTERFACE, VNL_IO_BEGIN);
			out.putHash(close_t::MID);
			addr.serialize(out);
			out.putEntry(VNL_IO_INTERFACE, VNL_IO_END);
			out.flush();
			table.erase(addr);
		}
		return Module::handle(msg);
	}
	
	virtual bool handle(vnl::Packet* pkt) {
		if(pkt->pkt_id == vni::PID_SAMPLE || pkt->pkt_id == vni::PID_FRAME) {
			pkt->serialize(out);
			out.flush();
		}
		return false;
	}
	
private:
	vnl::Address my_addr;
	vnl::io::Socket socket;
	vnl::io::TypeOutput out;
	vnl::Map<vnl::Address, int> table;
	
};




}

#endif /* INCLUDE_VNI_UPLINK_H_ */
