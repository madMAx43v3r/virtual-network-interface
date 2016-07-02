/*
 * Object.h
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_OBJECT_H_
#define INCLUDE_VNI_OBJECT_H_

#include <vni/Interface.h>
#include <vnl/Module.h>


namespace vni {

class Object : public Interface, public vnl::Module {
public:
	Object(const vnl::String& name)
		:	Module(name),
			in(&in_buf), out(&out_buf)
	{
	}
	
protected:
	virtual bool handle(vni::Class* sample, vnl::Address src_addr, vnl::Address dst_addr) = 0;
	
	virtual bool handle(vnl::Packet* pkt) {
		if(pkt->pkt_id == vni::PID_SAMPLE) {
			handle(((Sample*)pkt)->data, pkt->src_addr, pkt->dst_addr);
		} else if(pkt->pkt_id == vni::PID_FRAME) {
			Frame* req = (Frame*)pkt->payload;
			Frame* ret = buffer.create<Frame>();
			in_buf.wrap(req->data, req->size);
			int size = 0;
			int id = in.getEntry(size);
			if(id == VNL_IO_INTERFACE) {
				uint32_t hash = 0;
				in.getHash(hash);
				if(hash == vni_hash_) {
					ret->data = vnl::Page::alloc();
					out_buf.wrap(ret->data);
					while(!in.error()) {
						uint32_t hash = 0;
						int size = 0;
						int id = in.getEntry(size);
						if(id == VNL_IO_CALL) {
							in.getHash(hash);
							bool res = vni_call(in, hash, size);
							out.putBool(res);
						} else if(id == VNL_IO_CONST_CALL) {
							in.getHash(hash);
							if(!vni_const_call(in, hash, size, out)) {
								out.putNull();
							}
						} else if(id == VNL_IO_INTERFACE && size == VNL_IO_END) {
							break;
						}
					}
					out.flush();
					ret->size = out_buf.position();
				}
			}
			ret->seq = req->seq;
			send_async(ret, req->src_addr);
		}
		return false;
	}
	
private:
	vnl::io::ByteBuffer in_buf;
	vnl::io::ByteBuffer out_buf;
	vnl::io::TypeInput in;
	vnl::io::TypeOutput out;
	
};



}

#endif /* INCLUDE_VNI_OBJECT_H_ */
