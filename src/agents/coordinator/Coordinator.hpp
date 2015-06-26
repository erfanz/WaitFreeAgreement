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
#include "../../errors/Error.hpp"		// to return error from functions
#include <vector>

typedef ERROR::ErrorType ErrorType;

class Coordinator {
private:
	const coordinator_num_t coordinatorID;
	generation_num_t generationNum;
	offset_t freeBufferOffset;
	std::vector<MemoryServerContext> memoryServerCtxs;
	const MemoryServerContext *localMemoryServerCtx;
	ErrorType propagateLogEntry(LogEntry &entry);
//	const MemoryServerContext* findLocalMemoryServer();
	ErrorType createNewPointer(Change &change, Pointer &pointer);
	ErrorType makeNewLogEntry (Change &change, Pointer &entryPointer, LogEntry &entry) const;
	ErrorType publishChanges(LogEntry &entry);

public:
	Coordinator(coordinator_num_t coordinatorID);
	~Coordinator();
	// int connectToMemoryServers(std::vector<MemoryServer> memoryServers);
	ErrorType applyChange(Change &change, TID tID);
	// int readByKey(const Key key, const SCN scn, const TID tid);

};

#endif /* AGENTS_COORDINATOR_COORDINATOR_HPP_ */
