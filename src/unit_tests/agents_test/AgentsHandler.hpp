/*
 *  AgentsHandler.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jul 14, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef AGENTSHANDLER_HPP_
#define AGENTSHANDLER_HPP_

#include "../../agents/coordinator/Coordinator.hpp"
#include "../../request_buffer/RequestBuffer.hpp"
#include "../../agents/memory_server/MemoryRequestDispatcher.hpp"
#include "../../query_handler/AtomicUpdate.hpp"
#include "../../base_types/PrimitiveTypes.hpp"
#include "../../config.hpp"
#include <map>
#include <memory>


namespace agents_handler {
	extern MemoryRequestDispatcher *dispatcher;
	extern std::vector<Coordinator*> coordinators;
	extern std::vector<AtomicUpdate*> atomicUpdates;

	extern std::map<size_t,uint64_t> bucketToPointerMap;	// stores where each bucket points to during the execution of the test.
	extern std::map<std::string,std::string> keyToValueMap;	// stores the value associated with any given key.
	extern std::map<std::string,size_t> keyToBucketMap;	// stores the bucketID associated with any given key.

	void createCoordinators(std::vector<Coordinator*> &coordinators);
	void createAtomicUpdates(std::vector<AtomicUpdate*> &atomicUpdates);
	void setup();
	void resetMemoryRegions();
	void constructChange(Change **change, const std::vector<std::string> updateKeys, const std::vector<std::string> pureDependencies);
	void updateBucketInfo(const std::vector<std::string> &updateKeys, const std::vector<std::string> &pureDependencies, const Pointer &newEntryPointer);

}

#endif /* AGENTSHANDLER_HPP_ */
