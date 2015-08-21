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

#define CLASS_NAME	"LogEntry"


LogEntry::LogEntry() {
	;
}

/**
 * Creates a new log entry
 *
 * Input:
 * 	- const vector<Dependency> &:	the list of all the dependencies for the log entry
 * 	- const vector<KeyValue> &:		the list of all the new kv updates
 * 	- const Pointer &:				the Saturn Pointer of the log entry (self pointer)
 * 	- const EntryState:state &:		the initial mutable state of the log entry (should be UNKNOWN)
 */
LogEntry::LogEntry(const std::vector<Dependency> &dependencies, const std::vector<KeyValue> &updates, const Pointer &currentP, const EntryState::State &state) {
	this->dependencies_ = dependencies;
	this->updates_ = updates;
	this->currentP_ = currentP;
	this->state_ = state;
}


/**
 * Overloaded "less than" operator, used for total ordering between two log entries.
 *
 * Input:
 * 	- const LogEntry &:	the log entry to be compared with.
 *
 * Returns:
 * 	- true if the current entry is smaller than the input entry. Otherwise false.
 */
bool LogEntry::operator< (const LogEntry &right) const {
	return currentP_ < right.getCurrentP();
}

/**
 * Overloaded "greater than" operator, used for total ordering between two log entries.
 *
 * Input:
 * 	- const LogEntry &:	the log entry to be compared with.
 *
 * Returns:
 * 	- true if the current entry is bigger than the input entry. Otherwise false.
 */
bool LogEntry::operator>(const LogEntry &right) const {
	// TODO: this should be changed
	return currentP_ > right.getCurrentP();
}


/**
 * Checks if the current entry is the same as the input entry
 *
 * Input:
 * 	- const LogEntry &:	the log entry to be compared with.
 *
 * Returns:
 * 	- true if the current entry is the same as the input entry. Otherwise false.
 */
bool LogEntry::isEqual(const LogEntry &entry) const {
	if (! currentP_.isEqual(entry.getCurrentP())
			|| state_ != entry.getState()
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


/**
 * Serializes the log entry object as an output stream of characters
 *
 * Input:
 * 	- std::ostream&:	the output stream to which the serialization will be written.
 *
 * Returns:
 * 	- nothing
 */
void LogEntry::serialize(std::ostream& stream) const{
	/*
	 * - (a) Pointer to the current log entry
	 * - (b) state
	 * - (c) DependencyCnt
	 * - (d) Dependency_bucketID[DependencyCnt]
	 * - (e) Dependency_pointer[DependencyCnt]
	 * - (f) UpdateCnt
	 * - (g) Updates[KVCnt]
	 * - (h) Pointer
	 */

	std::string str = currentP_.toString() + " ";
	str += utilities::ToString<int>(getState()) + " ";
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

void LogEntry::setState(EntryState::State state) {
	this->state_ = state;
}


/**
 * Deserializes the input stream of characters to a log entry
 *
 * Input:
 * 	- std::istream&:	the input stream from which the deserialization will be done.
 * 	- LogEntry &:		the resulted log entry
 *
 * Returns:
 * 	- nothing
 */
void LogEntry::doDeserialize(std::istream& stream, LogEntry &entry){
	std::string pointer_start, pointer_end;
	int dependencyCnt, updateCnt;

	/* - Pointer to the current log entry 	*/
	stream >> pointer_start;

	/* - state		 						*/
	unsigned int state;
	stream >> state;
	entry.state_ = static_cast<EntryState::State>(state);


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
		Pointer::fromBinaryString(p, dep_pointer);
		entry.dependencies_.at(i).setPointer(dep_pointer);
	}

	/* - UpdateCnt								*/
	stream >> updateCnt;

	/* - Updates[KVCnt]					*/
	Pointer::fromBinaryString(pointer_start, entry.currentP_);
	entry.updates_ = std::vector<KeyValue> (updateCnt);
	for (int i = 0; i < updateCnt; i++){
		KeyValue::deserialize(stream, entry.updates_[i]);
	}

	/* - Pointer							*/
	// must be careful here. We must only read POINTER_SIZE characters, and not more. Otherwise, we read garbage.
	//stream >> pointer_end;	// this turns out to be wrong as it reads garbage.
	char cstr[Pointer::getTotalSize() + 1];

	// first reading the extra space.
	stream.get(cstr, 2);	// 2 denotes that we read only one byte.

	stream.get(cstr, Pointer::getTotalSize() + 1);
	pointer_end = std::string(cstr);
	if (pointer_end.compare(pointer_start) != 0) {
		DEBUG_CERR(CLASS_NAME, __func__, "Top pointer (" << pointer_start << ") and Down pointer (" << pointer_end << ")  do not match!! ");
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


/**
 * Calculates the size of the serialized log entry which will be created using the input dependencies and updates in bytes.
 * This method is used to create the self pointer (which contains the entry size) of log entries even before they are constructed.
 *
 * Input:
 * 	- const vector<Dependency> &:	the list of dependencies of the log entry
 * 	- const vector<KeyValue> &:		the list of kv updates
 *
 * Returns:
 * 	- the size of the serialized entry
 */
primitive::entry_size_t LogEntry::calculateEntrySize(const std::vector<Dependency> &dependencies, const std::vector<KeyValue> &updates) {
	size_t size = 0;
	size +=  Pointer::getTotalSize() + 1;								// for (a) Pointer to the current log entry, and the space after

	size += 1 + 1;														// for (b) state, and the space after
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

EntryState::State LogEntry::getState() const {
	return state_;
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

void LogEntry::setToNull() {
	dependencies_.clear();
	updates_.clear();
	currentP_.setToNull();
	state_ = EntryState::State::SERIALIZED;
}
