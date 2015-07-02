/*
 *  KeyValueTest.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 22, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "../../base_types/KeyValue.hpp"
#include "../TestBase.hpp"
#include <functional>	// std::function
#include <vector>

class KeyValueTest : public TestBase{
private:
	static std::vector<std::function<void()>> functionList;
public:
	static std::vector<std::function<void()>>& getFunctionList();

	static void test_constructor();
	static void test_serialize();
	static void test_deserialize();
	static void test_getTotalSize();
	static void test_toString();
};




