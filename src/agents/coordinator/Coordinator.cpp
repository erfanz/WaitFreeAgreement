///*
// *  Coordinator.cpp
// *
// *  Author		: Erfan Zamanian
// *  Created on	: Jun 10, 2015
// *  Description : PLEASE FILL IN THIS PART
// *
// */

#include "Coordinator.hpp"
#include "../../util/Errors.hpp"

Coordinator::Coordinator(coordinator_num_t coordNum) : coordinatorID (coordNum) {
	generationNum = 0;
	freeBufferOffset = (uint32_t)0;
}

Coordinator::~Coordinator() {
	// TODO Auto-generated destructor stub
}


//int Coordinator::connectToMemoryServers(std::vector<MemoryServer> memoryServers){
//	this->memoryServers = memoryServers;
//	return 0;
//}

ErrorType Coordinator::applyChange(Change &change, TID tID){
	LogEntry entry;
	Pointer entryPointer;
	TestError(createNewPointer(change, entryPointer), errorHandler);
	TestError(makeNewLogEntry(change, entryPointer, entry), errorHandler);
	TestError(propagateLogEntry(entry), errorHandler);
	TestError(publishChanges(entry), errorHandler);
}

ErrorType Coordinator::createNewPointer(Change &change, Pointer &pointer) {
	entry_size_t entrySize = LogEntry::calculateEntrySize(change.getDependencies(), change.getUpdates());
	pointer.setAll(coordinatorID, generationNum, entrySize, freeBufferOffset);

	freeBufferOffset += entrySize;	// moving forward the free buffer head

	return error::SUCCESS;
}

ErrorType Coordinator::makeNewLogEntry(Change &change, Pointer &entryPointer, LogEntry &entry) const{
	bool isSerialized = false;
	entry.setAll(change.getDependencies(), change.getUpdates(), entryPointer, isSerialized);
	return error::SUCCESS;
}

ErrorType Coordinator::propagateLogEntry(LogEntry &entry){
	for (size_t i = 0; i < memoryServerCtxs.size(); i++) {
		memoryServerCtxs.at(i).writeLogEntry(entry);
	}
	return error::SUCCESS;
}

ErrorType Coordinator::publishChanges(LogEntry &entry){
	return error::SUCCESS;
}

void Coordinator::errorHandler(ErrorType eType) {
	;
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
