/*
 *  SingleTreadedExecutionTest.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jul 7, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef SINGLETREADEDEXECUTIONTEST_HPP_
#define SINGLETREADEDEXECUTIONTEST_HPP_

#include "../../region/LocalRegionContext.hpp"
#include "../TestBase.hpp"
#include <functional>	// std::function
#include <vector>		// std::vector

class SingleTreadedExecutionTest : public TestBase{
private:
	static std::vector<std::function<void()>> functionList;
	static void test_simple_put_and_get();
	static void test_read_non_existing_key();

	static void test_no_key_update();
	static void test_no_dependency();
	static void test_complex_entry_chain();

public:
	static std::vector<std::function<void()>>& getFunctionList();
	static void test_all();
};

#endif /* SINGLETREADEDEXECUTIONTEST_HPP_ */
