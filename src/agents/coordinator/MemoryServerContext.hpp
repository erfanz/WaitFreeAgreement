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
#include "../../region/LocalRegionContext.hpp"
#include "../memory_server/MemoryServer.hpp"
#include "../../base_types/HashMaker.hpp"
#include "../../errors/Error.hpp"

typedef error::ErrorType ErrorType;


class MemoryServerContext {
private:
	bool isLocal_;
	LocalRegionContext<uint64_t>* bucketHash_;
	LocalRegionContext<uint64_t>* bucketValid_;
	LocalRegionContext<char>** logJournals_;


public:
	MemoryServerContext(MemoryServer &memServer, bool isLocal);
	ErrorType writeLogEntry(const primitive::coordinator_num_t cID, const LogEntry &entry);
	ErrorType readLogEntry(const Pointer &pointer, LogEntry &entry);
	ErrorType readBucketHash(const HashMaker &, Pointer &);
	ErrorType swapBucketHash(const size_t bucketID, const Pointer &expectedHead, const Pointer &newHead, Pointer &actualCurrentHead);
	ErrorType markSerialized(const primitive::coordinator_num_t cID, const LogEntry &entry);
};

#endif /* AGENTS_COORDINATOR_MEMORYSERVERCONTEXT_HPP_ */
