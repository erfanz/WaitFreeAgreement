/*
 *  CoordinatorTest.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jul 14, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef COORDINATORTEST_HPP_
#define COORDINATORTEST_HPP_

#include "../../region/LocalRegionContext.hpp"
#include "../../agents/coordinator/Coordinator.hpp"
#include "../../agents/coordinator/MemoryServerContext.hpp"
#include "../TestBase.hpp"
#include <functional>	// std::function
#include <vector>		// std::vector


class Coordinator;  		// it's needed so that CoordinatorTest becomes a friend class of Coordinator
class MemoryServerContext;	// it's needed so that CoordinatorTest becomes a friend class of MemoryServerContext

class CoordinatorTest : public TestBase{
private:
	static std::vector<std::function<void()>> functionList;
	static void test_check_serialized_when_all_serialized();
	static void test_check_serialized_when_some_serialized();
	static void test_check_serialized_when_none_serialized();
//	static void test_read_non_existing_key();
//
//	static void test_no_key_update();
//	static void test_no_dependency();
//	static void test_complex_entry_chain();

public:
	static std::vector<std::function<void()>>& getFunctionList();
	static void test_all();
};


#endif /* COORDINATORTEST_HPP_ */
