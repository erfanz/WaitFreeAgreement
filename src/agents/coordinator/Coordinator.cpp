///*
// *  Coordinator.cpp
// *
// *  Author		: Erfan Zamanian
// *  Created on	: Jun 10, 2015
// *  Description : PLEASE FILL IN THIS PART
// *
// */

#include "Coordinator.hpp"
#include "../../base_types/HashMaker.hpp"


#define CLASS_NAME	"Coord"


Coordinator::Coordinator(primitive::coordinator_num_t coordNum) : coordinatorID_(coordNum) {
	// since coordinatorID_ is const, it must be initialized using constructor initialization list
	generationNum_ = 0;
	freeBufferOffset_ = (uint32_t)0;
	DEBUG_COUT(CLASS_NAME, __func__, "Coordinator created with ID " << (int)coordinatorID_);
}

Coordinator::~Coordinator() {
	DEBUG_COUT(CLASS_NAME, __func__, "Coordinator destroyed with ID " << (int)coordinatorID_);
}

ErrorType Coordinator::connectToMemoryServers(std::vector<MemoryServerContext> memoryServerCtxs){
	this->memoryServerCtxs_ = memoryServerCtxs;
	this->localMSCtxIndex_ = coordinatorID_;
	DEBUG_COUT(CLASS_NAME, __func__, "Memory server contexts are all set for coordinator " << (int)coordinatorID_);
	return error::SUCCESS;
}

ErrorType Coordinator::applyChange(Change &change, TID tID, Pointer &newEntryPointer){
	DEBUG_COUT(CLASS_NAME, __func__, "Applying Change: " << change.toString() << " by coordinator " << (int)coordinatorID_);

	if (change.getUpdates().size() == 0) {
		DEBUG_COUT(CLASS_NAME, __func__, "Change does not contain any update");
		return error::NO_UPDATE_KEY_IN_CHANGE;
	}

	(void)tID;	// TODO: since it's unused

	LogEntry *entry = NULL;
	Pointer *entryPointer = NULL;

	error::Throwable::testError(createNewPointer(change, &entryPointer));
	error::Throwable::testError(makeNewLogEntry(change, *entryPointer, &entry));
	error::Throwable::testError(propagateLogEntry(*entry));
	error::Throwable::testError(publishChanges(*entry));
	error::Throwable::testError(serialize(*entry));

	newEntryPointer = *entryPointer;	// for logging purposes. Otherwise, the variable 'newEntryPointer' can be removed (also from the function signature)

	DEBUG_COUT(CLASS_NAME, __func__, "Change successfully applied by coordinator " << (int)coordinatorID_);
	return error::SUCCESS;
}

ErrorType Coordinator::createNewPointer(Change &change, Pointer **pointer) {

	primitive::entry_size_t entrySize = LogEntry::calculateEntrySize(change.getDependencies(), change.getUpdates());
	*pointer = new Pointer(coordinatorID_, generationNum_, entrySize, freeBufferOffset_);

	freeBufferOffset_ += entrySize;	// moving forward the free buffer head

	DEBUG_COUT(CLASS_NAME, __func__, "Pointer created (" << (*pointer)->toHexString()
			<< ") by coordinator " << (int)coordinatorID_);
	return error::SUCCESS;
}

ErrorType Coordinator::makeNewLogEntry(Change &change, Pointer &entryPointer, LogEntry **entry __attribute__((unused))) const{
	bool isSerialized = false;

	*entry = new LogEntry(change.getDependencies(), change.getUpdates(), entryPointer, isSerialized);
	DEBUG_COUT(CLASS_NAME, __func__, "Log entry created with pointer = "
			<< (*entry)->getCurrentP().toHexString() << " by coordinator " << (int)coordinatorID_);
	return error::SUCCESS;
}

