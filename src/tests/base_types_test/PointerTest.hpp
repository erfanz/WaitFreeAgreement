/*
 *  PointerTest.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 22, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef POINTERTEST_HPP_
#define POINTERTEST_HPP_

#include "../TestBase.hpp"
#include "../../base_types/Pointer.hpp"
#include <functional>	// std::function
#include <vector>



class PointerTest : public TestBase{
private:
	static std::vector<std::function<void()>> functionList;
public:
	static std::vector<std::function<void()>>& getFunctionList();

	static void test_construct();
	static void test_serialize();
	static void test_deserialize();
	static void test_getTotalSize();
	static void test_toString();
};

#endif /* POINTERTEST_HPP_ */
