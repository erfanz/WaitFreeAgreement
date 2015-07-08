/*
 *  SingleProcessExecution.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 30, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "../agents/coordinator/Coordinator.hpp"
#include "../agents/coordinator/MemoryServerContext.hpp"
#include "../agents/memory_server/MemoryServer.hpp"
#include "../base_types/PrimitiveTypes.hpp"
#include "../config.hpp"
#include <vector>
#include <map>

std::map<size_t,uint64_t> bucketToPointerMap;	// stores where each bucket points to during the execution of the test.
std::map<std::string,std::string> keyToValueMap;	// stores the value associated with any given key.
std::map<std::string,size_t> keyToBucketMap;	// stores the bucketID associated with any given key.

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

int main () {
	Change* change;
	Pointer newEntryPointer;

	std::vector<MemoryServer*> memoryServers;
	createMemoryServers(memoryServers);

	std::vector<Coordinator*> coordinators;
	createCoordinators(coordinators);

	connectCoordsToMSs(coordinators, memoryServers);

	// first initializing the map, so all the buckets are not pointing anywhere
	for (size_t i = 0; i < config::HASH_SIZE; i++)
		bucketToPointerMap.insert(std::pair<size_t,uint64_t>(i,0x0000000000000000) );

	for (size_t i = 0; i < config::KEY_CNT; i++) {
		std::string k = "k" + utilities::ToString<size_t>(i);
		std::string v = "v" + utilities::ToString<size_t>(i);
		keyToValueMap.insert(std::pair<std::string,std::string>(k,v) );

		HashMaker h(k);
		keyToBucketMap.insert(std::pair<std::string, size_t>(k,h.getHashed()) );
	}

	/* End of initialization
	 * -----------------------------------------------------------------------------------------------
	 * -----------------------------------------------------------------------------------------------
	 */

	std::vector<std::string> updateKeys;
	std::vector<std::string> pureDependencies;
	TID tid;
	tid.id = 12;

	updateKeys = {"k1", "k2"};
	pureDependencies = {};
	constructChange(&change, updateKeys, pureDependencies);
	coordinators.at(0)->applyChange(*change, tid, newEntryPointer);
	updateBucketInfo(updateKeys, pureDependencies, newEntryPointer);

	std::cout << std::endl;

	updateKeys = {"k5", "k2"};
	pureDependencies = {"k1"};
	constructChange(&change, updateKeys, pureDependencies);
	coordinators.at(1)->applyChange(*change, tid, newEntryPointer);
	updateBucketInfo(updateKeys, pureDependencies, newEntryPointer);

	std::cout << std::endl;

	updateKeys = {"k9", "k8", "k7", "k6"};
	pureDependencies = {"k2", "k3", "k4", "k5"};
	constructChange(&change, updateKeys, pureDependencies);
	coordinators.at(1)->applyChange(*change, tid, newEntryPointer);
	updateBucketInfo(updateKeys, pureDependencies, newEntryPointer);

	std::cout << std::endl;


	// let's see if the change is visible
	Key k1("k1");
	Value value;
	SCN scn(1);
	coordinators.at(0)->readByKey(k1, scn, tid, value);

	std::cout << "The content of key " << k1.getId() << " is " << value.getContent() << std::endl;


	// deleting objects
	for (size_t i = 0; i < config::MEMORY_SERVER_CNT; i++) {
		delete memoryServers.at(i);
	}

	for (size_t i = 0; i < config::COORDINATOR_CNT; i++) {
		delete coordinators.at(i);
	}

	return 0;
}
