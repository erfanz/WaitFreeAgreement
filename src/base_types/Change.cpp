/*
 *  Change.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 25, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "Change.hpp"

Change::Change(std::vector<KeyValue> updates, std::vector<Dependency> dependencies)
{
	this->updates_ = updates;
	this->dependencies_ = dependencies;
}

Change::~Change() {
	// TODO Auto-generated destructor stub
}

const std::vector<Dependency>& Change::getDependencies() const {
	return dependencies_;
}

const std::vector<KeyValue>& Change::getUpdates() const {
	return updates_;
}
