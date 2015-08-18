/*
 *  CompleteTest.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 17, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef COMPLETETEST_HPP_
#define COMPLETETEST_HPP_


#include "../TestBase.hpp"
#include <functional>	// std::function
#include <vector>		// std::vector

class CompleteTest: public TestBase{
private:
	static std::vector<std::function<void()>> functionList;
//	static void test_check_serialized_when_all_serialized();
//	static void test_check_serialized_when_some_serialized();
//	static void test_check_serialized_when_none_serialized();
//	static void test_resolve_conflict_case1();

public:
	static std::vector<std::function<void()>>& getFunctionList();
	static void test_all();
};

#endif /* COMPLETETEST_HPP_ */
