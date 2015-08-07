/*
 *  Coordinator.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 10, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef AGENTS_COORDINATOR_COORDINATOR_HPP_
#define AGENTS_COORDINATOR_COORDINATOR_HPP_

#include "MemoryServerContext.hpp"
#include "../../base_types/LogEntry.hpp"
#include "../../base_types/KeyValue.hpp"
#include "../../base_types/Change.hpp"
#include "../../base_types/PrimitiveTypes.hpp"	// primitive::XXX
#include "../../errors/Error.hpp"		// to return error from functions
#include "../../base_types/TID.hpp"
#include "../../base_types/SCN.hpp"
#include "../../util/utils.hpp"
#include <vector>
#include <set>
#include <map>


typedef error::ErrorType ErrorType;


class Coordinator : public error::Throwable{
private:
	const primitive::coordinator_num_t	coordinatorID_;
	primitive::generation_num_t			generationNum_;
	primitive::offset_t					freeBufferOffset_;
	std::vector<MemoryServerContext>	memoryServerCtxs_;
	size_t								localMSCtxIndex_;

	template <typename container> ErrorType readBucketHeadsFromAllReplicas(const container &readBuckets, std::vector<std::map<size_t, Pointer> > &bucketHeads);
	ErrorType blockingReadEntry(const Pointer &pointer, LogEntry &entry);
	//bool checkIfBlocks (const LogEntry &biggerEntry, const LogEntry &smallerEntry, const std::vector<std::map<size_t, Pointer> > &bucketHeads);
	bool isFailed (const LogEntry &e, const std::vector<std::map<size_t, Pointer> > &collectedBucketHeads, std::map<Pointer, LogEntry> &pointerToEntryMap) const;
	ErrorType finishMakingSerialized(const LogEntry &e, const std::vector<std::map<size_t, Pointer> > &collectedBucketHeads, std::map<Pointer, LogEntry> &pointerToEntryMap);

	ErrorType propagateLogEntry(LogEntry &entry);
	ErrorType createNewPointer(Change &change, Pointer **pointer);
	ErrorType makeNewLogEntry(Change &change, Pointer &entryPointer, LogEntry **entry) const;
	ErrorType publishChanges(LogEntry &entry);
	ErrorType replicateSerializationStatus(const LogEntry &entry, const LogEntry::Status serializedFlag);

public:
	friend class CoordinatorTest;	// since we want to test even private member methods of Coordinator in our unit tests
	Coordinator(primitive::coordinator_num_t coordinatorID);
	~Coordinator();
	ErrorType connectToMemoryServers(std::vector<MemoryServerContext> memoryServerCtxs);
	ErrorType applyChange(Change &change, TID tID, LogEntry **newEntry);
	ErrorType readLatest(const Key &key, Value &returnValue, LogEntry &headEntry);
	ErrorType readByKey(const Key key, const SCN scn, const TID tid, Value &returnValue, int &searchDepth);
	ErrorType checkIfSerialized(const LogEntry &entry);
	ErrorType resolve(const size_t bucketID, LogEntry &headEntry);

};

#endif /* AGENTS_COORDINATOR_COORDINATOR_HPP_ */
