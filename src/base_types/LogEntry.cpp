/*
 *  LogEntry.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 17, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "LogEntry.hpp"
#include "../util/utils.hpp"	// for toString()
#include <string>
#include <stdint.h>
#include <iomanip>      // std::setw
#include <stdlib.h>     /* exit, EXIT_FAILURE */

LogEntry::LogEntry() {
	;
}
LogEntry::LogEntry(std::vector<Dependency> dependencies, std::vector<KeyValue> updates, Pointer currentP, bool serialized){
	this->dependencies_ = dependencies;
	this->updates_ = updates;
	this->currentP_ = currentP;
	this->serialized_ = serialized;
}

void LogEntry::serialize(std::ostream& stream) const{
	/*
	 * - (a) Pointer to the current log entry
	 * - (b) IsSerialized
	 * - (c) DependencyCnt
	 * - (d) Dependency_bucketID[DependencyCnt]
	 * - (e) Dependency_pointer[DependencyCnt]
	 * - (f) UpdateCnt
	 * - (g) Updates[KVCnt]
	 * - (h) Pointer
	 */

	std::string str = currentP_.toString() + " ";
	str += utilities::ToString<bool>(isSerialized()) + " ";
	str += utilities::ToString<size_t>(dependencies_.size()) + " ";

	for (size_t i=0; i < dependencies_.size(); i++)
		str += utilities::ToString<size_t>(dependencies_[i].getBucketID()) + " ";

	for (size_t i=0; i < dependencies_.size(); i++)
		str += dependencies_[i].getPointer().toString() + " ";

	str += utilities::ToString<size_t>(updates_.size()) + " ";

	for (size_t i=0; i < updates_.size(); i++)
		str += updates_[i].toString() + " ";

	str += currentP_.toString();
	stream << str;
}


void LogEntry::setDependencies(const std::vector<Dependency>& dependencies) {
	this->dependencies_ = dependencies;
}

void LogEntry::setUpdates(const std::vector<KeyValue>& updates) {
	this->updates_ = updates;
}

void LogEntry::setCurrentP(const Pointer& currentP) {
	this->currentP_ = currentP;
}

void LogEntry::setSerialized(bool serialized) {
	this->serialized_ = serialized;
}

void LogEntry::doDeserialize(std::istream& stream, LogEntry &entry){
	std::string pointer_start, pointer_end;
	int dependencyCnt, updateCnt;

	/* - Pointer to the current log entry 	*/
	stream >> pointer_start;

	/* - IsSerialized 						*/
	stream >> entry.serialized_;

	/* - DependencyCnt						*/
	stream >> dependencyCnt;

	/* - Dependency_pucketID[DependencyCnt]
	   - Dependency_pointer[DependencyCnt] 	*/
	entry.dependencies_ = std::vector<Dependency> (dependencyCnt);
	size_t bid;
	for (int i = 0; i < dependencyCnt; i++){
		stream >> bid;
		entry.dependencies_.at(i).setBucketID(bid);
	}

	std::string p;
	for (int i = 0; i < dependencyCnt; i++){
		stream >> p;
		Pointer dep_pointer;
		Pointer::fromString(p, dep_pointer);
		entry.dependencies_.at(i).setPointer(dep_pointer);
	}

	/* - UpdateCnt								*/
	stream >> updateCnt;

	/* - Updates[KVCnt]					*/
	Pointer::fromString(pointer_start, entry.currentP_);
	entry.updates_ = std::vector<KeyValue> (updateCnt);
	for (int i = 0; i < updateCnt; i++){
		KeyValue::deserialize(stream, entry.updates_[i]);
	}

	/* - Pointer							*/
	stream >> pointer_end;
	if (pointer_end.compare(pointer_start) != 0) {
		std::cerr << "Pointers do not match" << std::endl;
		exit(1);
	}
}

const std::vector<Dependency>& LogEntry::getDependencies() const{
	return dependencies_;
}

const std::vector<KeyValue>& LogEntry::getUpdates() const {
	return updates_;
}

const Pointer LogEntry::getCurrentP() const {
	return currentP_;
}

primitive::entry_size_t LogEntry::calculateEntrySize(const std::vector<Dependency> &dependencies, const std::vector<KeyValue> &updates) {
	size_t size = 0;
	size +=  Pointer::getTotalSize() + 1;								// for (a) Pointer to the current log entry, and the space after

	size += 1 + 1;														// for (b) IsSerialized, and the space after
	size += utilities::numDigits(int (dependencies.size())) + 1;		// for (c) dependencyCnt, and the space after

	for (size_t i = 0; i < dependencies.size(); i++) {
		size += utilities::numDigits(int (dependencies.at(i).getBucketID())) + 1;	// for (d) Dependency_bucketID[], and the space after
		size += Pointer::getTotalSize() + 1;										// for (e) Dependency_pointer[], and the space after
	}

	size += utilities::numDigits(int (updates.size())) + 1;				// for (f) kvNum, and the space after

	for (size_t i = 0; i < updates.size(); i++)
		size += updates.at(i).getTotalSize() + 1;						// for (g) Updates[] and the space after

	size += Pointer::getTotalSize();
	primitive::entry_size_t entrySize = primitive::entry_size_t (size);
	return entrySize;
}

bool LogEntry::isSerialized() const {
	return serialized_;
}

bool LogEntry::isEqual(const LogEntry &entry) const {
	if (! currentP_.isEqual(entry.getCurrentP())
			|| serialized_ != entry.isSerialized()
			|| updates_.size() != entry.getUpdates().size()
			|| dependencies_.size() != entry.getDependencies().size())
		return false;

	for (size_t i = 0; i < dependencies_.size(); i++) {
		if (! dependencies_.at(i).isEqual(entry.getDependencies().at(i)))
			return false;
	}

	for (size_t i = 0; i < updates_.size(); i++) {
		if (! updates_.at(i).isEqual(entry.getUpdates().at(i)))
			return false;
	}
	return true;
}

bool LogEntry::getUpdateIfExists(const Key &key, Value &value) const {
	for (size_t i = 0; i < updates_.size(); i++) {
		if (updates_.at(i).getKey().isEqual(key)) {
			value = updates_.at(i).getValue();
			return true;
		}
	}
	return false;
}

bool LogEntry::getDependencyIfExists(const size_t bucketID, Pointer &pointer) const {
	for (size_t i = 0; i < dependencies_.size(); i++) {
		if (dependencies_.at(i).getBucketID() == bucketID) {
			pointer = dependencies_.at(i).getPointer();
			return true;
		}
	}
	return false;
}
