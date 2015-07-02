/*
 *  LogEntryTest.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 18, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "LogEntryTest.hpp"
#include "../../base_types/PrimitiveTypes.hpp"	// primitive::entry_size_t
#include <assert.h>
#include <sstream>      // std::ostringstream, std::istringstream

#define CLASS_NAME	"LogEntryTest"

std::vector<std::function<void()>> LogEntryTest::functionList {
	test_constructor,
	test_serialize,
	test_deserialize,
	test_calculateEntrySize,
	test_findKey
};


LogEntry* LogEntryTest::makeLogEntry() {
	std::vector<Dependency> dependencies;

	int b1 = 10;
	Pointer p1 = Pointer::makePointer(0x1011001000000011);
	Dependency dep1(b1, p1);

	int b2 = 11;
	Pointer p2 = Pointer::makePointer(0x0101000100000001);
	Dependency dep2(b2, p2);

	dependencies.push_back(dep1);
	dependencies.push_back(dep2);

	std::vector<KeyValue> kvList;

	Key k1("k101");
	Value v1("v101");

	Key k2("k202");
	Value v2("v202");

	Key k3("k303");
	Value v3("v303");

	KeyValue kv1 (k1, v1);
	KeyValue kv2 (k2, v2);
	KeyValue kv3 (k3, v3);

	kvList.push_back(kv1);
	kvList.push_back(kv2);
	kvList.push_back(kv3);

	primitive::entry_size_t 		entrySize	= LogEntry::calculateEntrySize(dependencies, kvList);
	primitive::coordinator_num_t	cID			= 2;
	primitive::generation_num_t		gNum		= 12;
	primitive::offset_t				offset		= 12;

	Pointer pointer(cID, gNum, entrySize, offset);

	return new LogEntry(dependencies, kvList, pointer, false);
}

std::vector<std::function<void()>>& LogEntryTest::getFunctionList() {
	//return PointerTest::functionList;
	return functionList;
}

void LogEntryTest::test_constructor() {
	TestBase::printMessage(CLASS_NAME, __func__);

	std::vector<Dependency> dependencies;

	int b1 = 10;
	Pointer p1 = Pointer::makePointer(0x1011001000000011);
	Dependency dep1(b1, p1);

	int b2 = 11;
	Pointer p2 = Pointer::makePointer(0x0101000100000001);
	Dependency dep2(b2, p2);

	dependencies.push_back(dep1);
	dependencies.push_back(dep2);

	std::vector<KeyValue> updates;

	Key k1("k101");
	Value v1("v101");

	Key k2("k202");
	Value v2("v202");

	KeyValue kv1 (k1, v1);
	KeyValue kv2 (k2, v2);

	updates.push_back(kv1);
	updates.push_back(kv2);

	Pointer pc = Pointer::makePointer(0x1000000000000001);

	LogEntry entry(dependencies, updates, pc, false);

	assert(entry.getCurrentP().isEqual(pc) == true);
	assert(entry.isSerialized() == false);

	std::vector<Dependency> depList = entry.getDependencies();
	assert(depList.size() == dependencies.size());
	for (size_t i = 0; i < depList.size(); i++)
		assert(depList.at(i).isEqual(dependencies.at(i)) == true);

	std::vector<KeyValue> newUpdates = entry.getUpdates();
	assert(newUpdates.size() == updates.size());
	for (size_t i = 0; i < newUpdates.size(); i++)
		assert(newUpdates.at(i).isEqual(updates.at(i)) == true);
}

void LogEntryTest::test_serialize() {
	TestBase::printMessage(CLASS_NAME, __func__);

	std::vector<Dependency> dependencies;

	int b1 = 10;
	Pointer p1 = Pointer::makePointer(0x1011001000000011);
	Dependency dep1(b1, p1);

	int b2 = 11;
	Pointer p2 = Pointer::makePointer(0x0101000100000001);
	Dependency dep2(b2, p2);

	dependencies.push_back(dep1);
	dependencies.push_back(dep2);

	std::vector<KeyValue> kvList;

	Key k1("k101");
	Value v1("v101");

	Key k2("k202");
	Value v2("v202");

	KeyValue kv1 (k1, v1);
	KeyValue kv2 (k2, v2);

	kvList.push_back(kv1);
	kvList.push_back(kv2);


	LogEntry entry(dependencies, kvList, Pointer::makePointer(0x1000000000000001), false);

	std::string expected = entry.getCurrentP().toString() + " "
			+ "0" + " "		// isSerialized
			+ "2" + " "		// depNum
			+ "10" + " "
			+ "11" + " "
			+ p1.toString() + " "
			+ p2.toString() + " "
			+ "2" + " "		// kvNum
			+ kv1.toString() + " "
			+ kv2.toString() + " "
			+ entry.getCurrentP().toString();

	std::ostringstream os;
	entry.serialize(os);
	assert(os.str().compare(expected) == 0);
}

void LogEntryTest::test_deserialize() {
	TestBase::printMessage(CLASS_NAME, __func__);

	LogEntry *entry = makeLogEntry();

	std::ostringstream os;

	entry->serialize(os);


	LogEntry newEntry;
	std::istringstream is(os.str());
	LogEntry::deserialize(is, newEntry);

	assert(newEntry.isEqual(*entry) == true);
}

void LogEntryTest::test_calculateEntrySize() {
	TestBase::printMessage(CLASS_NAME, __func__);

	LogEntry *entry = makeLogEntry();


	std::ostringstream os;
	entry->serialize(os);
	assert(LogEntry::calculateEntrySize(entry->getDependencies(), entry->getUpdates()) == primitive::entry_size_t(os.str().size()));
}

void LogEntryTest::test_findKey() {
	TestBase::printMessage(CLASS_NAME, __func__);

	LogEntry *entry = makeLogEntry();


	Pointer p;
	assert(entry->getDependencyIfExists(9, p) == false);

	assert(entry->getDependencyIfExists(10, p) == true);
	assert(p.isEqual(Pointer::makePointer(0x1011001000000011)) == true);

	assert(entry->getDependencyIfExists(11, p) == true);
	assert(p.isEqual(Pointer::makePointer(0x0101000100000001)) == true);


	Key k("k_doesnt_exist");
	Value v;
	assert(entry->getUpdateIfExists(k, v) == false);

	k.setId("k101");
	Value expected("v101");
	assert(entry->getUpdateIfExists(k, v) == true);
	assert(v.isEqual(expected) == true);

	k.setId("k202");
	expected.setContent("v202");
	assert(entry->getUpdateIfExists(k, v) == true);
	assert(v.isEqual(expected) == true);
}
