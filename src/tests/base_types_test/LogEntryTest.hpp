/*
 *  LogEntryTest.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 22, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef LOGENTRYTEST_HPP_
#define LOGENTRYTEST_HPP_

#include "../TestBase.hpp"
#include "../../base_types/LogEntry.hpp"
#include <functional>	// std::function
#include <vector>


class LogEntryTest : public TestBase{
private:
	static std::vector<std::function<void()>> functionList;
public:
	static std::vector<std::function<void()>>& getFunctionList();
	static void test_constructor();
	static void test_serialize();
	static void test_deserialize();
	static void test_calculateEntrySize();
};

#endif /* LOGENTRYTEST_HPP_ */
