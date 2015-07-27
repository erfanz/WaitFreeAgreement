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

AtomicUpdate::AtomicUpdate(Coordinator *coordinator) {
	coordinator_ = coordinator;
}

ErrorType AtomicUpdate::addDependency(const Key &key, Value &value) {
	Pointer pointerToEntry;
	error::ErrorType eType = coordinator_->readLatest(key, value, pointerToEntry);

	if (eType == error::SUCCESS) {
		HashMaker h(key);
		size_t bucketID = h.getHashed();
		Dependency d(bucketID, pointerToEntry);
		dependencies_.push_back(d);
	}
	return eType;
}

ErrorType AtomicUpdate::addUpdate(const Key &key, const Value &value) {
	KeyValue kv(key, value);
	updates_.push_back(kv);
	return error::SUCCESS;
}

ErrorType AtomicUpdate::apply() {
	Change change(updates_, dependencies_);
	TID tid;
	Pointer newEntryPointer;
	return coordinator_->applyChange(change, tid, newEntryPointer);
}

AtomicUpdate::~AtomicUpdate() {

}

