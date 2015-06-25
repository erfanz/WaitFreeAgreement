/*
 *  MemoryServer.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 10, 2015
 *  Description : FILL IN THIS PART
 */

#ifndef AGENTS_MEMORY_SERVER_MEMORYSERVER_HPP_
#define AGENTS_MEMORY_SERVER_MEMORYSERVER_HPP_

class MemoryServer{
public:
	uint64_t keyValid[HASH_SIZE];
	uint64_t keyHash[HASH_SIZE];
	char journals[COORDINATOR_CNT][LOG_JOURNAL_SIZE];

	MemoryServer();
};


#endif /* AGENTS_MEMORY_SERVER_MEMORYSERVER_HPP_ */
