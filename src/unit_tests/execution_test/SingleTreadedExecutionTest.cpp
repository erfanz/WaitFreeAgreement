/*
 *  SingleTreadedExecutionTest.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jul 7, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "SingleTreadedExecutionTest.hpp"
#include "../agents_test/AgentsHandler.hpp"
#include "../../agents/coordinator/Coordinator.hpp"
#include "../../agents/coordinator/MemoryServerContext.hpp"
#include "../../agents/memory_server/MemoryServer.hpp"
#include "../../base_types/PrimitiveTypes.hpp"
#include "../../config.hpp"
#include <map>
#include <assert.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#define CLASS_NAME	"SinTrdExecTest"


std::vector<std::function<void()>> SingleTreadedExecutionTest::functionList {
	test_all
};

std::vector<std::function<void()>>& SingleTreadedExecutionTest::getFunctionList() {
	return functionList;
}

void SingleTreadedExecutionTest::test_all() {
	TestBase::printMessage(CLASS_NAME, __func__);
	agents_handler::setup();
	srand (time(NULL));

	test_simple_put_and_get();
	test_read_non_existing_key();
	test_no_key_update();
	test_no_dependency();
	test_complex_entry_chain();
}

void SingleTreadedExecutionTest::test_simple_put_and_get() {
	TestBase::printMessage(CLASS_NAME, __func__);
	agents_handler::resetMemoryServers();

	Change* change;
	Pointer newEntryPointer;
	std::vector<std::string> updateKeys;
	std::vector<std::string> pureDependencies;
	TID tid;
	tid.id = 12;

	updateKeys = {"k1", "k2"};
	pureDependencies = {"k3"};
	agents_handler::constructChange(&change, updateKeys, pureDependencies);
	error::ErrorType eType = agents_handler::coordinators.at(rand() % config::COORDINATOR_CNT)->applyChange(*change, tid, newEntryPointer);
	agents_handler::updateBucketInfo(updateKeys, pureDependencies, newEntryPointer);
	assert(eType == error::SUCCESS);

	// let's see if the change is visible
	std::vector<std::string> readKeys = {"k1", "k2"};
	std::vector<int> expectedDepth = {1, 1};

	Value value;
	SCN scn(1);
	int searchDepth;

	for (size_t i = 0; i < readKeys.size(); i++) {
		for (size_t c = 0; c < config::COORDINATOR_CNT; c++) {
			Key key(readKeys.at(i));
			error::ErrorType eType = agents_handler::coordinators.at(c)->readByKey(key, scn, tid, value, searchDepth);
			assert(eType == error::SUCCESS && value.isEqual(agents_handler::keyToValueMap[readKeys.at(i)]));
		}
	}
}

void SingleTreadedExecutionTest::test_read_non_existing_key() {
	TestBase::printMessage(CLASS_NAME, __func__);
	agents_handler::resetMemoryServers();

	Key k1("k1");
	Value value;
	SCN scn(1);
	TID tid;
	tid.id = 12;
	int searchDepth;

	error::ErrorType eType = agents_handler::coordinators.at(0)->readByKey(k1, scn, tid, value, searchDepth);
	assert(eType == error::KEY_NOT_FOUND);
}

void SingleTreadedExecutionTest::test_no_key_update() {
	TestBase::printMessage(CLASS_NAME, __func__);
	agents_handler::resetMemoryServers();

	Change* change;
	Pointer newEntryPointer;
	std::vector<std::string> updateKeys;
	std::vector<std::string> pureDependencies;
	TID tid;
	tid.id = 12;

	updateKeys = {};
	pureDependencies = {"k1"};
	agents_handler::constructChange(&change, updateKeys, pureDependencies);
	ErrorType eType = agents_handler::coordinators.at(0)->applyChange(*change, tid, newEntryPointer);
	assert(eType == error::NO_UPDATE_KEY_IN_CHANGE);
}

void SingleTreadedExecutionTest::test_no_dependency() {
	TestBase::printMessage(CLASS_NAME, __func__);
	agents_handler::resetMemoryServers();

	Change* change;
	Pointer newEntryPointer;
	std::vector<std::string> updateKeys;
	std::vector<std::string> pureDependencies;
	TID tid;
	tid.id = 12;

	updateKeys = {"k1", "k2", "k3", "k4"};
	pureDependencies = {};
	agents_handler::constructChange(&change, updateKeys, pureDependencies);
	ErrorType eType = agents_handler::coordinators.at(0)->applyChange(*change, tid, newEntryPointer);
	agents_handler::updateBucketInfo(updateKeys, pureDependencies, newEntryPointer);
	assert(eType == error::SUCCESS);

	// let's see if the change is visible
	std::vector<std::string> readKeys = {"k1", "k2", "k3", "k4"};
	std::vector<int> expectedDepth = {1, 1, 1, 1};

	Value value;
	SCN scn(1);
	int searchDepth;

	for (size_t i = 0; i < readKeys.size(); i++) {
		for (size_t c = 0; c < config::COORDINATOR_CNT; c++) {
			Key key(readKeys.at(i));
			error::ErrorType eType = agents_handler::coordinators.at(c)->readByKey(key, scn, tid, value, searchDepth);
			assert(eType == error::SUCCESS && value.isEqual(agents_handler::keyToValueMap[readKeys.at(i)]));
		}
	}
}

void SingleTreadedExecutionTest::test_complex_entry_chain() {
	TestBase::printMessage(CLASS_NAME, __func__);
	agents_handler::resetMemoryServers();

	/*
	 *             k2
	 *            /   \
	 *   k5 --> k4     k1
	 *            \   /
	 *             k3
	 */

	Change* change;
	Pointer newEntryPointer;
	std::vector<std::string> updateKeys;
	std::vector<std::string> pureDependencies;
	TID tid;
	tid.id = 12;

	// first insert k1
	updateKeys = {"k1"};
	pureDependencies = {};
	agents_handler::constructChange(&change, updateKeys, pureDependencies);
	ErrorType eType = agents_handler::coordinators.at(rand() % config::COORDINATOR_CNT)->applyChange(*change, tid, newEntryPointer);
	agents_handler::updateBucketInfo(updateKeys, pureDependencies, newEntryPointer);
	assert(eType == error::SUCCESS);

	// insert k2, which depends on k1
	updateKeys = {"k2"};
	pureDependencies = {"k1"};
	agents_handler::constructChange(&change, updateKeys, pureDependencies);
	eType = agents_handler::coordinators.at(rand() % config::COORDINATOR_CNT)->applyChange(*change, tid, newEntryPointer);
	agents_handler::updateBucketInfo(updateKeys, pureDependencies, newEntryPointer);
	assert(eType == error::SUCCESS);

	// insert k3, which depends on k1
	updateKeys = {"k3"};
	pureDependencies = {"k1"};
	agents_handler::constructChange(&change, updateKeys, pureDependencies);
	eType = agents_handler::coordinators.at(rand() % config::COORDINATOR_CNT)->applyChange(*change, tid, newEntryPointer);
	agents_handler::updateBucketInfo(updateKeys, pureDependencies, newEntryPointer);
	assert(eType == error::SUCCESS);

	// insert k4, which depends on k2 and k3
	updateKeys = {"k4"};
	pureDependencies = {"k2", "k3"};
	agents_handler::constructChange(&change, updateKeys, pureDependencies);
	eType = agents_handler::coordinators.at(rand() % config::COORDINATOR_CNT)->applyChange(*change, tid, newEntryPointer);
	agents_handler::updateBucketInfo(updateKeys, pureDependencies, newEntryPointer);
	assert(eType == error::SUCCESS);

	// insert k5, which depends on k4
	updateKeys = {"k5"};
	pureDependencies = {"k4"};
	agents_handler::constructChange(&change, updateKeys, pureDependencies);
	eType = agents_handler::coordinators.at(rand() % config::COORDINATOR_CNT)->applyChange(*change, tid, newEntryPointer);
	agents_handler::updateBucketInfo(updateKeys, pureDependencies, newEntryPointer);
	assert(eType == error::SUCCESS);


	// let's see if the change is visible
	std::vector<std::string> readKeys = {"k1", "k2", "k3", "k4", "k5"};
	std::vector<int> expectedDepth = {4, 3, 3, 2, 1};

	Value value;
	SCN scn(1);
	int searchDepth;

	for (size_t i = 0; i < readKeys.size(); i++) {
		for (size_t c = 0; c < config::COORDINATOR_CNT; c++) {
			Key key(readKeys.at(i));
			error::ErrorType eType = agents_handler::coordinators.at(c)->readByKey(key, scn, tid, value, searchDepth);
			assert(eType == error::SUCCESS && value.isEqual(agents_handler::keyToValueMap[readKeys.at(i)]));
		}
	}
}
