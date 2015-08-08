/*
 *  MemoryServerContext.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 11, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef AGENTS_COORDINATOR_MEMORYSERVERCONTEXT_HPP_
#define AGENTS_COORDINATOR_MEMORYSERVERCONTEXT_HPP_

#include "../../base_types/LogEntry.hpp"
#include "../../base_types/EntryState.hpp"
#include "../../region/LocalRegionContext.hpp"
#include "../memory_server/MemoryServer.hpp"
#include "../../base_types/HashMaker.hpp"
#include "../../errors/Error.hpp"
#include <future>         // std::promise, std::future


typedef error::ErrorType ErrorType;


class MemoryServerContext {
private:
	size_t memServerCtxID_;
	bool isLocal_;
	LocalRegionContext<uint64_t>* bucketHash_;
	LocalRegionContext<uint64_t>* bucketValid_;
	LocalRegionContext<char>** logJournals_;


public:
	friend class CoordinatorTest;	// since we want to test even private member methods of Coordinator in our unit tests
	MemoryServerContext(const size_t memServerCtxID, MemoryServer &memServer, bool isLocal);

	void writeLogEntry(const primitive::coordinator_num_t cID, const LogEntry &entry, std::promise<ErrorType> &errorProm);
	void readLogEntry(const Pointer &pointer, std::promise<ErrorType> &errorProm, LogEntry &entry) const;
	void readBucketHash(const size_t bucketID, std::promise<ErrorType> &errorProm, Pointer &pointer) const;
	void swapBucketHash(const size_t bucketID, const Pointer &expectedHead, const Pointer &newHead, std::promise<ErrorType> &errorProm, Pointer &actualHead);
	//ErrorType swapMultipleBucketHash(const std::vector<Dependency> &dependencies, const Pointer &newHead, std::vector<Pointer> &actualCurrentHeads);

	void markState(const LogEntry &entry, const EntryState::State state, std::promise<ErrorType> &errorProm);
	void checkState(const LogEntry &entry, std::promise<ErrorType> &errorProm, EntryState::State &state);
};

#endif /* AGENTS_COORDINATOR_MEMORYSERVERCONTEXT_HPP_ */
