/*
 * test_module.cpp
 *
 *  Created on: Jul 15, 2016
 *      Author: mad
 */

#include <test/TestNodeSupport.hxx>
#include <vnl/Terminal.h>


namespace test {

class TestNode : public TestNodeBase {
public:
	TestNode(const vnl::String& domain_, const vnl::String& topic_)
		:	TestNodeBase(domain_, topic_)
	{
	}
	
	int counter = 0;
	
protected:
	virtual void main(vnl::Engine* engine) {
		log(INFO).out << "Hello World: level=" << level << ", instance=" << instance << vnl::endl;
		set_timeout(1000*1000, std::bind(&TestNode::print_stats, this), VNL_TIMER_REPEAT);
		vnl::Receiver topic(this, vnl::Address(my_domain, "test/topic"));
		run();
	}
	
	void print_stats() {
		log(INFO).out << "counter = " << vnl::dec(counter) << " (System: " << vnl::Page::get_num_alloc() << " Pages, " << vnl::Block::get_num_alloc() << " Blocks)" << vnl::endl;
		counter = 0;
	}
	
	virtual void handle(const TestType& ev, vnl::Address src_addr, vnl::Address dst_addr) {
		counter++;
	}
	
	virtual TestValue test_func(const TestValue& val) const {
		TestValue res;
		res.x = 1337;
		res.y = 1338;
		res.z = 1339;
		return res;
	}
	
	virtual test_value_t test_func2(const test_value_t& val) const {
		test_value_t res;
		res.pos[0] = 9;
		res.pos[1] = 99;
		res.pos[2] = 999;
		return res;
	}
	
};

class TroubleMaker : public vni::Module {
public:
	TroubleMaker(const vnl::String& domain_, const vnl::String& topic_)
		:	Module(domain_, topic_)
	{
		dst = vnl::Address(my_domain, "test/topic");
	}
	
	vnl::Address dst;
	
protected:
	virtual void main(vnl::Engine* engine) {
		log(INFO).out << "trouble begins ..." << vnl::endl;
		set_timeout(0, std::bind(&TroubleMaker::fire_machine_gun, this), VNL_TIMER_REPEAT);
		run();
	}
	
	void fire_machine_gun() {
		TestType* data = vni::create<TestType>();
		publish(data, dst);
		std::this_thread::yield();	// for valgrind to switch threads
	}
	
};

} // test


int main() {
	
	vnl::Layer layer;
	
	vnl::spawn(new test::TroubleMaker("default", "test/trouble"));
	vnl::spawn(new test::TestNode("default", "test/node"));
	
	vnl::run(new vnl::Terminal());
	
}

