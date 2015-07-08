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
#include <atomic>

class MemoryServer{
private:
	std::atomic<uint64_t>* bucketValid_;
	std::atomic<uint64_t>* bucketHash_;
	std::atomic<char>** logJournals_;

public:
	MemoryServer();
	void getMemoryHandlers(std::atomic<uint64_t>** bucketValid, std::atomic<uint64_t>** bucketHash, std::atomic<char>*** logJournal);
	void resetMemoryBuffers();


	~MemoryServer();
};

#endif /* AGENTS_MEMORY_SERVER_MEMORYSERVER_HPP_ */
