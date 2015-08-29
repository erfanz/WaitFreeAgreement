/*
 *  Replica.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 11, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef REPLICA_HPP_
#define REPLICA_HPP_

#include "LogJournal.hpp"
#include "HashBucket.hpp"
#include "../../config.hpp"
#include <vector>
#include <memory>

class Replica {
private:
	std::vector<LogJournal> logs_;
	HashBucket hash_;

public:
	Replica();
	LocalRegionContext<uint64_t>* getHashRegion();
	LocalRegionContext<char>* getLogRegion(size_t index);
	void resetMemoryRegions();
	~Replica();
};

#endif /* REPLICA_HPP_ */
