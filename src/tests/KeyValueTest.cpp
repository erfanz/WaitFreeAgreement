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

void KeyValueTest::test_constructor(){
	TestBase::printMessage(CLASS_NAME, __func__);

	Key k("k1");
	Value v("v1");
	Pointer p(9, 98, 987, 9876);
	KeyValue kv(k, v, p);

	assert(kv.getKey().isEqual(k) == true);
	assert(kv.getValue().isEqual(v) == true);
	assert(kv.getNextPointer().isEqual(p) == true);
}

void KeyValueTest::test_serialize() {
	TestBase::printMessage(CLASS_NAME, __func__);

	Key k("k1");
	Value v("v1");
	Pointer p(9, 98, 987, 9876);
	KeyValue kv(k, v, p);

	std::ostringstream os;
	std::string expected = "k1 v1 " + p.toString();
	kv.serialize(os);

	assert(os.str().compare(expected) == 0);
}
void KeyValueTest::test_deserialize() {
	TestBase::printMessage(CLASS_NAME, __func__);

	Key k("k1");
	Value v("v1");
	Pointer p(9, 98, 987, 9876);
	KeyValue kv(k, v, p);

	std::ostringstream os;
	kv.serialize(os);

	KeyValue newKv;
	std::istringstream is(os.str());
	KeyValue::deserialize(is, newKv);

	assert(newKv.getKey().isEqual(k));
	assert(newKv.getValue().isEqual(v));
	assert(newKv.getNextPointer().isEqual(p));


}
void KeyValueTest::test_toString() {

}





