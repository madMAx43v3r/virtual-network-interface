/*
 * Class.h
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_CLASS_H_
#define INCLUDE_VNI_CLASS_H_

#include <vni/Struct.h>


namespace vni {

class Class : public Struct {
public:
	
	static Class* create(uint32_t hash);
	
	static void destroy(Class* obj);
	
	
};



}

#endif /* INCLUDE_VNI_CLASS_H_ */
