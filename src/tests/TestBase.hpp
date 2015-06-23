/*
 *  TestBase.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 22, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef TESTBASE_HPP_
#define TESTBASE_HPP_

#include <string>
#include <iostream>

class TestBase {
protected:
	 static void printMessage(std::string className, std::string functionName) {
		 std::cout << "Testing " << className << "::" << functionName << "() ... " << std::endl;
	 }
};


#endif /* TESTBASE_HPP_ */
