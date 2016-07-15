/*
 * String.h
 *
 *  Created on: Jul 4, 2016
 *      Author: mad
 */

#ifndef CPP_INCLUDE_VNI_STRING_H_
#define CPP_INCLUDE_VNI_STRING_H_

#include <vni/StringSupport.hxx>
#include <vnl/String.h>


namespace vni {

class String : public StringBase, public vnl::String {
public:
	
	virtual void to_string(vnl::String& str) const {
		// TODO: escape
		str << "\"" << *this << "\"";
	}
	
	virtual void serialize(vnl::io::TypeOutput& out) const {
		Writer wr(out);
		out.putString(*this);
	}
	
	virtual void deserialize(vnl::io::TypeInput& in, int size) {
		in.getString(*this);
	}
	
};


}

#endif  /* CPP_INCLUDE_VNI_STRING_H_ */
