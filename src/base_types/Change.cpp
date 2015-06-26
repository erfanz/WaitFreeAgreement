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
	this->updates = updates;
	this->dependencies = dependencies;
}

Change::~Change() {
	// TODO Auto-generated destructor stub
}

const std::vector<Dependency>& Change::getDependencies() const {
	return dependencies;
}

const std::vector<KeyValue>& Change::getUpdates() const {
	return updates;
}
