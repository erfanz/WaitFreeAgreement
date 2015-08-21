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


std::vector<Coordinator*>	agents_handler::coordinators;
std::vector<Change*>		agents_handler::changes;
MemoryRequestDispatcher		*agents_handler::dispatcher;

std::map<size_t,uint64_t> agents_handler::bucketToPointerMap;	// stores where each bucket points to during the execution of the test.
std::map<std::string,std::string> agents_handler::keyToValueMap;	// stores the value associated with any given key.
std::map<std::string,size_t> agents_handler::keyToBucketMap;	// stores the bucketID associated with any given key.


void agents_handler::createCoordinators(std::vector<Coordinator*> &coordinators) {
	std::shared_ptr<RequestBuffer> reqBufferPtr = dispatcher->getRequestBuffer();
	for (primitive::coordinator_num_t i = 0; i < config::COORDINATOR_CNT; i++)
		coordinators.push_back(new Coordinator (i, reqBufferPtr));
}

void agents_handler::createChanges(std::vector<Change*> &atomicUpdates) {
	for (primitive::coordinator_num_t i = 0; i < config::COORDINATOR_CNT; i++)
		atomicUpdates.push_back(new Change(coordinators[i]));
}

void agents_handler::setup() {
	dispatcher = new MemoryRequestDispatcher();
	agents_handler::createCoordinators(coordinators);
	agents_handler::createChanges(changes);

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

void agents_handler::resetMemoryRegions() {
	dispatcher->resetAllInstances();

	for (size_t i = 0; i < config::HASH_SIZE; i++)
		bucketToPointerMap[i] = 0ULL;
}

/*

void agents_handler::constructDependencies(Change **change __attribute__((unused)), const std::vector<std::string> updateKeys, const std::vector<std::string> pureDependencies) {
	std::vector<KeyValue> updates;
	std::vector<Dependency> dependencies;
	std::vector<std::string>::const_iterator it;
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
*/

void agents_handler::updateBucketInfo(const std::vector<std::string> &updateKeys, const std::vector<std::string> &pureDependencies, const Pointer &newEntryPointer) {
	uint64_t p = newEntryPointer.toULL();
	std::vector<std::string>::const_iterator it;
	for (it = updateKeys.begin(); it != updateKeys.end(); ++it) {
		size_t bucketID = keyToBucketMap.find((std::string)*it)->second;
		bucketToPointerMap[bucketID] = p;
	}

	for (it = pureDependencies.begin(); it != pureDependencies.end(); ++it) {
		size_t bucketID = keyToBucketMap.find((std::string)*it)->second;
		bucketToPointerMap[bucketID] = p;
	}
}
