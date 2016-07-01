/*
 * Type.cpp
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#include <vni/Type.h>


static vni::Type* vni::Type::create(uint32_t hash) {
	switch(hash) {
	
	}
	return 0;
}

static void vni::Type::destroy(Type* obj) {
	if(obj) {
		switch(obj->vni_hash_) {
		
		}
	}
}


