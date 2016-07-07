/*
 * Array.h
 *
 *  Created on: Jul 5, 2016
 *      Author: mad
 */

#ifndef CPP_INCLUDE_VNI_ARRAY_H_
#define CPP_INCLUDE_VNI_ARRAY_H_

#include <vni/ArrayBase.h>
#include <vnl/Array.h>


namespace vni {

template<typename T>
class Array : public vni::ArrayBase<T>, public vnl::Array<T> {
public:
	virtual void serialize(vnl::io::TypeOutput& out) const {
		Writer wr(out);
		out.putEntry(VNL_IO_ARRAY, size());
		out.putEntry(VNL_IO_CLASS, 0);
		for(vnl::Array<T>::const_iterator iter = begin(); iter != end(); ++iter) {
			iter->serialize(out);
		}
	}
	
	virtual void deserialize(vnl::io::TypeInput& in, int size) {
		int id = in.getEntry(size);
		if(id == VNL_IO_ARRAY) {
			int dim = size;
			id = in.getEntry(size);
			if(id == VNL_IO_CLASS) {
				for(int i = 0; i < dim && !in.error(); ++i) {
					id = in.getEntry(size);
					uint32_t hash = 0;
					switch(id) {
						case VNL_IO_STRUCT:
							push_back(T()).deserialize(in, size);
							break;
						case VNL_IO_CLASS:
						case VNL_IO_INTERFACE:
							in.getHash(hash);
							push_back(T()).deserialize(in, size);
							break;
						default: in.set_error(VNL_IO_INVALID_ID);
					}
				}
			} else {
				in.set_error(VNL_IO_INVALID_ID);
			}
		} else {
			in.set_error(VNL_IO_INVALID_ID);
		}
		in.skip(VNL_IO_INTERFACE, 0);
	}
};


template<typename T>
void _vni_deserialize_array_special(vnl::io::TypeInput& in, int size, vnl::Array<T>& out) {
	int id = in.getEntry(size);
	if(id == VNL_IO_ARRAY) {
		int num = size;
		id = in.getEntry(size);
		bool fail = false;
		if(id == VNL_IO_INTEGER) {
			switch(size) {
				case VNL_IO_BYTE:
					int8_t tmp = 0;
					for(int i = 0; i < num && !in.error(); ++i) {
						in.readChar(tmp);
						out.push_back(tmp);
					}
					break;
				case VNL_IO_WORD: {
					int16_t tmp = 0;
					for(int i = 0; i < num && !in.error(); ++i) {
						in.readShort(tmp);
						out.push_back(tmp);
					}
					break;
				}
				case VNL_IO_DWORD: {
					int32_t tmp = 0;
					for(int i = 0; i < num && !in.error(); ++i) {
						in.readInt(tmp);
						out.push_back(tmp);
					}
					break;
				}
				case VNL_IO_QWORD: {
					int64_t tmp = 0;
					for(int i = 0; i < num && !in.error(); ++i) {
						in.readLong(tmp);
						out.push_back(tmp);
					}
					break;
				}
				default: fail = true;
			}
		} else if(id == VNL_IO_REAL) {
			switch(size) {
				case VNL_IO_DWORD: {
					float tmp = 0;
					for(int i = 0; i < num && !in.error(); ++i) {
						in.readFloat(tmp);
						out.push_back(tmp);
					}
					break;
				}
				case VNL_IO_QWORD: {
					double tmp = 0;
					for(int i = 0; i < num && !in.error(); ++i) {
						in.readDouble(tmp);
						out.push_back(tmp);
					}
					break;
				}
				default: fail = true;
			}
		} else if(id == VNL_IO_BOOL) {
			if(size == 1) {
				int tmp = 0;
				for(int i = 0; i < num && !in.error(); ++i) {
					int check = in.getEntry(tmp);
					out.push_back(check == VNL_IO_BOOL && tmp == VNL_IO_TRUE);
				}
			} else {
				fail = true;
			}
		} else {
			fail = true;
		}
		if(fail) {
			for(int i = 0; i < num; ++i) {
				in.skip(id, size);
			}
		}
	} else {
		in.skip(id, size);
	}
	in.skip(VNL_IO_INTERFACE, 0);
}


#define _VNI_ARRAY_SPECIAL(T, io_id, io_size, io_write_func) \
	template<> \
	class Array<T> : public vni::ArrayBase<T>, public vnl::Array<T> { \
	public: \
		virtual void serialize(vnl::io::TypeOutput& out) const { \
			Writer wr(out); \
			out.putEntry(VNL_IO_ARRAY, size()); \
			out.putEntry(io_id, io_size); \
			for(vnl::Array<T>::const_iterator iter = begin(); iter != end(); ++iter) { \
				out.io_write_func(*iter); \
			} \
		} \
		virtual void deserialize(vnl::io::TypeInput& in, int size) { \
			_vni_deserialize_array_special(in, size, *this); \
		} \
	};

_VNI_ARRAY_SPECIAL(bool, VNL_IO_BOOL, VNL_IO_BYTE, putBool);
_VNI_ARRAY_SPECIAL(int8_t, VNL_IO_INTEGER, VNL_IO_BYTE, writeChar);
_VNI_ARRAY_SPECIAL(int16_t, VNL_IO_INTEGER, VNL_IO_WORD, writeShort);
// TODO



}

#endif /* CPP_INCLUDE_VNI_ARRAY_H_ */
