/*
 * TestNodeBase.h
 *
 *  Created on: Jul 4, 2016
 *      Author: mad
 */

#ifndef CPP_GENERATED_TEST_TESTNODEBASE_H_
#define CPP_GENERATED_TEST_TESTNODEBASE_H_

#include <vni/Value.h>
#include <vni/Object.h>
#include <test/TestType.h>


namespace test {

class TestNodeBase : public vni::Object {
public:
	int level;
	vni::String instance;
	
	static const uint32_t HASH = 0x12332453;
	
	TestNodeBase(const vnl::String& domain, const vnl::String& name)
		:	vni::Object(domain, name)
	{
		vni_hash_ = HASH;
		level = -1;
		instance = "blubb";
	}
	
	virtual const char* vni_type_name() const {
		return "test.TestNode";
	}
	
	class Client : public Object::Client {
	public:
		int test_func(const test::TestValue* val, test::TestValue*& _result) {
			_buf.wrap(_data);
			Writer _wr(_out);
			_wr.test_func(val);
			vnl::Packet* _pkt = _call();
			if(_pkt) {
				_result = vni::read<test::TestValue>(_in);
				_pkt->ack();
			}
			return _error;
		}
		int test_func2(const test::value_t& val, test::value_t& _result) {
			_buf.wrap(_data);
			Writer _wr(_out);
			_wr.test_func2(val);
			vnl::Packet* _pkt = _call();
			if(_pkt) {
				vni::read(_in, _result);
				_pkt->ack();
			}
			return _error;
		}
	};
	
	virtual void serialize(vnl::io::TypeOutput& _out) const {
		Writer _wr(_out);
		_out.putEntry(VNL_IO_CALL, 1);
		_out.putHash(0x8356785);
		vni::write(_out, level);
		_out.putEntry(VNL_IO_CALL, 1);
		_out.putHash(0x8357475);
		vni::write(_out, instance);
	}
	
protected:
	virtual void handle(test::TestType* ev, vnl::Address src_addr, vnl::Address dst_addr) = 0;
	
	virtual test::TestValue* test_func(test::TestValue*& val) const = 0;
	virtual test::value_t test_func2(test::value_t& val) const = 0;
	
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
		void test_func(const test::TestValue* val) {
			_out.putEntry(VNL_IO_CALL, 1);
			_out.putHash(0x2354923);
			vni::write(_out, val);
		}
		void test_func2(const test::value_t& val) {
			_out.putEntry(VNL_IO_CALL, 1);
			_out.putHash(0x2354923);
			vni::write(_out, &val);
		}
	protected:
		vnl::io::TypeOutput& _out;
	};
	
	virtual bool vni_call(vnl::io::TypeInput& in, uint32_t hash, int num_args) {
		switch(hash) {
		case 0x8356785: if(num_args == 1) { in.getValue(level); } break;
		case 0x8357475: if(num_args == 1) { vni::read(in, instance); } break;
		}
		return false;
	}
	
	virtual bool vni_const_call(vnl::io::TypeInput& in, uint32_t hash, int num_args, vnl::io::TypeOutput& out) {
		switch(hash) {
		case 0x2354923:
			switch(num_args) {
			case 1:
				test::TestValue* val = test::TestValue::read(in);
				test::TestValue* _result = test_func(val);
				test::TestValue::write(out, _result);
				test::TestValue::destroy(_result);
				test::TestValue::destroy(val);
				break;
			}
			break;
		case 0x2354924:
			switch(num_args) {
			case 1:
				test::value_t val; test::value_t::read(in, &val);
				test::value_t _result = test_func2(val);
				test::value_t::write(out, &_result);
				break;
			}
			break;
		}
		return false;
	}
	
	virtual bool handle(vni::Value* sample, vnl::Address src_addr, vnl::Address dst_addr) {
		switch(sample->vni_hash_) {
		case test::TestType::VNI_HASH: handle(sample, src_addr, dst_addr); return true;
		}
		return false;
	}
	
};


}

#endif /* CPP_GENERATED_TEST_TESTNODEBASE_H_ */
