/*
 *  Instance.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 11, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef INSTANCE_HPP_
#define INSTANCE_HPP_

#include "LogJournal.hpp"
#include "HashBucket.hpp"
#include "../../config.hpp"
#include <vector>
#include <memory>

class Instance {
private:
	std::vector<LogJournal> logs_;
	HashBucket hash_;

public:
	Instance();
	LocalRegionContext<uint64_t>* getHashRegion();
	LocalRegionContext<char>* getLogRegion(size_t index);
	void resetMemoryRegions();
	~Instance();
};

#endif /* INSTANCE_HPP_ */
