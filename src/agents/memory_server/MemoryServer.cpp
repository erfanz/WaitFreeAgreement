/*
 *  MemoryServer.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 10, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "MemoryServer.hpp"
#include "../../config.hpp"


MemoryServer::MemoryServer(){
	bucketValid_	= new std::atomic<uint64_t>[config::HASH_SIZE];
	bucketHash_		= new std::atomic<uint64_t>[config::HASH_SIZE];
	logJournals_	= new std::atomic<char>*[config::COORDINATOR_CNT];

	resetMemoryBuffers();
}

void MemoryServer::getMemoryHandlers(std::atomic<uint64_t>** bucketValid  __attribute__ ((unused)),
		std::atomic<uint64_t>**  bucketHash __attribute__ ((unused)),
		std::atomic<char>*** logJournal __attribute__ ((unused)) ) {

	*bucketValid = this->bucketValid_;
	*bucketHash	= this->bucketHash_;
	*logJournal	= this->logJournals_;
}

void MemoryServer::resetMemoryBuffers() {
	// initializing the hashBuffer
	for (size_t i = 0; i < config::HASH_SIZE; i++) {
		bucketValid_[i] = (uint64_t) 0;
		bucketHash_[i] = (uint64_t) 0;
	}

	// initializing the journals
	for (size_t i = 0; i < config::COORDINATOR_CNT; i++) {
		logJournals_[i] = new std::atomic<char>[config::LOG_JOURNAL_SIZE];
		for (size_t j = 0; j < config::LOG_JOURNAL_SIZE; j++)
			logJournals_[i][j] = 0;
	}
}

MemoryServer::~MemoryServer(){
	delete[] bucketValid_;
	delete[] bucketHash_;

	for (size_t i = 0; i < config::COORDINATOR_CNT; i++) {
		delete [] logJournals_[i];
	}
	delete [] logJournals_;
}
