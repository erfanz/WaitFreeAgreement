/*
 *  KeyValueTest.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 22, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "KeyValueTest.hpp"
#include <assert.h>
#include <sstream>      // std::ostringstream, std::istringstream


#define CLASS_NAME	"KeyValueTest"

std::vector<std::function<void()>> KeyValueTest::functionList {
	test_constructor,
	test_serialize,
	test_deserialize,
	test_getTotalSize,
	test_toString
};

std::vector<std::function<void()>>& KeyValueTest::getFunctionList() {
	//return PointerTest::functionList;
	return functionList;
}

void KeyValueTest::test_constructor(){
	TestBase::printMessage(CLASS_NAME, __func__);

	Key k("k1");
	Value v("v1");
	KeyValue kv(k, v);

	assert(kv.getKey().isEqual(k) == true);
	assert(kv.getValue().isEqual(v) == true);
}

void KeyValueTest::test_serialize() {
	TestBase::printMessage(CLASS_NAME, __func__);

	Key k("k1");
	Value v("v1");
	KeyValue kv(k, v);

	std::ostringstream os;
	std::string expected = "k1 v1";
	kv.serialize(os);

	assert(os.str().compare(expected) == 0);
}

void KeyValueTest::test_deserialize() {
	TestBase::printMessage(CLASS_NAME, __func__);

	Key k("k1");
	Value v("v1");
	KeyValue kv(k, v);

	std::ostringstream os;
	kv.serialize(os);

	KeyValue newKv;
	std::istringstream is(os.str());
	KeyValue::deserialize(is, newKv);

	assert(newKv.getKey().isEqual(k));
	assert(newKv.getValue().isEqual(v));
}

void KeyValueTest::test_getTotalSize() {
	TestBase::printMessage(CLASS_NAME, __func__);

	Key k("key12");
	Value v("value100987");
	KeyValue kv(k, v);

	std::ostringstream os;
	kv.serialize(os);

	assert(kv.getTotalSize() == os.str().size());



}

void KeyValueTest::test_toString() {

}
