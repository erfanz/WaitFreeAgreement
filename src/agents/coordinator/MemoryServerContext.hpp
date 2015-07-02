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
	LocalRegionContext* bucketHash_;
	LocalRegionContext* bucketValid_;
	LocalRegionContext** logJournals_;


public:
	MemoryServerContext(MemoryServer &memServer, bool isLocal);
//	bool isKeyValid (const uint64_t, const Key &) const;
//	int getLastMissedSCN(const HashMaker &hashedKey, SCN &outputScn) const;
//	int findItemByVersion(HashMaker &, Key &, SCN &, TID &, LogEntry &) const;
//	LogEntry* fetchLogEntry(uint8_t coordinatorNum, uint32_t offset, uint16_t length) const;
//	int getKeyHash(HashMaker &, Pointer &p);

	ErrorType writeLogEntry(const primitive::coordinator_num_t cID, const LogEntry &entry);
	ErrorType readLogEntry(const Pointer &pointer, LogEntry &entry);
	ErrorType readBucketHash(const HashMaker &, Pointer &);


};

#endif /* AGENTS_COORDINATOR_MEMORYSERVERCONTEXT_HPP_ */
