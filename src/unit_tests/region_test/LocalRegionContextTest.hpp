/*
 *  LocalRegionContextTest.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 26, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef LOCALREGIONCONTEXTTEST_HPP_
#define LOCALREGIONCONTEXTTEST_HPP_

#include "../../region/LocalRegionContext.hpp"
#include "../TestBase.hpp"
#include <functional>	// std::function
#include <vector>		// std::vector

class LocalRegionContextTest : public TestBase{
private:
	static std::vector<std::function<void()>> functionList;
public:
	static std::vector<std::function<void()>>& getFunctionList();
	static void test_read_and_write();
	static void test_read_and_write_with_logentry();
};

#endif /* LOCALREGIONCONTEXTTEST_HPP_ */
