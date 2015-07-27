/*
 *  testMain.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 22, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "base_types_test/LogEntryTest.hpp"
#include "base_types_test/PointerTest.hpp"
#include "base_types_test/KeyValueTest.hpp"
#include "base_types_test/DependencyTest.hpp"
#include "region_test/LocalRegionContextTest.hpp"
//#include "execution_test/SingleTreadedExecutionTest.hpp"
#include "agents_test/CoordinatorTest.hpp"


#include <functional>	// std::function
#include <vector>

template <typename T>
void appendVector(std::vector<T> &mainVec, const std::vector<T> &toBeAppendedVec) {
	mainVec.insert(mainVec.end(), toBeAppendedVec.begin(), toBeAppendedVec.end());
}

int main() {
	std::vector<std::function<void()>> allTestFunctions;
	appendVector(allTestFunctions, PointerTest::getFunctionList());
	appendVector(allTestFunctions, KeyValueTest::getFunctionList());
	appendVector(allTestFunctions, DependencyTest::getFunctionList());
	appendVector(allTestFunctions, LogEntryTest::getFunctionList());
	appendVector(allTestFunctions, LocalRegionContextTest::getFunctionList());
	//appendVector(allTestFunctions, SingleTreadedExecutionTest::getFunctionList());
	appendVector(allTestFunctions, CoordinatorTest::getFunctionList());

	for (std::size_t i = 0; i < allTestFunctions.size(); i++){
		allTestFunctions.at(i)();
	}

	std::cout << std::endl << "All tests passed!" << std::endl;
	return 0;
}
