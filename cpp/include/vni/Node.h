/*
 * Node.h
 *
 *  Created on: Jun 28, 2016
 *      Author: mad
 */

#ifndef CPP_INCLUDE_VNI_NODE_H_
#define CPP_INCLUDE_VNI_NODE_H_

#include <vni/NodeBase.h>


namespace vni {

class Node : public vni::NodeBase {
public:
	
protected:
	const vnl::Address* vni_src = 0;
	const vnl::Address* vni_dst = 0;
	
};


}

#endif /* CPP_INCLUDE_VNI_NODE_H_ */
