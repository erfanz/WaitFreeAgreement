/*
 *  MemoryServer.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 10, 2015
 *  Description : FILL IN THIS PART
 */

#ifndef AGENTS_MEMORY_SERVER_MEMORYSERVER_HPP_
#define AGENTS_MEMORY_SERVER_MEMORYSERVER_HPP_

#include <stdint.h>

class MemoryServer{
private:
	uint64_t* bucketValid_;
	uint64_t* bucketHash_;
	char** logJournals_;

public:
	MemoryServer();
	void getMemoryHandlers(uint64_t** bucketValid, uint64_t** bucketHash, char*** logJournal);

	~MemoryServer();
};

#endif /* AGENTS_MEMORY_SERVER_MEMORYSERVER_HPP_ */
