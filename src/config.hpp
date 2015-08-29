/*
 *  config.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 10, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include <string>
#include <iostream>

#ifndef CONFIG_HPP_
#define CONFIG_HPP_



namespace config {
	#define DEBUG_ENABLED (true)

	static const int	MEMORY_SERVER_CNT	= 2;		// number of replicas
	static const int	COORDINATOR_CNT 	= 2;

	static const int	OPERATION_CNT 		= 10;		// number of changes by each coordinator (only for testing)

	static const int	KEY_CNT				= 10;		// Number of keys (i.e. |K|)
	static const int	LOG_JOURNAL_SIZE	= 200000;	// in bytes
	static const int	HASH_SIZE			= 2;		// number of buckets of the hash table

	static const int	REQUEST_QUEUE_SIZE	= 1000;		// the size of the request queue (called RequestBuffer in the code)

	static const int	TIMEOUT_IN_MS		= 500;		// timeout for memory requests
}

#endif /* CONFIG_HPP_ */
