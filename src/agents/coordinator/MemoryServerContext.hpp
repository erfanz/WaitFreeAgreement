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

#include "../../base_classes/LogEntry.hpp"
#include "../../base_types/HashMaker.hpp"
#include "../memory_server/MemoryServer.hpp"
#include "../../errors/Error.hpp"

class MemoryServerContext {
private:
	MemoryServer memoryServer;
	bool isLocal;
public:
	MemoryServerContext(MemoryServer memServer, bool isLocal);
	bool isKeyValid (const uint64_t, const Key &) const;
	int getLastMissedSCN(const HashMaker &hashedKey, SCN &outputScn) const;
	int findItemByVersion(HashMaker &, Key &, SCN &, TID &, LogEntry &) const;
	LogEntry* fetchLogEntry(uint8_t coordinatorNum, uint32_t offset, uint16_t length) const;
	int getKeyHash(HashMaker &, Pointer &p);

};


#endif /* AGENTS_COORDINATOR_MEMORYSERVERCONTEXT_HPP_ */
