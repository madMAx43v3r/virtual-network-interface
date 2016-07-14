/*
 * Module.h
 *
 *  Created on: Jul 6, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_MODULE_H_
#define INCLUDE_VNI_MODULE_H_

#include <vni/ModuleBase.hxx>


namespace vni {

class Module : public ModuleBase {
public:
	Module(const vnl::String& domain, const vnl::String& name)
		:	ModuleBase(domain, name)
	{
	}
	
protected:
	virtual bool handle_switch(vni::Value* sample, vnl::Address src_addr, vnl::Address dst_addr) = 0;
	
	virtual bool handle(vnl::Packet* pkt) {
		if(pkt->pkt_id == vni::PID_SAMPLE) {
			vni::Value* sample = ((Sample*)pkt)->data;
			if(sample) {
				handle_switch(sample, pkt->src_addr, pkt->dst_addr);
			}
			return false;
		} else {
			return ModuleBase::handle(pkt);
		}
	}
	
	
};



}

#endif /* INCLUDE_VNI_MODULE_H_ */
