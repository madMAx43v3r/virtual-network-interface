/*
 * Class.cpp
 *
 *  Created on: Jun 30, 2016
 *      Author: mad
 */

#include <vni/Class.h>


static vni::Class* vni::Class::create(uint32_t hash) {
	switch(hash) {
	
	}
	return 0;
}

static void vni::Class::destroy(Class* obj) {
	if(obj) {
		switch(obj->vni_hash_) {
		
		}
	}
}


