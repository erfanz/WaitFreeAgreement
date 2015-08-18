/*
 *  HashBucket.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 11, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef HASHBUCKET_HPP_
#define HASHBUCKET_HPP_

#include "../../region/LocalRegionContext.hpp"
#include "../../config.hpp"
#include <stdint.h>
#include <atomic>


class HashBucket {
private:
	std::atomic<uint64_t>* content_;

public:
	LocalRegionContext<uint64_t> *region_;

	HashBucket();
	void resetRegion();
	~HashBucket();
};

#endif /* HASHBUCKET_HPP_ */
