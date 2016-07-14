/*
 * Node.h
 *
 *  Created on: Jul 6, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_NODE_H_
#define INCLUDE_VNI_NODE_H_

#include <vni/NodeBase.hxx>


namespace vni {

class Node : public NodeBase {
public:
	Node(const vnl::String& domain, const vnl::String& name)
		:	NodeBase(domain, name)
	{
	}
	
protected:
	virtual bool handle_switch(const vni::Value* sample, vnl::Address src_addr, vnl::Address dst_addr) = 0;
	
	virtual bool handle(vnl::Packet* pkt) {
		if(pkt->pkt_id == vni::PID_SAMPLE) {
			handle_switch(((Sample*)pkt)->data, pkt->src_addr, pkt->dst_addr);
			return false;
		} else {
			return NodeBase::handle(pkt);
		}
	}
	
	
};



}

#endif /* INCLUDE_VNI_NODE_H_ */
