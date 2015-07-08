/*
 *  Change.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 25, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "Change.hpp"
#include "../util/utils.hpp"

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

const std::string Change::toString() const {
	std::string str = "";
	str += "Updates: \"";
	for (size_t i = 0; i < updates_.size(); i++){
		str += updates_.at(i).toString();
		str += ", ";
	}

	str += "\", Dependencies: \"";

	for (size_t i = 0; i < dependencies_.size(); i++){
		str += utilities::ToString<size_t>(dependencies_.at(i).getBucketID()) + "(" + dependencies_.at(i).getPointer().toHexString() + ")";
		str += ", ";
	}
	str += "\"";
	return str;
}
