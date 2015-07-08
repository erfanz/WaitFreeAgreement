/*
 *  SingleTreadedExecutionTest.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jul 7, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "SingleTreadedExecutionTest.hpp"
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

std::vector<MemoryServer*> memoryServers;
std::vector<Coordinator*> coordinators;

std::map<size_t,uint64_t> bucketToPointerMap;	// stores where each bucket points to during the execution of the test.
std::map<std::string,std::string> keyToValueMap;	// stores the value associated with any given key.
std::map<std::string,size_t> keyToBucketMap;	// stores the bucketID associated with any given key.


std::vector<std::function<void()>> SingleTreadedExecutionTest::functionList {
	test_all
};

std::vector<std::function<void()>>& SingleTreadedExecutionTest::getFunctionList() {
	return functionList;
}

void createMemoryServers(std::vector<MemoryServer*> &memoryServers){
	for (size_t i = 0; i < config::MEMORY_SERVER_CNT; i++)
		memoryServers.push_back(new MemoryServer());
}

void createCoordinators(std::vector<Coordinator*> &coordinators) {
	for (primitive::coordinator_num_t i = 0; i < config::COORDINATOR_CNT; i++)
		coordinators.push_back(new Coordinator (i));
}

void connectCoordsToMSs(std::vector<Coordinator*> &coordinators, std::vector<MemoryServer*> &memoryServers) {
	for (primitive::coordinator_num_t c = 0; c < config::COORDINATOR_CNT; c++) {
		std::vector<MemoryServerContext> memoryServerCtxs;
		for (size_t m = 0; m < config::MEMORY_SERVER_CNT; m++) {
			MemoryServerContext* mscTmp = new MemoryServerContext (*memoryServers.at(m), true);
			memoryServerCtxs.push_back(*mscTmp);
		}
		// assign to coordinators
		coordinators.at(c)->connectToMemoryServers(memoryServerCtxs);
	}
}

void setup() {
	createMemoryServers(memoryServers);
	createCoordinators(coordinators);
	connectCoordsToMSs(coordinators, memoryServers);

	// first initializing the map, so all the buckets are not pointing anywhere
	for (size_t i = 0; i < config::HASH_SIZE; i++)
		bucketToPointerMap.insert(std::pair<size_t,uint64_t>(i,0ULL) );

	for (size_t i = 0; i < config::KEY_CNT; i++) {
		std::string k = "k" + utilities::ToString<size_t>(i);
		std::string v = "v" + utilities::ToString<size_t>(i);
		keyToValueMap.insert(std::pair<std::string,std::string>(k,v) );

		HashMaker h(k);
		keyToBucketMap.insert(std::pair<std::string, size_t>(k,h.getHashed()) );
	}
}

void resetMemoryServers() {
	for (size_t i = 0; i < memoryServers.size(); i++)
		memoryServers.at(i)->resetMemoryBuffers();

	for (size_t i = 0; i < config::HASH_SIZE; i++)
		bucketToPointerMap[i] = 0ULL;
}

void constructChange(Change **change __attribute__((unused)), std::vector<std::string> updateKeys, std::vector<std::string> pureDependencies) {
	std::vector<KeyValue> updates;
	std::vector<Dependency> dependencies;
	std::vector<std::string>::iterator it;

	for (it = updateKeys.begin(); it != updateKeys.end(); ++it){
		Key k(*it);
		Value v(keyToValueMap.find((std::string)*it)->second);
		KeyValue kv (k, v);
		updates.push_back(kv);
	}

	for (it = pureDependencies.begin(); it != pureDependencies.end(); ++it){
		size_t bucketID = keyToBucketMap.find((std::string)*it)->second;
		uint64_t headPointer = bucketToPointerMap.find(bucketID)->second;
		Pointer p = Pointer::makePointer(headPointer);
		Dependency dep(bucketID, p);
		dependencies.push_back(dep);
	}

	// adding the update keys as dependencies
	for (it = updateKeys.begin(); it != updateKeys.end(); ++it){
		size_t bucketID = keyToBucketMap.find((std::string)*it)->second;
		uint64_t headPointer = bucketToPointerMap.find(bucketID)->second;
		Pointer p = Pointer::makePointer(headPointer);
		Dependency dep(bucketID, p);
		dependencies.push_back(dep);
	}

	*change = new Change(updates, dependencies);
}

void updateBucketInfo(std::vector<std::string> updateKeys, std::vector<std::string> pureDependencies, Pointer &newEntryPointer) {
	uint64_t p = newEntryPointer.toULL();
	std::vector<std::string>::iterator it;
	for (it = updateKeys.begin(); it != updateKeys.end(); ++it) {
		size_t bucketID = keyToBucketMap.find((std::string)*it)->second;
		bucketToPointerMap[bucketID] = p;
	}

	for (it = pureDependencies.begin(); it != pureDependencies.end(); ++it) {
		size_t bucketID = keyToBucketMap.find((std::string)*it)->second;
		bucketToPointerMap[bucketID] = p;
	}
}

void SingleTreadedExecutionTest::test_all() {
	TestBase::printMessage(CLASS_NAME, __func__);
	setup();
	srand (time(NULL));

	test_simple_put_and_get();
	test_read_non_existing_key();
	test_no_key_update();
	test_no_dependency();
	test_complex_entry_chain();
}

void SingleTreadedExecutionTest::test_simple_put_and_get() {
	TestBase::printMessage(CLASS_NAME, __func__);
	resetMemoryServers();

	Change* change;
	Pointer newEntryPointer;
	std::vector<std::string> updateKeys;
	std::vector<std::string> pureDependencies;
	TID tid;
	tid.id = 12;

	updateKeys = {"k1", "k2"};
	pureDependencies = {"k3"};
	constructChange(&change, updateKeys, pureDependencies);
	error::ErrorType eType = coordinators.at(rand() % config::COORDINATOR_CNT)->applyChange(*change, tid, newEntryPointer);
	updateBucketInfo(updateKeys, pureDependencies, newEntryPointer);
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
			error::ErrorType eType = coordinators.at(c)->readByKey(key, scn, tid, value, searchDepth);
			assert(eType == error::SUCCESS && value.isEqual(keyToValueMap[readKeys.at(i)]));
		}
	}
}

void SingleTreadedExecutionTest::test_read_non_existing_key() {
	TestBase::printMessage(CLASS_NAME, __func__);
	resetMemoryServers();

	Key k1("k1");
	Value value;
	SCN scn(1);
	TID tid;
	tid.id = 12;
	int searchDepth;

	error::ErrorType eType = coordinators.at(0)->readByKey(k1, scn, tid, value, searchDepth);
	assert(eType == error::KEY_NOT_FOUND);
}

void SingleTreadedExecutionTest::test_no_key_update() {
	TestBase::printMessage(CLASS_NAME, __func__);
	resetMemoryServers();

	Change* change;
	Pointer newEntryPointer;
	std::vector<std::string> updateKeys;
	std::vector<std::string> pureDependencies;
	TID tid;
	tid.id = 12;

	updateKeys = {};
	pureDependencies = {"k1"};
	constructChange(&change, updateKeys, pureDependencies);
	ErrorType eType = coordinators.at(0)->applyChange(*change, tid, newEntryPointer);
	assert(eType == error::NO_UPDATE_KEY_IN_CHANGE);
}

void SingleTreadedExecutionTest::test_no_dependency() {
	TestBase::printMessage(CLASS_NAME, __func__);
	resetMemoryServers();

	Change* change;
	Pointer newEntryPointer;
	std::vector<std::string> updateKeys;
	std::vector<std::string> pureDependencies;
	TID tid;
	tid.id = 12;

	updateKeys = {"k1", "k2", "k3", "k4"};
	pureDependencies = {};
	constructChange(&change, updateKeys, pureDependencies);
	ErrorType eType = coordinators.at(0)->applyChange(*change, tid, newEntryPointer);
	updateBucketInfo(updateKeys, pureDependencies, newEntryPointer);
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
			error::ErrorType eType = coordinators.at(c)->readByKey(key, scn, tid, value, searchDepth);
			assert(eType == error::SUCCESS && value.isEqual(keyToValueMap[readKeys.at(i)]));
		}
	}
}

void SingleTreadedExecutionTest::test_complex_entry_chain() {
	TestBase::printMessage(CLASS_NAME, __func__);
	resetMemoryServers();

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
	constructChange(&change, updateKeys, pureDependencies);
	ErrorType eType = coordinators.at(rand() % config::COORDINATOR_CNT)->applyChange(*change, tid, newEntryPointer);
	updateBucketInfo(updateKeys, pureDependencies, newEntryPointer);
	assert(eType == error::SUCCESS);

	// insert k2, which depends on k1
	updateKeys = {"k2"};
	pureDependencies = {"k1"};
	constructChange(&change, updateKeys, pureDependencies);
	eType = coordinators.at(rand() % config::COORDINATOR_CNT)->applyChange(*change, tid, newEntryPointer);
	updateBucketInfo(updateKeys, pureDependencies, newEntryPointer);
	assert(eType == error::SUCCESS);

	// insert k3, which depends on k1
	updateKeys = {"k3"};
	pureDependencies = {"k1"};
	constructChange(&change, updateKeys, pureDependencies);
	eType = coordinators.at(rand() % config::COORDINATOR_CNT)->applyChange(*change, tid, newEntryPointer);
	updateBucketInfo(updateKeys, pureDependencies, newEntryPointer);
	assert(eType == error::SUCCESS);

	// insert k4, which depends on k2 and k3
	updateKeys = {"k4"};
	pureDependencies = {"k2", "k3"};
	constructChange(&change, updateKeys, pureDependencies);
	eType = coordinators.at(rand() % config::COORDINATOR_CNT)->applyChange(*change, tid, newEntryPointer);
	updateBucketInfo(updateKeys, pureDependencies, newEntryPointer);
	assert(eType == error::SUCCESS);

	// insert k5, which depends on k4
	updateKeys = {"k5"};
	pureDependencies = {"k4"};
	constructChange(&change, updateKeys, pureDependencies);
	eType = coordinators.at(rand() % config::COORDINATOR_CNT)->applyChange(*change, tid, newEntryPointer);
	updateBucketInfo(updateKeys, pureDependencies, newEntryPointer);
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
			error::ErrorType eType = coordinators.at(c)->readByKey(key, scn, tid, value, searchDepth);
			assert(eType == error::SUCCESS && value.isEqual(keyToValueMap[readKeys.at(i)]));
		}
	}
}
