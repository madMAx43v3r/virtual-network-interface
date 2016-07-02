/*
 * Port.h
 *
 *  Created on: Jul 1, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_PORT_H_
#define INCLUDE_VNI_PORT_H_

#include <vnl/Address.h>
#include <vnl/Util.h>


namespace vni {

class Port {
public:
	Port(vnl::String name) : name(name) {
		vnl::String domain = vnl::get_local_domain_name();
		vnl::String path;
		path << "vni/port/" << name;
		uplink = vnl::Endpoint(domain, vnl::String(path) << "/uplink");
		downlink = vnl::Endpoint(domain, vnl::String(path) << "/downlink");
	}
	
	vnl::String name;
	vnl::Endpoint uplink;
	vnl::Endpoint downlink;
	
	int fd = -1;
	
};



}

#endif /* INCLUDE_VNI_PORT_H_ */
