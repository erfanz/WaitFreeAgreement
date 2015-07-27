/*
 *  AgentsHandler.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jul 14, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "AgentsHandler.hpp"
#include <algorithm>		// for find()


std::vector<MemoryServer*> agents_handler::memoryServers;
std::vector<Coordinator*> agents_handler::coordinators;

std::map<size_t,uint64_t> agents_handler::bucketToPointerMap;	// stores where each bucket points to during the execution of the test.
std::map<std::string,std::string> agents_handler::keyToValueMap;	// stores the value associated with any given key.
std::map<std::string,size_t> agents_handler::keyToBucketMap;	// stores the bucketID associated with any given key.

void agents_handler::createMemoryServers(std::vector<MemoryServer*> &memoryServers){
	for (size_t i = 0; i < config::MEMORY_SERVER_CNT; i++)
		memoryServers.push_back(new MemoryServer());
}

void agents_handler::createCoordinators(std::vector<Coordinator*> &coordinators) {
	for (primitive::coordinator_num_t i = 0; i < config::COORDINATOR_CNT; i++)
		coordinators.push_back(new Coordinator (i));
}

void agents_handler::connectCoordsToMSs(std::vector<Coordinator*> &coordinators, std::vector<MemoryServer*> &memoryServers) {
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

void agents_handler::setup() {
	agents_handler::createMemoryServers(memoryServers);
	agents_handler::createCoordinators(coordinators);
	agents_handler::connectCoordsToMSs(coordinators, memoryServers);

	// first initializing the map, so all the buckets are not pointing anywhere
	for (size_t i = 0; i < config::HASH_SIZE; i++)
		agents_handler::bucketToPointerMap.insert(std::pair<size_t,uint64_t>(i,0ULL) );

	for (size_t i = 0; i < config::KEY_CNT; i++) {
		std::string k = "k" + utilities::ToString<size_t>(i);
		std::string v = "v" + utilities::ToString<size_t>(i);
		agents_handler::keyToValueMap.insert(std::pair<std::string,std::string>(k,v) );

		HashMaker h(k);
		agents_handler::keyToBucketMap.insert(std::pair<std::string, size_t>(k,h.getHashed()) );
	}
}

void agents_handler::resetMemoryServers() {
	for (size_t i = 0; i < memoryServers.size(); i++)
		memoryServers.at(i)->resetMemoryBuffers();

	for (size_t i = 0; i < config::HASH_SIZE; i++)
		bucketToPointerMap[i] = 0ULL;
}

void agents_handler::constructChange(Change **change __attribute__((unused)), std::vector<std::string> updateKeys, std::vector<std::string> pureDependencies) {
	std::vector<KeyValue> updates;
	std::vector<Dependency> dependencies;
	std::vector<std::string>::iterator it;
	std::vector<size_t> buckets;

	for (it = updateKeys.begin(); it != updateKeys.end(); ++it){
		Key k(*it);
		Value v(keyToValueMap.find((std::string)*it)->second);
		KeyValue kv (k, v);
		updates.push_back(kv);
	}

	for (it = pureDependencies.begin(); it != pureDependencies.end(); ++it){
		size_t bucketID = keyToBucketMap.find((std::string)*it)->second;
		if (std::find(buckets.begin(), buckets.end(), bucketID) == buckets.end()) {
			buckets.push_back(bucketID);

			uint64_t headPointer = bucketToPointerMap.find(bucketID)->second;
			Pointer p = Pointer::makePointer(headPointer);
			Dependency dep(bucketID, p);
			dependencies.push_back(dep);
		}
	}

	// adding the update keys as dependencies
	for (it = updateKeys.begin(); it != updateKeys.end(); ++it){
		size_t bucketID = keyToBucketMap.find((std::string)*it)->second;
		if (std::find(buckets.begin(), buckets.end(), bucketID) == buckets.end()) {
			buckets.push_back(bucketID);

			uint64_t headPointer = bucketToPointerMap.find(bucketID)->second;
			Pointer p = Pointer::makePointer(headPointer);
			Dependency dep(bucketID, p);
			dependencies.push_back(dep);
		}
	}

	*change = new Change(updates, dependencies);
}

void agents_handler::updateBucketInfo(std::vector<std::string> updateKeys, std::vector<std::string> pureDependencies, Pointer &newEntryPointer) {
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