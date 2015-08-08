/*
 *  CoordinatorTest.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jul 14, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "CoordinatorTest.hpp"
#include "AgentsHandler.hpp"
#include "../../agents/coordinator/Coordinator.hpp"
#include "../../agents/coordinator/MemoryServerContext.hpp"
#include "../../agents/memory_server/MemoryServer.hpp"
#include "../../base_types/PrimitiveTypes.hpp"
#include "../../config.hpp"
#include <map>
#include <assert.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <future>         // std::promise, std::future



#define CLASS_NAME	"CoordinatorTest"


std::vector<std::function<void()>> CoordinatorTest::functionList {
	test_all
};

std::vector<std::function<void()>>& CoordinatorTest::getFunctionList() {
	return functionList;
}

void CoordinatorTest::test_all() {
	TestBase::printMessage(CLASS_NAME, __func__);
	agents_handler::setup();
	srand ((unsigned int)time(NULL));

	test_check_serialized_when_all_serialized();
	//test_check_serialized_when_some_serialized();
	// test_check_serialized_when_none_serialized();
	//test_resolve_conflict_case1();
}

void CoordinatorTest::test_check_serialized_when_all_serialized() {
	TestBase::printMessage(CLASS_NAME, __func__);
	agents_handler::resetMemoryServers();

	LogEntry headEntry;
	LogEntry *newEntry1, *newEntry2;
	Value returnValue;
	ErrorType eType;

	Change* change;
	TID tid;
	std::vector<std::string> updateKeys;
	std::vector<std::string> pureDependencies;

	// First insert k1
	updateKeys = {"k1"};
	pureDependencies = {};
	agents_handler::constructChange(&change, updateKeys, pureDependencies);
	eType = agents_handler::coordinators.at(0)->applyChange(*change, tid, &newEntry1);
	agents_handler::updateBucketInfo(updateKeys, pureDependencies, newEntry1->getCurrentP());
	assert(eType == error::SUCCESS);

	// Then insert k2, which depends on k1
	updateKeys = {"k2"};
	pureDependencies = {"k1"};
	agents_handler::constructChange(&change, updateKeys, pureDependencies);
	eType = agents_handler::coordinators.at(0)->applyChange(*change, tid, &newEntry2);
	agents_handler::updateBucketInfo(updateKeys, pureDependencies, newEntry2->getCurrentP());
	assert(eType == error::SUCCESS);


	const Key k1("k1");
	eType = agents_handler::coordinators.at(config::COORDINATOR_CNT - 1)->readLatest(k1, returnValue, headEntry);
	assert(eType == error::SUCCESS);
	assert(returnValue.getContent().compare(agents_handler::keyToValueMap.find(k1.getId())->second) == 0);
	//assert(entryPointer.isEqual(headEntry.getCurrentP()));

	const Key k2("k2");
	eType = agents_handler::coordinators.at(config::COORDINATOR_CNT - 1)->readLatest(k2, returnValue, headEntry);
	assert(eType == error::SUCCESS);
	assert(returnValue.getContent().compare(agents_handler::keyToValueMap.find(k2.getId())->second) == 0);
	//assert(entryPointer.isEqual(headEntry.getCurrentP()));

	delete newEntry1;
	delete newEntry2;
}

void CoordinatorTest::test_check_serialized_when_some_serialized() {
	TestBase::printMessage(CLASS_NAME, __func__);
	agents_handler::resetMemoryServers();

	std::promise<ErrorType> errorProm;
	std::future<ErrorType> errorFut = errorProm.get_future();

	LogEntry *newEntry1, *newEntry2;
	LogEntry headEntry;
	Pointer *entryPointer = new Pointer();
	Value returnValue;
	ErrorType eType;

	Change* change;
	TID tid;
	std::vector<std::string> updateKeys;
	std::vector<std::string> pureDependencies;

	// First insert k1
	updateKeys = {"k1"};
	pureDependencies = {};
	agents_handler::constructChange(&change, updateKeys, pureDependencies);
	eType = agents_handler::coordinators.at(0)->applyChange(*change, tid, &newEntry1);
	agents_handler::updateBucketInfo(updateKeys, pureDependencies, newEntry1->getCurrentP());
	assert(eType == error::SUCCESS);

	// then insert k2, which depends on k1.
	// here, we don't use coordinator.applyChange(), since we want to partially mark the entries serialized
	updateKeys = {"k2"};
	pureDependencies = {"k1"};
	agents_handler::constructChange(&change, updateKeys, pureDependencies);

	agents_handler::coordinators.at(0)->createNewPointer(*change, &entryPointer);
	agents_handler::coordinators.at(0)->makeNewLogEntry(*change, *entryPointer, &newEntry2);
	agents_handler::coordinators.at(0)->propagateLogEntry(*newEntry2);
	agents_handler::coordinators.at(0)->publishChanges(*newEntry2);
	// instead of agents_handler::coordinators.at(0)->makeSerialized(*entry), we have:
	agents_handler::coordinators.at(0)->memoryServerCtxs_.at(0).markState(*newEntry2, EntryState::SERIALIZED, errorProm);
	eType = errorFut.get();
	assert(eType == error::SUCCESS);

	agents_handler::updateBucketInfo(updateKeys, pureDependencies, *entryPointer);

	const Key k1("k1");
	eType = agents_handler::coordinators.at(config::COORDINATOR_CNT - 1)->readLatest(k1, returnValue, headEntry);
	assert(eType == error::SUCCESS);
	assert(returnValue.getContent().compare(agents_handler::keyToValueMap.find(k1.getId())->second) == 0);
	assert(entryPointer->isEqual(headEntry.getCurrentP()));

	const Key k2("k2");
	eType = agents_handler::coordinators.at(config::COORDINATOR_CNT - 1)->readLatest(k2, returnValue, headEntry);
	assert(eType == error::SUCCESS);
	assert(returnValue.getContent().compare(agents_handler::keyToValueMap.find(k2.getId())->second) == 0);
	assert(entryPointer->isEqual(headEntry.getCurrentP()));

	delete newEntry1;
	delete newEntry2;
}

void CoordinatorTest::test_check_serialized_when_none_serialized() {
	TestBase::printMessage(CLASS_NAME, __func__);
	agents_handler::resetMemoryServers();

	LogEntry *entry = NULL;
	LogEntry headEntry;
	Pointer *entryPointer = new Pointer();
	Value returnValue;
	ErrorType eType;

	Change* change;
	std::vector<std::string> updateKeys;
	std::vector<std::string> pureDependencies;

	updateKeys = {"k1"};
	pureDependencies = {};
	agents_handler::constructChange(&change, updateKeys, pureDependencies);

	agents_handler::coordinators.at(0)->createNewPointer(*change, &entryPointer);
	agents_handler::coordinators.at(0)->makeNewLogEntry(*change, *entryPointer, &entry);
	agents_handler::coordinators.at(0)->propagateLogEntry(*entry);
	agents_handler::coordinators.at(0)->publishChanges(*entry);
	// note that we don't execute	agents_handler::coordinators.at(0)->makeSerialized(*entry), we have:

	agents_handler::updateBucketInfo(updateKeys, pureDependencies, *entryPointer);

	const Key k1("k1");
	eType = agents_handler::coordinators.at(config::COORDINATOR_CNT - 1)->readLatest(k1, returnValue, headEntry);
	assert(eType == error::SUCCESS);
	assert(returnValue.getContent().compare(agents_handler::keyToValueMap.find(k1.getId())->second) == 0);
	assert(entryPointer->isEqual(headEntry.getCurrentP()));
}



void CoordinatorTest::test_resolve_conflict_case1() {
	TestBase::printMessage(CLASS_NAME, __func__);
	agents_handler::resetMemoryServers();

	assert(config::COORDINATOR_CNT == 2);

	LogEntry *entry = NULL;
	Pointer *entryPointer = new Pointer();
	Value returnValue;
	Pointer pointerToEntry;
	LogEntry headEntry;

	Change* change;
	std::vector<std::string> updateKeys;
	std::vector<std::string> pureDependencies;

	std::promise<ErrorType>	errorProm1, errorProm2;
	std::future<ErrorType>	errorFut1, errorFut2;
	errorFut1 = errorProm1.get_future();
	errorFut2 = errorProm2.get_future();

	Pointer actualHead;


	updateKeys = {"k1"};
	pureDependencies = {};
	agents_handler::constructChange(&change, updateKeys, pureDependencies);
	agents_handler::coordinators.at(0)->createNewPointer(*change, &entryPointer);
	agents_handler::coordinators.at(0)->makeNewLogEntry(*change, *entryPointer, &entry);
	agents_handler::coordinators.at(0)->propagateLogEntry(*entry);

	agents_handler::coordinators.at(0)->memoryServerCtxs_.at(0).swapBucketHash(
			entry->getDependencies()[0].getBucketID(), entry->getDependencies()[0].getPointer(), entry->getCurrentP(), errorProm1, actualHead);

	if (errorFut1.get() != error::SUCCESS) {
		DEBUG_COUT(CLASS_NAME, __func__, "CASing entry " << entry->getCurrentP().toHexString() << " failed");
	}

	updateKeys = {"k2"};
	pureDependencies = {};
	agents_handler::constructChange(&change, updateKeys, pureDependencies);
	agents_handler::coordinators.at(1)->createNewPointer(*change, &entryPointer);
	agents_handler::coordinators.at(1)->makeNewLogEntry(*change, *entryPointer, &entry);
	agents_handler::coordinators.at(1)->propagateLogEntry(*entry);

	agents_handler::coordinators.at(1)->memoryServerCtxs_.at(1).swapBucketHash(
			entry->getDependencies()[0].getBucketID(), entry->getDependencies()[0].getPointer(), entry->getCurrentP(), errorProm2, actualHead);

	if (errorFut2.get() != error::SUCCESS) {
		DEBUG_COUT(CLASS_NAME, __func__, "CASing entry " << entry->getCurrentP().toHexString() << " failed");
	}

	agents_handler::coordinators.at(1)->resolve(0, headEntry);
}
