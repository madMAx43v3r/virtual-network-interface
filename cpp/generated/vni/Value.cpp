/*
 * Value.cpp
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#include <vni/Value.h>


namespace vni {

static vni::Value* Value::create(uint32_t hash) {
	switch(hash) {
	
	}
	return 0;
}

static void Value::destroy(Value* obj) {
	if(obj) {
		switch(obj->vni_hash_) {
		
		}
	}
}


}
