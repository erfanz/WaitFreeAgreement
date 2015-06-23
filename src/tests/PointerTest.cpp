/*
 *  Pointer_test.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 12, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "PointerTest.hpp"
#include "TestBase.hpp"
#include "../base_types/Pointer.hpp"
#include <sstream>      // std::ostringstream, std::istringstream
#include <assert.h>

#define CLASS_NAME	"PointerTest"

void PointerTest::test_construct() {
	TestBase::printMessage(CLASS_NAME, __func__);

	uint64_t input = 0x1001011000000001;
	Pointer p = Pointer::makePointer(input);
	assert (p.getCoordinatorNum() == 0x10);
	assert (p.getGenerationNum() == 0x01);
	assert (p.getLength() == 0x0110);
	assert (p.getOffset() == 0x00000001);

	Pointer p2(12, 32, 117, 18098) ;
	assert (p2.getCoordinatorNum() == 12);
	assert (p2.getGenerationNum() == 32);
	assert (p2.getLength() == 117);
	assert (p2.getOffset() == 18098);
}

void PointerTest::test_serialize(){
	TestBase::printMessage(CLASS_NAME, __func__);

	uint64_t input = 0x1001011000000001;
	Pointer p = Pointer::makePointer(input);
	std::ostringstream os;
	std::string expected = "0001000000000001000000010001000000000000000000000000000000000001";
	p.serialize(os);
	assert(os.str().compare(expected) == 0);
}

void PointerTest::test_deserialize(){
	TestBase::printMessage(CLASS_NAME, __func__);

	Pointer p(12, 32, 117, 18098) ;
	std::ostringstream os;
	p.serialize(os);

	Pointer p2;
	std::istringstream is(os.str());
	Pointer::deserialize(is, p2);

	assert(p.getCoordinatorNum() == p2.getCoordinatorNum());
	assert(p.getGenerationNum() == p2.getGenerationNum());
	assert(p.getLength() == p2.getLength());
	assert(p.getOffset() == p2.getOffset());
}

void PointerTest::test_toString(){
	TestBase::printMessage(CLASS_NAME, __func__);

	uint64_t input = 0x1001011000000001;
	Pointer p = Pointer::makePointer(input);
	std::string expectedResult = "0001000000000001000000010001000000000000000000000000000000000001";
	assert(p.toString().compare(expectedResult) == 0);
}
