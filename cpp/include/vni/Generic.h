/*
 * Generic.h
 *
 *  Created on: Jul 6, 2016
 *      Author: mad
 */

#ifndef INCLUDE_VNI_GENERIC_H_
#define INCLUDE_VNI_GENERIC_H_

#include <vni/Value.h>
#include <vni/Interface.h>
#include <vni/List.h>
#include <vni/Integer.h>
// TODO: etc


namespace vni {

inline Value* read(vnl::io::TypeInput& in, int id, int size) {
	Value* obj = 0;
	switch(id) {
		case VNL_IO_NULL:
			break;
		case VNL_IO_BOOL:
			obj = Bool::create();
			if(obj) {
				in.getBool(((Bool*)obj)->value, id, size);
			}
			break;
		case VNL_IO_INTEGER:
			obj = Integer::create();
			if(obj) {
				in.readValue(((Integer*)obj)->value, id, size);
			}
			break;
		case VNL_IO_REAL:
			obj = Real::create();
			if(obj) {
				in.readValue(((Real*)obj)->value, id, size);
			}
			break;
		case VNL_IO_BINARY: {
			// TODO
			break;
		}
		case VNL_IO_STRING: {
			// TODO
			break;
		}
		case VNL_IO_ARRAY: {
			vni::List<Value>* list = vni::Interface::create<vni::List<Value> >(vni::List<Value>::VNI_HASH);
			if(list) {
				int num = size;
				id = in.getEntry(size);
				switch(id) {
					case VNL_IO_NULL: break;
					case VNL_IO_BOOL:
						for(int i = 0; i < num && !in.error(); ++i) {
							Value* tmp = vni::read(in, VNL_IO_BOOL, size);
							if(tmp) {
								list->push_back(tmp);
							}
						}
						break;
					case VNL_IO_INTEGER:
						// TODO
						break;
					case VNL_IO_REAL:
						// TODO
						break;
					case VNL_IO_BINARY:
						// TODO
						break;
					case VNL_IO_STRING:
						// TODO
						break;
					case VNL_IO_CALL:
						// TODO
						break;
					case VNL_IO_CONST_CALL:
						// TODO
						break;
					case VNL_IO_ARRAY:
						// TODO
						break;
					case VNL_IO_STRUCT:
						// TODO
						break;
					case VNL_IO_CLASS:
						// TODO
						break;
					case VNL_IO_INTERFACE:
						// TODO
						break;
					default:
						for(int i = 0; i < num && !in.error(); ++i) {
							in.skip(id, size);
						}
				}
			}
			obj = list;
			break;
		}
		case VNL_IO_CALL: {
			// TODO
			break;
		}
		case VNL_IO_CONST_CALL: {
			// TODO
			break;
		}
		case VNL_IO_CLASS: {
			uint32_t hash = 0;
			in.getHash(hash);
			obj = Value::create(hash);
			if(obj) {
				obj->deserialize(in, size);
			}
			break;
		}
		case VNL_IO_INTERFACE: {
			uint32_t hash = 0;
			in.getHash(hash);
			obj = Interface::create(hash);
			if(obj) {
				obj->deserialize(in, size);
			}
			break;
		}
		default:
			in.skip(id, size);
	}
	return obj;
}

template<>
static Value* read<Value>(vnl::io::TypeInput& in) {
	int size = 0;
	int id = in.getEntry(size);
	return vni::read(in, id, size);
}



}

#endif /* INCLUDE_VNI_GENERIC_H_ */
