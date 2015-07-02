/*
 *  MemoryServerTest.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 29, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef MEMORYSERVERTEST_HPP_
#define MEMORYSERVERTEST_HPP_

#include "../../agents/memory_server/MemoryServer.hpp"
#include "../TestBase.hpp"
#include <functional>	// std::function
#include <vector>


class MemoryServerTest : public TestBase{
private:
	static std::vector<std::function<void()>> functionList;
public:
	static std::vector<std::function<void()>>& getFunctionList();
	static void test_getMemoryHandler();
};

#endif /* MEMORYSERVERTEST_HPP_ */
