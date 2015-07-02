/*
 *  DependencyTest.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 24, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "DependencyTest.hpp"
#include <assert.h>
#include <sstream>      // std::ostringstream, std::istringstream

#define CLASS_NAME	"DependencyTest"

std::vector<std::function<void()>> DependencyTest::functionList {
	test_constructor,
	test_serialize,
	test_deserialize,
	test_toString
};

std::vector<std::function<void()>>& DependencyTest::getFunctionList() {
	//return PointerTest::functionList;
	return functionList;
}

void DependencyTest::test_constructor() {
	TestBase::printMessage(CLASS_NAME, __func__);

	size_t bucketID = 10;
	Pointer p = Pointer::makePointer(0x1000000000000001);

	Dependency d(bucketID, p);
	assert(d.getBucketID() == bucketID);
	assert(d.getPointer().isEqual(p));
}

void DependencyTest::test_serialize() {
	TestBase::printMessage(CLASS_NAME, __func__);

	size_t bucketID = 10;
	Pointer p = Pointer::makePointer(0x1abcdef001102201);

	Dependency d(bucketID, p);

	std::string expected = std::string("10") + " " + p.toString();


	std::ostringstream os;
	d.serialize(os);

	assert(os.str().compare(expected) == 0);
}

void DependencyTest::test_deserialize() {
	TestBase::printMessage(CLASS_NAME, __func__);

	size_t bucketID = 10;
	Pointer p = Pointer::makePointer(0x1abcdef001102201);

	Dependency d(bucketID, p);

	std::ostringstream os;
	d.serialize(os);

	Dependency newD;
	std::istringstream is(os.str());
	Dependency::deserialize(is, newD);

	assert(newD.getBucketID() == d.getBucketID());
	assert(newD.getPointer().isEqual(d.getPointer()));
}

void DependencyTest::test_toString() {
	TestBase::printMessage(CLASS_NAME, __func__);

	size_t bucketID = 10;
	Pointer p = Pointer::makePointer(0x1abcdef001102201);

	Dependency d(bucketID, p);

	std::string output = d.toString();

	std::string expected = std::string("10") + " " + p.toString();

	assert(output.compare(expected) == 0);
}
