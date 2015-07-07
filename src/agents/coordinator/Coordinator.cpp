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

ErrorType Coordinator::applyChange(Change &change, TID tID){
	(void)tID;	// TODO: since it's unused

	LogEntry *entry = NULL;
	Pointer *entryPointer = NULL;

	error::Throwable::testError(createNewPointer(change, &entryPointer));
	error::Throwable::testError(makeNewLogEntry(change, *entryPointer, &entry));
	error::Throwable::testError(propagateLogEntry(*entry));
	error::Throwable::testError(publishChanges(*entry));
	error::Throwable::testError(serialize(*entry));

	DEBUG_COUT(CLASS_NAME, __func__, "Change successfully applied by coordinator " << (int)coordinatorID_);
	return error::SUCCESS;
}

ErrorType Coordinator::createNewPointer(Change &change, Pointer **pointer) {

	primitive::entry_size_t entrySize = LogEntry::calculateEntrySize(change.getDependencies(), change.getUpdates());
	*pointer = new Pointer(coordinatorID_, generationNum_, entrySize, freeBufferOffset_);

	freeBufferOffset_ += entrySize;	// moving forward the free buffer head

	DEBUG_COUT(CLASS_NAME, __func__, "Pointer created (" << (*pointer)->toHexString() << ") by coordinator " << (int)coordinatorID_);

	return error::SUCCESS;
}

ErrorType Coordinator::makeNewLogEntry(Change &change, Pointer &entryPointer, LogEntry **entry __attribute__((unused))) const{
	bool isSerialized = false;

	*entry = new LogEntry(change.getDependencies(), change.getUpdates(), entryPointer, isSerialized);
	DEBUG_COUT(CLASS_NAME, __func__, "Log entry created with pointer = " << (*entry)->getCurrentP().toHexString() << " by coordinator " << (int)coordinatorID_);

	return error::SUCCESS;
}

ErrorType Coordinator::propagateLogEntry(LogEntry &entry){
	for (size_t i = 0; i < memoryServerCtxs_.size(); i++) {
		memoryServerCtxs_.at(i).writeLogEntry(coordinatorID_, entry);
		DEBUG_COUT(CLASS_NAME, __func__, "Copied log entry to memory server " << i << " by coordinator " << (int)coordinatorID_);
	}
	return error::SUCCESS;
}

ErrorType Coordinator::publishChanges(LogEntry &entry){
	std::vector<Dependency> dependencies = entry.getDependencies();
	Pointer actualCurrentHead;

	for (size_t i = 0; i < memoryServerCtxs_.size(); i++) {
		for (size_t d = 0; d < dependencies.size(); d++){
			DEBUG_COUT(CLASS_NAME, __func__, "CAS the hash bucket " << dependencies.at(d).getBucketID() << " in memory server " << i << " by coordinator " << (int)coordinatorID_);

			ErrorType eType = memoryServerCtxs_.at(i).swapBucketHash(dependencies.at(d).getBucketID(), dependencies.at(d).getPointer(), entry.getCurrentP(), actualCurrentHead);
			if (eType != error::SUCCESS) {
				std::cerr << "CAS failed!!" << std::endl;
				return eType;
			}
		}
	}

	DEBUG_COUT(CLASS_NAME, __func__, "Published the change by coordinator " << (int)coordinatorID_);

	return error::SUCCESS;
}

ErrorType Coordinator::serialize(LogEntry &entry) {
	for (size_t i = 0; i < memoryServerCtxs_.size(); i++) {
		memoryServerCtxs_.at(i).markSerialized(coordinatorID_, entry);
		DEBUG_COUT(CLASS_NAME, __func__, "The new log entry marked serialized on memory server " << i << " by coordinator " << (int)coordinatorID_);
	}
	return error::SUCCESS;
}

ErrorType Coordinator::readByKey(const Key key, const SCN scn, const TID tid, Value &returnValue) {
	(void)scn;	// TODO: must be removed
	(void)tid;	// TODO: must be removed


	HashMaker hashedKey(key.getId());
	LogEntry entry;
	Pointer p;
	Value v;


	error::Throwable::testError(memoryServerCtxs_.at(localMSCtxIndex_).readBucketHash(hashedKey, p));
	DEBUG_COUT(CLASS_NAME, __func__, "Reading bucket hash for key " << key.getId() << " resulted pointer: " << p.toHexString() << " by coordinator " << (int)coordinatorID_);


	while (true) {
		error::Throwable::testError(memoryServerCtxs_.at(localMSCtxIndex_).readLogEntry(p, entry));
		if (entry.getDependencyIfExists(hashedKey.getHashed(), p) == true) {
			continue;
		}
		else if (entry.getUpdateIfExists(key, v) == true) {
			// TODO: check if the KV pair fulfills the SCN and TID conditions
			break;
		}
		else {
			// no cleanup is needed
			return error::KEY_NOT_IN_THE_LOG_ENTRY;
		}
	}
	returnValue = v;
	return error::SUCCESS;
}

void Coordinator::errorHandler(const ErrorType eType) {
	std::cerr << "Error Occured, type: " << eType << std::endl;
}

//int Coordinator::readByKey(const Key key, const SCN scn, const TID tid){
//	// hash the key to find the corresponding bucket
//	HashedKey hashedKey = key.hashKey();
//
//	// find the local memory server, if exists
//	const MemoryServerContext *localMS = findLocalMemoryServer();
//
//	// Checks if it's not out-dated for that particular bucket
//	const SCN missedSCN = localMS->getLastMissedSCN(hashedKey);
//	if (! scn.isSCNCompatible(missedSCN))
//		// TODO: Hand the request over to another coordinator
//		return ERROR_LOCAL_REPLICA_IS_OUTDATED;
//
//
//	// Searches down the key hash to find the entry with the given key and with appropriate scn
//	LogEntry entry = localMS->findItemByVersion(hashedKey, key, scn, tid);
//
//	// check if the item is serialized
//	if (entry.isSerialized())
//		return entry.getValue();
//	else{
//		// TODO resolve conflict
//		return -1;
//	}
//}


//const MemoryServerContext* Coordinator::findLocalMemoryServer(){
//	return &memoryServerContexts.at(coordinatorNum);
//}
