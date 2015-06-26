/*
 *  testMain.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 22, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "LogEntryTest.hpp"
#include "PointerTest.hpp"
#include "KeyValueTest.hpp"
#include "DependencyTest.hpp"



int main() {
	PointerTest::test_construct();
	PointerTest::test_serialize();
	PointerTest::test_deserialize();
	PointerTest::test_getTotalSize();
	PointerTest::test_toString();

	KeyValueTest::test_constructor();
	KeyValueTest::test_serialize();
	KeyValueTest::test_deserialize();
	KeyValueTest::test_getTotalSize();
	KeyValueTest::test_toString();

	DependencyTest::test_constructor();
	DependencyTest::test_serialize();
	DependencyTest::test_deserialize();
	DependencyTest::test_toString();

	LogEntryTest::test_constructor();
	LogEntryTest::test_serialize();
	LogEntryTest::test_deserialize();
	LogEntryTest::test_calculateEntrySize();

	std::cout << std::endl << "All tests passed!" << std::endl;
	return 0;
}


