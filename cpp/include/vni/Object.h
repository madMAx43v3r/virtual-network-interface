/*
 * Object.h
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_OBJECT_H_
#define INCLUDE_VNI_OBJECT_H_

#include <vni/ObjectBase.h>
#include <vni/Binary.h>
#include <vnl/Module.h>


namespace vni {

enum {
	VNI_SUCCESS = 0, VNI_ERROR = -1
};

class Object : public ObjectBase, public vnl::Module {
public:
	Object(const vnl::String& domain, const vnl::String& name)
		:	Module(vnl::String(domain) << "/" << name),
			my_address(domain, name),
			in(&in_buf), out(&out_buf)
	{
	}
	
protected:
	virtual void run() {
		Module::open(my_address);
		Module::run();
		Module::close(my_address);
	}
	
	virtual bool handle(vnl::Packet* pkt) {
		if(pkt->pkt_id == vni::PID_FRAME) {
			Frame* request = (Frame*)pkt->payload;
			Frame* result = buffer.create<Frame>();
			result->seq = request->seq;
			uint32_t& last_seq = clients[request->src_addr];
			if(request->seq <= last_seq) {
				send_async(result, request->src_addr);
				return false;
			}
			in_buf.wrap(request->data, request->size);
			int size = 0;
			int id = in.getEntry(size);
			if(id == VNL_IO_INTERFACE) {
				uint32_t hash = 0;
				in.getHash(hash);
				if(hash == vni_hash_) {
					result->data = vnl::Page::alloc();
					out_buf.wrap(result->data);
					while(!in.error()) {
						uint32_t hash = 0;
						int size = 0;
						int id = in.getEntry(size);
						if(id == VNL_IO_CALL) {
							in.getHash(hash);
							bool res = vni_call(in, hash, size);
							if(!res) {
								in.skip(VNL_IO_CALL, size);
							}
							out.putBool(res);
						} else if(id == VNL_IO_CONST_CALL) {
							in.getHash(hash);
							if(!vni_const_call(in, hash, size, out)) {
								in.skip(VNL_IO_CONST_CALL, size);
								out.putNull();
							}
						} else if(id == VNL_IO_INTERFACE && size == VNL_IO_END) {
							break;
						} else {
							in.skip(id, size);
						}
					}
					out.flush();
					result->size = out_buf.position();
				}
			}
			send_async(result, request->src_addr);
			last_seq = request->seq;
		}
		return false;
	}
	
	virtual Binary vni_serialize() const {
		Binary blob;
		blob.data = vnl::Page::alloc();
		vnl::io::ByteBuffer buf;
		vnl::io::TypeOutput out(&buf);
		buf.wrap(blob.data);
		serialize(out);
		out.flush();
		blob.size = buf.position();
		return blob;
	}
	
	virtual void vni_deserialize(vni::Binary& blob) {
		vnl::io::ByteBuffer buf;
		vnl::io::TypeInput in(&buf);
		buf.wrap(blob.data, blob.size);
		vni::read(in, *this);
	}
	
protected:
	vnl::Address my_address;
	
private:
	vnl::Map<vnl::Address, uint32_t> clients;
	vnl::io::ByteBuffer in_buf;
	vnl::io::ByteBuffer out_buf;
	vnl::io::TypeInput in;
	vnl::io::TypeOutput out;
	
};



}

#endif /* INCLUDE_VNI_OBJECT_H_ */
