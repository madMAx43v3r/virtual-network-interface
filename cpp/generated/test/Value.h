/*
 * Value.h
 *
 *  Created on: May 22, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_TEST_VALUE_H_
#define CPP_GENERATED_TEST_VALUE_H_

#include <vnl/io.h>


namespace test {

class Value : public vnl::io::Serializable {
public:
	
	int32_t x;
	int32_t y;
	int32_t z;
	
	virtual void serialize(vnl::io::TypeOutput<vnl::io::PageBuffer>& stream) {
		stream.beginType(0x1212323435345ULL, 3);
		stream.putInt(0x4354534, x);
		stream.putInt(0x4345534, y);
		stream.putInt(0x4356544, z);
	}
	
	virtual void deserialize(vnl::io::TypeInput<vnl::io::PageBuffer>& stream, int num_entries) {
		for(int i = 0; i < num_entries; ++i) {
			uint32_t hash;
			int16_t size;
			stream.beginEntry(hash, size);
			switch(hash) {
				case 0x4354534: stream.getInt(x, size); break;
				case 0x4345534: stream.getInt(y, size); break;
				case 0x4356544: stream.getInt(z, size); break;
				default: stream.skip(size);
			}
		}
	}
	
};


}

#endif /* CPP_GENERATED_TEST_VALUE_H_ */
