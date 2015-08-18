/*
 *  Instance.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 11, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "Instance.hpp"
#include "../../util/utils.hpp"

#define CLASS_NAME	"Instance"

Instance::Instance()
: logs_(config::COORDINATOR_CNT) {
	resetMemoryRegions();
}

LocalRegionContext<uint64_t>* Instance::getHashRegion() {
	return hash_.region_;
}

LocalRegionContext<char>* Instance::getLogRegion(size_t index) {
	return logs_.at(index).region_;
}

void Instance::resetMemoryRegions() {
	hash_.resetRegion();

	for (size_t i = 0; i < config::COORDINATOR_CNT; i++)
		logs_[i].resetRegion();
}

Instance::~Instance() {
	DEBUG_COUT (CLASS_NAME, __func__, "Instance destroyed!");
}
