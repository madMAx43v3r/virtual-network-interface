/*
 * Interface.cpp
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#include <vni/Interface.h>

#include <vni/List.h>
#include <vni/Array.h>

namespace vni {

static vni::Interface* Interface::create(uint32_t hash) {
	switch(hash) {
	case vni::List<vni::Value>::VNI_HASH:
		return vni::GlobalPool<vni::List<vni::Value> >::create();
	case vni::Array<vni::Value>::VNI_HASH:
		return vni::GlobalPool<vni::Array<vni::Value> >::create();
	}
	return 0;
}

static void Interface::destroy(Interface* obj) {
	if(obj) {
		switch(obj->vni_hash_) {
		case vni::List<vni::Value>::VNI_HASH:
			vni::GlobalPool<vni::List<vni::Value> >::destroy((vni::List<vni::Value>*)obj); break;
		}
	}
}


}
