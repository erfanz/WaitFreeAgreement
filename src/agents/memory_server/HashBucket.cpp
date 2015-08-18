/*
 *  HashBucket.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 17, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "HashBucket.hpp"
#include "../../util/utils.hpp"

#define CLASS_NAME	"HashBucket"

HashBucket::HashBucket() {
	content_ = new std::atomic<uint64_t>[config::HASH_SIZE];
	region_ = new LocalRegionContext<uint64_t>(content_);
}

void HashBucket::resetRegion() {
	for (size_t i = 0; i < config::HASH_SIZE; i++)
		content_[i] = (uint64_t) 0;
}

HashBucket::~HashBucket() {
	delete[] content_;
	delete region_;
	DEBUG_COUT (CLASS_NAME, __func__, "HashBucket destroyed!");
}