ErrorType Coordinator::propagateLogEntry(LogEntry &entry){
	for (size_t i = 0; i < memoryServerCtxs_.size(); i++) {
		memoryServerCtxs_.at(i).writeLogEntry(coordinatorID_, entry);
	}

	DEBUG_COUT(CLASS_NAME, __func__, "Log entry " << entry.getCurrentP().toHexString()
			<< " propagated to all replicas by coordinator " << (int)coordinatorID_);
	return error::SUCCESS;
}

ErrorType Coordinator::publishChanges(LogEntry &entry){
	std::vector<Dependency> dependencies = entry.getDependencies();
	Pointer actualCurrentHead;

	for (size_t i = 0; i < memoryServerCtxs_.size(); i++) {
		for (size_t d = 0; d < dependencies.size(); d++){

			ErrorType eType = memoryServerCtxs_.at(i).swapBucketHash(
					dependencies.at(d).getBucketID(),
					dependencies.at(d).getPointer(),
					entry.getCurrentP(), actualCurrentHead);

			if (eType != error::SUCCESS) {
				std::cerr << "CAS failed!!" << std::endl;
				return eType;
			}
		}
		DEBUG_COUT(CLASS_NAME, __func__, "CAS " << dependencies.size() << " hash bucket(s) in memory server "
				<< i << " by coordinator " << (int)coordinatorID_);
	}

	DEBUG_COUT(CLASS_NAME, __func__, "Published the change by coordinator " << (int)coordinatorID_);
	return error::SUCCESS;
}

ErrorType Coordinator::serialize(LogEntry &entry) {
	for (size_t i = 0; i < memoryServerCtxs_.size(); i++) {
		memoryServerCtxs_.at(i).markSerialized(coordinatorID_, entry);
	}
	DEBUG_COUT(CLASS_NAME, __func__, "Log entry " << entry.getCurrentP().toHexString()
			<< " serialized on all replicas by coordinator " << (int)coordinatorID_);

	return error::SUCCESS;
}

ErrorType Coordinator::readByKey(const Key key, const SCN scn, const TID tid, Value &returnValue, int &searchDepth) {
	(void)scn;	// TODO: must be removed
	(void)tid;	// TODO: must be removed

	HashMaker hashedKey(key.getId());
	LogEntry entry;
	Pointer p;
	Value v;

	ErrorType eType = memoryServerCtxs_.at(localMSCtxIndex_).readBucketHash(hashedKey, p);
	DEBUG_COUT(CLASS_NAME, __func__, "Reading bucket hash for key " << key.getId() << " resulted pointer: "
			<< p.toHexString() << " by coordinator " << (int)coordinatorID_);

	if (eType == error::KEY_NOT_FOUND)
		return eType;

	searchDepth = 1;

	while (true) {
		error::Throwable::testError(memoryServerCtxs_.at(localMSCtxIndex_).readLogEntry(p, entry));
		if (entry.getUpdateIfExists(key, v) == true) {
			// TODO: check if the KV pair fulfills the SCN and TID conditions
			DEBUG_COUT(CLASS_NAME, __func__, "Key " << key.getId() << " found as an update in log entry " << p.toHexString()
					<< " by coordinator " << (int)coordinatorID_);
			break;
		}
		else if (entry.getDependencyIfExists(hashedKey.getHashed(), p) == true) {
			DEBUG_COUT(CLASS_NAME, __func__, "Key " << key.getId() << " found as a dependency in log entry "
					<< p.toHexString() << " by coordinator " << (int)coordinatorID_);

			searchDepth++;
			continue;
		}
		else {
			// no cleanup is needed
			DEBUG_COUT(CLASS_NAME, __func__, "Key " << key.getId() << " found neither as a dependency nor an update in log entry "
					<< p.toHexString() << " by coordinator " << (int)coordinatorID_);
			return error::KEY_NOT_FOUND;
		}
	}
	returnValue = v;
	return error::SUCCESS;
}

void Coordinator::errorHandler(const ErrorType eType) {
	std::cerr << "Error Occured, type: " << eType << std::endl;
}
