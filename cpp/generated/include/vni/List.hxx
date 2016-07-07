/*
 * List.hxx
 *
 *  Created on: Jul 7, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_INCLUDE_VNI_LIST_HXX_
#define CPP_GENERATED_INCLUDE_VNI_LIST_HXX_

#include <vni/List.h>


namespace vni {

template<typename T>
vni::List<T>* ListBase<T>::create() {
	return vni::Pool<vni::List<T> >::create();
}

template<typename T>
vni::List<T>* ListBase<T>::create(vnl::Hash32 hash) {
	switch(hash) {
		case vni::List<T>::VNI_HASH: return vni::Pool<vni::List<T> >::create();
	}
	return 0;
}


}

#endif /* CPP_GENERATED_INCLUDE_VNI_LIST_HXX_ */
