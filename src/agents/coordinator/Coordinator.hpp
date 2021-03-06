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

#include "../../request_buffer/request/Request.hpp"
#include "../../request_buffer/RequestBuffer.hpp"
#include "../../base_types/LogEntry.hpp"
#include "../../base_types/EntryState.hpp"
#include "../../base_types/KeyValue.hpp"
#include "../../base_types/PrimitiveTypes.hpp"	// primitive::SOME_TYPE
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
	std::shared_ptr<RequestBuffer>		requestBuffer_;
	primitive::generation_num_t			generationNum_;
	primitive::offset_t					freeBufferOffset_;
	size_t								localMSIndex_;

	template <typename container> ErrorType readBucketHeadsFromAllReplicas(const container &readBuckets, std::vector<std::map<size_t, Pointer> > &bucketHeads);
	ErrorType readEntry(const Pointer &pointer, LogEntry &entry);
	bool isFailed (const LogEntry &e, const std::vector<std::map<size_t, Pointer> > &collectedBucketHeads, const std::map<Pointer, LogEntry> &pointerToEntryMap) const;
	ErrorType finishMakingSerialized(const LogEntry &e, const std::vector<std::map<size_t, Pointer> > &collectedBucketHeads, const std::map<Pointer, LogEntry> &pointerToEntryMap);
	ErrorType createNewPointer(const std::vector<Dependency> &deps, const std::vector<KeyValue> &updates, Pointer **pointer);
	ErrorType makeNewLogEntry(const std::vector<Dependency> &deps, const std::vector<KeyValue> &updates, const Pointer &entryPointer, LogEntry **entry) const;
	ErrorType propagateLogEntry(const LogEntry &entry);
	ErrorType publishChanges(const LogEntry &entry);
	ErrorType replicateState(const LogEntry &entry, const EntryState::State state);
	static std::string printChange(const std::vector<Dependency> &deps, const std::vector<KeyValue> &updates);

public:
	friend class CoordinatorTest;	// since we want to test even private member methods of Coordinator in our unit tests
	Coordinator(primitive::coordinator_num_t coordinatorID, std::shared_ptr<RequestBuffer> requestBuffer);
	primitive::coordinator_num_t getID();
	ErrorType applyChange(const std::vector<Dependency> &deps, const std::vector<KeyValue> &updates, const TID tID, LogEntry **newEntry);
	ErrorType readLatest(const Key &key, Value &returnValue, LogEntry &headEntry);
	//ErrorType readByKey(const Key key, const SCN scn, const TID tid, Value &returnValue);
	ErrorType checkIfSerialized(const LogEntry &entry);
	ErrorType resolve(const size_t bucketID, LogEntry &headEntry);
	~Coordinator();
};

#endif /* AGENTS_COORDINATOR_COORDINATOR_HPP_ */
