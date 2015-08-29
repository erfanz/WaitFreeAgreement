/*
 *  Replica.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 11, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "Replica.hpp"
#include "../../util/utils.hpp"

#define CLASS_NAME	"Replica"

Replica::Replica()
: logs_(config::COORDINATOR_CNT) {
	resetMemoryRegions();
}

LocalRegionContext<uint64_t>* Replica::getHashRegion() {
	return hash_.region_;
}

LocalRegionContext<char>* Replica::getLogRegion(size_t index) {
	return logs_.at(index).region_;
}

void Replica::resetMemoryRegions() {
	hash_.resetRegion();

	for (size_t i = 0; i < config::COORDINATOR_CNT; i++)
		logs_[i].resetRegion();
}

Replica::~Replica() {
	DEBUG_COUT (CLASS_NAME, __func__, "Replica destroyed!");
}
