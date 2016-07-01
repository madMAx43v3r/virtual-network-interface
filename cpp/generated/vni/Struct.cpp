/*
 * Type.cpp
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#include <vni/Struct.h>


static vni::Struct* vni::Struct::create(uint32_t hash) {
	switch(hash) {
	
	}
	return 0;
}

static void vni::Struct::destroy(Struct* obj) {
	if(obj) {
		switch(obj->vni_hash_) {
		
		}
	}
}


