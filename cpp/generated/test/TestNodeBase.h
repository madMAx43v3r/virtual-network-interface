/*
 * TestNodeBase.h
 *
 *  Created on: Jul 4, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_TEST_TESTNODEBASE_H_
#define CPP_GENERATED_TEST_TESTNODEBASE_H_

#include <vni/Object.h>
#include <test/TestType.h>

namespace test {

class TestNodeBase : public vni::Object {
public:
	static const uint32_t HASH = 0x12332453;
	
	TestNodeBase() {
		vni_hash_ = HASH;
	}
	
	virtual const char* vni_type_name() const {
		return "test.TestNode";
	}
	
	class Client : public vni::Client {
	public:
	};
	
	class Config : public vni::Struct {
	public:
		int level;
		vni::String instance;
		Config() {
			level = -1;
			instance = "blubb";
		}
		// all the other struct stuff
	};
	
protected:
	test::TestNodeBase::Config config;
	
	virtual void handle(test::TestType* ev, vnl::Address src_addr, vnl::Address dst_addr) = 0;
	
protected:
	class Writer {
	public:
		Writer(vnl::io::TypeOutput& out) : _out(out) {
			_out.putEntry(VNL_IO_INTERFACE, VNL_IO_BEGIN);
			_out.putHash(HASH);
		}
		~Writer() {
			_out.putEntry(VNL_IO_INTERFACE, VNL_IO_END);
		}
		void setConfig(Config& conf) {
			_out.putEntry(VNL_IO_CALL, 1);
			_out.putHash(0x1337);
			conf.serialize(_out);
		}
	protected:
		vnl::io::TypeOutput& _out;
	};
	
	virtual bool vni_call(vnl::io::TypeInput& in, uint32_t hash, int num_args) {
		switch(hash) {
		case 0x1337:
			if(num_args == 1) {
				test::TestNodeBase::Config conf;
				test::TestNodeBase::Config::read(in, &conf);
				setConfig(conf);
				return true;
			}
			break;
		}
		return false;
	}
	
	virtual bool vni_const_call(vnl::io::TypeInput& in, uint32_t hash, int num_args, vnl::io::TypeOutput& out) {
		switch(hash) {
		}
		return false;
	}
	
	virtual bool handle(vni::Class* sample, vnl::Address src_addr, vnl::Address dst_addr) {
		switch(sample->vni_hash_) {
		case test::TestType::HASH: handle(sample, src_addr, dst_addr); return true;
		}
		return false;
	}
	
private:
	void setConfig(test::TestNodeBase::Config& conf) {
		test::TestNodeBase::config = conf;
	}
	
};


}

#endif /* CPP_GENERATED_TEST_TESTNODEBASE_H_ */
