/*
 *  DependencyTest.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 24, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef DEPENDENCYTEST_HPP_
#define DEPENDENCYTEST_HPP_

#include "TestBase.hpp"
#include "../base_types/Dependency.hpp"

class DependencyTest : public TestBase{
public:
	static void test_constructor();
	static void test_serialize();
	static void test_deserialize();
	static void test_toString();
};

#endif /* DEPENDENCYTEST_HPP_ */
