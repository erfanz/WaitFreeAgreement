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

Coordinator::Coordinator(uint32_t coordNum) : coordinatorNum (coordNum) {
	generationNum = 0;
	freeBufferOffset = (uint32_t)0;
}

Coordinator::~Coordinator() {
	// TODO Auto-generated destructor stub
}


int Coordinator::connectToMemoryServers(std::vector<MemoryServer> memoryServers){
	this->memoryServers = memoryServers;
	return 0;
}
int Coordinator::writeLogEntry(){
	// a log entry is created and is placed on the log journal of all memory servers

	return 0;
}
int Coordinator::publishChanges(){
	return 0;
}


int Coordinator::applyChange(std::vector<KeyValue> &updates, std::vector<KeyValue> &dependencies, TID tID){
	const MemoryServerContext *localMS = findLocalMemoryServer();
	for (int i = 0; i < updates.size(); i++){


	}
}


int Coordinator::putByKey(Key key, Value value){
//	const LogEntry *entry = constructUpdate(key, value);
//	propagateLogEntry(entry, )
//
//	for (size_t i = 0; i < MEMORY_SERVER_CNT; i++){
//
//
//	}
}
int Coordinator::readByKey(const Key key, const SCN scn, const TID tid){
	// hash the key to find the corresponding bucket
	HashedKey hashedKey = key.hashKey();

	// find the local memory server, if exists
	const MemoryServerContext *localMS = findLocalMemoryServer();

	// Checks if it's not out-dated for that particular bucket
	const SCN missedSCN = localMS->getLastMissedSCN(hashedKey);
	if (! scn.isSCNCompatible(missedSCN))
		// TODO: Hand the request over to another coordinator
		return ERROR_LOCAL_REPLICA_IS_OUTDATED;


	// Searches down the key hash to find the entry with the given key and with appropriate scn
	LogEntry entry = localMS->findItemByVersion(hashedKey, key, scn, tid);

	// check if the item is serialized
	if (entry.isSerialized())
		return entry.getValue();
	else{
		// TODO resolve conflict
		return -1;
	}
}

const LogEntry* Coordinator::constructUpdate (const Key key, const Value value) const{
//	// 1. Find the local memory server, if exists.
//	const MemoryServerContext *localMS = findLocalMemoryServer();
//
//	// 2. Check whether the key is up-to-date.
//	uint64_t hashedKey = key.hashKey();
//
//	if (localMS->checkKeyValid(hashedKey, key) == true) {
//		// The local memory server is up-to-date
//		// so it's safe to construct the update from the local copy
//
//		//Pointer oldVersion = localMS->findTheLatestVersion(hashedKey, key);
//		const LogEntry* entry = makeNewLogEntry(key, value);
//		return entry;
//
//	}
//	else {
//		// the coordinator might have missed updates, run Paxos and make read repair
//
//		// TODO
//		return 0L;

	}
}

const MemoryServerContext* Coordinator::findLocalMemoryServer(){
	return &memoryServerContexts.at(coordinatorNum);
}

const LogEntry* Coordinator::makeNewLogEntry(const uint64_t &hashedKey, const Key &key, const Value &value) const{
	const Pointer* oldPointer = getLastUpdate(hashedKey, key);

	Pointer newPointer = Pointer(coordinatorNum, freeBufferOffset, value.getSize(), generationNum);
	LogEntry *entry = new LogEntry (value, oldPointer, newPointer);

	return entry;


}
