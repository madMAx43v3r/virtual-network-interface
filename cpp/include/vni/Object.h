/*
 * Object.h
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_OBJECT_H_
#define INCLUDE_VNI_OBJECT_H_

#include <vnl/Module.h>
#include <vni/ObjectBase.h>
#include <vni/info/Announce.hxx>


namespace vni {

class Object : public ObjectBase, public vnl::Module {
public:
	Object(const vnl::String& domain, const vnl::String& topic)
		:	Module(),
			my_domain(domain), my_topic(topic),
			my_address(domain, topic),
			in(&buf_in), out(&buf_out)
	{
	}
	
protected:
	virtual void run() {
		Module::open(my_address);
		vni::info::Announce* announce = vni::info::Announce::create();
		announce->domain = my_domain;
		announce->topic = my_topic;
		publish(announce, vnl::Address(my_address.A, "vni/info/Announce"));
		Module::run();
		Module::close(my_address);
	}
	
	virtual void publish(vni::Value* data, vnl::Address topic) {
		vni::Sample* pkt = buffer.create<vni::Sample>();
		pkt->src_addr = my_address;
		pkt->data = data;
		send_async(pkt, topic);
	}
	
	virtual bool handle(vnl::Message* msg) {
		if(msg->msg_id == vnl::Packet::MID) {
			vnl::Packet* pkt = (vnl::Packet*)msg;
			uint32_t& last_seq = sources[pkt->src_addr];
			// TODO: handle sequence wrap around
			if(pkt->seq_num <= last_seq) {
				return handle_duplicate(pkt);
			}
			last_seq = pkt->seq_num;
			return handle(pkt);
		}
		return Module::handle(msg);
	}
	
	virtual bool handle(vnl::Packet* pkt) {
		if(pkt->pkt_id == vni::PID_FRAME) {
			Frame* request = (Frame*)pkt->payload;
			Frame* result = buffer.create<Frame>();
			result->seq_num = request->seq_num;
			buf_in.wrap(request->data, request->size);
			int size = 0;
			int id = in.getEntry(size);
			if(id == VNL_IO_INTERFACE) {
				uint32_t hash = 0;
				in.getHash(hash);
				while(!in.error()) {
					uint32_t hash = 0;
					int size = 0;
					int id = in.getEntry(size);
					if(id == VNL_IO_CALL) {
						in.getHash(hash);
						bool res = vni_call(in, hash, size);
						if(!res) {
							in.skip(id, size, hash);
						}
					} else if(id == VNL_IO_CONST_CALL) {
						in.getHash(hash);
						if(!vni_const_call(in, hash, size, out)) {
							in.skip(id, size, hash);
							out.putNull();
						}
					} else if(id == VNL_IO_INTERFACE && size == VNL_IO_END) {
						break;
					} else {
						in.skip(id, size);
					}
				}
				out.flush();
				result->size = buf_out.position();
				result->data = buf_out.release();
			}
			send_async(result, request->src_addr);
		}
		return false;
	}
	
	virtual bool handle_duplicate(vnl::Packet* pkt) {
		if(pkt->pkt_id == vni::PID_FRAME) {
			Frame* result = buffer.create<Frame>();
			send_async(result, pkt->src_addr);
		}
		return false;
	}
	
	virtual vni::info::Class get_class() const {
		vni::info::Class res;
		res.name = VNI_NAME;
		// TODO
		return res;
	}
	
	virtual Binary vni_serialize() const {
		vnl::io::ByteBuffer buf;
		vnl::io::TypeOutput out(&buf);
		serialize(out);
		out.flush();
		Binary blob;
		blob.size = buf.position();
		blob.data = buf.release();
		return blob;
	}
	
	virtual void vni_deserialize(const vni::Binary& blob) {
		vnl::io::ByteBuffer buf;
		vnl::io::TypeInput in(&buf);
		buf.wrap(blob.data, blob.size);
		vni::read(in, *this);
	}
	
protected:
	vnl::Address my_address;
	vnl::String my_domain;
	vnl::String my_topic;
	
private:
	vnl::Map<vnl::Address, uint32_t> sources;
	vnl::io::ByteBuffer buf_in;
	vnl::io::ByteBuffer buf_out;
	vnl::io::TypeInput in;
	vnl::io::TypeOutput out;
	
};



}

#endif /* INCLUDE_VNI_OBJECT_H_ */
