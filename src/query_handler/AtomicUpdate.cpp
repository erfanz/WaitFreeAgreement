/*
 *  AtomicUpdate.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jul 9, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "AtomicUpdate.hpp"
#include "../base_types/HashMaker.hpp"
#include "../base_types/TID.hpp"

#define CLASS_NAME	"AtomicUpdate"


AtomicUpdate::AtomicUpdate(Coordinator *coordinator) {
	DEBUG_COUT(CLASS_NAME, __func__, "Atomic Update with coordinator " << (int) coordinator->getID() << " created");
	coordinator_ = coordinator;
	newEntry = NULL;
}

void AtomicUpdate::reset() {
	dependencyMap_.clear();
	updates_.clear();
	newEntry = NULL;
}

ErrorType AtomicUpdate::get(const Key &key, Value &value) {
	DEBUG_COUT(CLASS_NAME, __func__, "getting value for key " << key.getId());

	LogEntry headEntry;
	HashMaker h(key.getId());
	size_t bucketID = h.getHashed();
	std::map<size_t, Pointer>::const_iterator it;

	ErrorType eType = coordinator_->readLatest(key, value, headEntry);

	if (eType == error::SUCCESS || eType == error::ENTRY_DOES_NOT_EXIST) {
		// First, check if this bucket is already in the dependencies, but with a different value.
		// If so, return an error
		it = dependencyMap_.find(bucketID);
		if (it != dependencyMap_.end() && !it->second.isEqual(headEntry.getCurrentP() ) )
			return error::GET_POINTER_CHANGED;

		// Else, add the dependency
		else {
			DEBUG_COUT (CLASS_NAME, __func__, "Added the dependency dep[" << (int)bucketID << "] = " << headEntry.getCurrentP().toHexString());
			dependencyMap_[bucketID] = headEntry.getCurrentP();
		}
	}
	return eType;
}

ErrorType AtomicUpdate::set(const Key &key, const Value &value) {
	DEBUG_COUT(CLASS_NAME, __func__, "setting value[" << key.getId() << "] = " << value.getContent());

	KeyValue kv(key, value);
	HashMaker h(key.getId());
	size_t bucketID = h.getHashed();

	if (dependencyMap_.find(bucketID) == dependencyMap_.end())
		return error::SET_WITH_NO_GET;

	updates_.push_back(kv);
	return error::SUCCESS;
}

ErrorType AtomicUpdate::serialize() {
	DEBUG_COUT(CLASS_NAME, __func__, "serializing the change");

	TID tid;
	ErrorType eType;

	// Transform the dependencyMap_ to vector type (which is needed by the change constructor)
	std::vector<Dependency> deps;

	std::map<size_t, Pointer>::const_iterator it;
	for (it = dependencyMap_.begin(); it != dependencyMap_.end(); it++) {
		Dependency d(it->first, it->second);
		deps.push_back(d);
	}

	// Construct the change
	Change change(updates_, deps);
	eType = coordinator_->applyChange(change, tid, &newEntry);

	if (eType == error::SUCCESS)
		return error::SUCCESS;
	else {
		// it would be unsafe to assume that the change was unsuccessful,
		// as it may be change that the change gets serialized, either because of the outstanding CASes or by other coordinators call resolve
		// Therefore, resolve must be called on any of the change dependencies (doesn't matter which one), to make sure that the entry is serialized or failed

		LogEntry resolvedEntry;
		size_t sampleBucketDependency = change.getDependencies()[0].getBucketID();
		eType = coordinator_->resolve(sampleBucketDependency, resolvedEntry);
		if (eType != error::SUCCESS)
			return eType;

		return coordinator_->checkIfSerialized(*newEntry);
	}
}

AtomicUpdate::~AtomicUpdate() {
	delete newEntry;	// which was allocated in Coordinator class
	DEBUG_COUT(CLASS_NAME, __func__, "Atomic Update destroyed!");
}
