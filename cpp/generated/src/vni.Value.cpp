/*
 * Value.cpp
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#include <vni/Value.h>
#include <vni/List.hxx>
#include <test/TestType.h>


namespace vni {

vni::Value* Value::create() {
	return vni::Pool<vni::Value>::create();
}

vni::Value* Value::create(vnl::Hash32 hash) {
	switch(hash) {
	case vni::Value::VNI_HASH: return vni::create<vni::Value>();
	case test::TestType::VNI_HASH: return vni::create<test::TestType>();
	}
	return 0;
}

bool reflect::Value::is_base(vnl::Hash32 hash) {
	switch(hash) {
	}
	return false;
}

bool reflect::Value::is_instance(vnl::Hash32 hash) {
	switch(hash) {
		case test::TestType::VNI_HASH: return true;
	}
	return false;
}


}
