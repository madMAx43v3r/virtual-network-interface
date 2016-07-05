/*
 * String.h
 *
 *  Created on: Jul 4, 2016
 *      Author: mad
 */

#ifndef CPP_INCLUDE_VNI_STRING_H_
#define CPP_INCLUDE_VNI_STRING_H_

#include <vni/StringBase.h>
#include <vnl/String.h>


namespace vni {

class String : public StringBase, public vnl::String {
public:
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		Writer wr(out);
		out.putString(*this);
	}
	
	virtual void deserialize(vnl::io::TypeInput& in, int size) {
		in.getString(*this);
		in.skip(VNL_IO_INTERFACE, 0);
	}
	
};


}

#endif  /* CPP_INCLUDE_VNI_STRING_H_ */
