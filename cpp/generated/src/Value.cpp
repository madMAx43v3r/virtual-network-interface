/*
 * Value.cpp
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#include <vni/Value.h>
#include <test/TestType.h>



vni::Value* vni::Value::create() {
	return vni::GlobalPool<vni::Value>::create();
}

vni::Value* vni::Value::create(vnl::Hash32 hash) {
	switch(hash) {
	case vni::Value::VNI_HASH: return vni::GlobalPool<vni::Value>::create();
	case test::TestType::VNI_HASH: return vni::GlobalPool<test::TestType>::create();
	}
	return 0;
}

bool vni::reflect::Value::is_base(vnl::Hash32 hash) {
	switch(hash) {
	}
	return false;
}

bool vni::reflect::Value::is_instance(vnl::Hash32 hash) {
	switch(hash) {
		case test::TestType::VNI_HASH: return true;
	}
	return false;
}


