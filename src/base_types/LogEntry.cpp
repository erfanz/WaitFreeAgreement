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

LogEntry::LogEntry(){
	;
}

LogEntry::LogEntry(std::vector<Dependency> dependencies, std::vector<KeyValue> updates, Pointer currentP, bool serialized){
	this->dependencies = dependencies;
	this->updates = updates;
	this->currentP = currentP;
	this->serialized = serialized;
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
	std::string str = currentP.toString() + " ";
	str += utilities::ToString<bool>(isSerialized()) + " ";
	str += utilities::ToString<size_t>(dependencies.size()) + " ";

	for (size_t i=0; i < dependencies.size(); i++)
		str += utilities::ToString<int>(dependencies[i].getBucketID()) + " ";

	for (size_t i=0; i < dependencies.size(); i++)
		str += dependencies[i].getPointer().toString() + " ";

	str += utilities::ToString<size_t>(updates.size()) + " ";

	for (size_t i=0; i < updates.size(); i++)
		str += updates[i].toString() + " ";

	str += currentP.toString();
	stream << str;
}


void LogEntry::setDependencies(const std::vector<Dependency>& dependencies) {
	this->dependencies = dependencies;
}

void LogEntry::setUpdates(const std::vector<KeyValue>& updates) {
	this->updates = updates;
}

void LogEntry::setCurrentP(const Pointer& currentP) {
	this->currentP = currentP;
}

void LogEntry::setSerialized(bool serialized) {
	this->serialized = serialized;
}

void LogEntry::setAll(const std::vector<Dependency>& dependencies, const std::vector<KeyValue>& updates, const Pointer& currentP, bool serialized) {
	this->dependencies = dependencies;
	this->updates = updates;
	this->currentP = currentP;
	this->serialized = serialized;
}

void LogEntry::doDeserialize(std::istream& stream, LogEntry &entry){
	std::string pointer_start, pointer_end;
	int dependencyCnt, updateCnt;

	/* - Pointer to the current log entry 	*/
	stream >> pointer_start;

	/* - IsSerialized 						*/
	stream >> entry.serialized;

	/* - DependencyCnt						*/
	stream >> dependencyCnt;

	/* - Dependency_pucketID[DependencyCnt]
	   - Dependency_pointer[DependencyCnt] 	*/
	entry.dependencies = std::vector<Dependency> (dependencyCnt);
	int bid;
	for (int i=0; i < dependencyCnt; i++){
		stream >> bid;
		entry.dependencies.at(i).setBucketID(bid);
	}

	std::string p;
	for (int i=0; i < dependencyCnt; i++){
		stream >> p;
		Pointer dep_pointer;
		Pointer::fromString(p, dep_pointer);
		entry.dependencies.at(i).setPointer(dep_pointer);
	}

	/* - UpdateCnt								*/
	stream >> updateCnt;

	/* - Updates[KVCnt]					*/
	Pointer::fromString(pointer_start, entry.currentP);
	entry.updates = std::vector<KeyValue> (updateCnt);
	for (int i=0; i < updateCnt; i++){
		KeyValue::deserialize(stream, entry.updates[i]);
	}

	/* - Pointer							*/
	stream >> pointer_end;
	if (pointer_end.compare(pointer_start) != 0) {
		std::cerr << "Pointers do not match" << std::endl;
		exit(1);
	}
}

const std::vector<Dependency>& LogEntry::getDependencies() const{
	return dependencies;
}

const std::vector<KeyValue>& LogEntry::getUpdates() const {
	return updates;
}

const Pointer LogEntry::getCurrentP() const {
	return currentP;
}

entry_size_t LogEntry::calculateEntrySize(const std::vector<Dependency> &dependencies, const std::vector<KeyValue> &updates) {
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
	entry_size_t entrySize = entry_size_t (size);
	return entrySize;
}

bool LogEntry::isSerialized() const {
	return serialized;
}

bool LogEntry::isEqual(const LogEntry &entry) const {
	if (! currentP.isEqual(entry.getCurrentP())
			|| serialized != entry.isSerialized()
			|| updates.size() != entry.getUpdates().size()
			|| dependencies.size() != entry.getDependencies().size())
		return false;

	for (size_t i = 0; i < dependencies.size(); i++) {
		if (! dependencies.at(i).isEqual(entry.getDependencies().at(i)))
			return false;
	}

	for (size_t i = 0; i < updates.size(); i++) {
		if (! updates.at(i).isEqual(entry.getUpdates().at(i)))
			return false;
	}
	return true;
}
