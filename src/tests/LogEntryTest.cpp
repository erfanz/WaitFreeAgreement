/*
 *  LogEntryTest.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 18, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "LogEntryTest.hpp"
#include <assert.h>
#include <sstream>      // std::ostringstream, std::istringstream

#define CLASS_NAME	"LogEntryTest"

void LogEntryTest::test_constructor() {
	TestBase::printMessage(CLASS_NAME, __func__);

	uint64_t c = 0x1000000000000001;

	uint64_t a = 0x1011001000000011;
	uint64_t b = 0x0101000100000001;

	std::vector<KeyValue> kvList;

	Key k1("k101");
	Value v1("v101");

	Key k2("k202");
	Value v2("v202");

	Pointer pa = Pointer::makePointer(a);
	Pointer pb = Pointer::makePointer(b);
	Pointer pc = Pointer::makePointer(c);



	KeyValue kv1 (k1, v1, pa);
	KeyValue kv2 (k2, v2, pb);

	kvList.push_back(kv1);
	kvList.push_back(kv2);

	LogEntry entry(kvList, pc, false);

	assert(entry.getCurrentP().isEqual(pc) == true);
	assert(entry.isSerialized() == false);
	std::vector<KeyValue> kvs = entry.getKVs();
	assert(kvs.size() == kvList.size());
	for (size_t i = 0; i < kvs.size(); i++)
		assert(kvs.at(i).isEqual(kvList.at(i)) == true);
}


void LogEntryTest::test_serialize() {
	TestBase::printMessage(CLASS_NAME, __func__);

	uint64_t c = 0x1000000000000001;

	uint64_t a = 0x1011001000000011;
	uint64_t b = 0x0101000100000001;

	std::vector<KeyValue> kvList;

	Key k1("k101");
	Value v1("v101");

	Key k2("k202");
	Value v2("v202");

	KeyValue kv1 (k1, v1, Pointer::makePointer(a));
	KeyValue kv2 (k2, v2, Pointer::makePointer(b));

	kvList.push_back(kv1);
	kvList.push_back(kv2);

	LogEntry entry(kvList, Pointer::makePointer(c), false);

	std::string expected = entry.getCurrentP().toString() + " " + "0" + " " + "2" + " " + kv1.toString() + " " + kv2.toString() + " " + entry.getCurrentP().toString();

	std::ostringstream os;
	entry.serialize(os);

	assert(os.str().compare(expected) == 0);
}

void LogEntryTest::test_deserialize() {
	TestBase::printMessage(CLASS_NAME, __func__);

	uint64_t z = 0x1000000000000001;
	uint64_t a = 0x1011001000000011;
	uint64_t b = 0x0101000100000001;
	uint64_t c = 0x11f10d0100ccdf01;

	std::vector<KeyValue> kvList;

	Key k1("k101");
	Value v1("v101");

	Key k2("k202");
	Value v2("v202");

	Key k3("k303");
	Value v3("v303");

	KeyValue kv1 (k1, v1, Pointer::makePointer(a));
	KeyValue kv2 (k2, v2, Pointer::makePointer(b));
	KeyValue kv3 (k3, v3, Pointer::makePointer(c));

	kvList.push_back(kv1);
	kvList.push_back(kv2);
	kvList.push_back(kv3);

	LogEntry entry(kvList, Pointer::makePointer(z), false);

	std::ostringstream os;
	entry.serialize(os);

	LogEntry newEntry;
	std::istringstream is(os.str());
	LogEntry::deserialize(is, newEntry);

	assert(newEntry.isEqual(entry) == true);
}
