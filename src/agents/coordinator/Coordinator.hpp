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
#include "../../base_types/KeyValue.hpp"
#include <vector>

class Coordinator {
private:
	const uint8_t coordinatorID;
	uint8_t generationNum;
	uint32_t freeBufferOffset;
	std::vector<MemoryServerContext> memoryServerContexts;
	int writeLogEntry();
	int publishChanges();
	const LogEntry* constructUpdate(const Key, const Value) const;
	const MemoryServerContext* findLocalMemoryServer();
	const LogEntry* makeNewLogEntry (Key key, Value value) const;



public:
	Coordinator(uint8_t coordinatorID);
	~Coordinator();
	int connectToMemoryServers(std::vector<MemoryServer> memoryServers);
	int applyChange(std::vector<KeyValue> &updates, std::vector<KeyValue> &dependencies, TID tID);
	int readByKey(const Key key, const SCN scn, const TID tid);

};

#endif /* AGENTS_COORDINATOR_COORDINATOR_HPP_ */
