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

typedef error::ErrorType ErrorType;


class Coordinator : public error::Throwable{
private:
	const primitive::coordinator_num_t coordinatorID_;
	primitive::generation_num_t generationNum_;
	primitive::offset_t freeBufferOffset_;
	std::vector<MemoryServerContext> memoryServerCtxs_;
	MemoryServerContext *localMemoryServerCtx_;

	ErrorType propagateLogEntry(LogEntry &entry);
	ErrorType createNewPointer(Change &change, Pointer *pointer);
	ErrorType makeNewLogEntry (Change &change, Pointer &entryPointer, LogEntry *entry) const;
	ErrorType publishChanges(LogEntry &entry);
	void errorHandler(const ErrorType eType);


public:
	Coordinator(primitive::coordinator_num_t coordinatorID);
	~Coordinator();
	ErrorType connectToMemoryServers(std::vector<MemoryServerContext> memoryServerCtxs);
	ErrorType applyChange(Change &change, TID tID);
	ErrorType readByKey(const Key key, const SCN scn, const TID tid, Value &returnValue);

};

#endif /* AGENTS_COORDINATOR_COORDINATOR_HPP_ */
