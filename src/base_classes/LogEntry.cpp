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

LogEntry::LogEntry(std::vector<Dependency> dependencies, std::vector<KeyValue> kvList, Pointer currentP, bool serialized){
	this->dependencies = dependencies;
	this->kvList = kvList;
	this->currentP = currentP;
	this->serialized = serialized;
}

void LogEntry::serialize(std::ostream& stream) const{
	/*
	 * - Pointer to the current log entry
	 * - isSerialized
	 * - DependencyCnt
	 * - Dependency_pucketID[DependencyCnt]
	 * - Dependency_pointer[DependencyCnt]
	 * - KVCnt
	 * - KeyValue[KVCnt]
	 * - Pointer
	 */
	std::string str = currentP.toString() + " ";
	str += utilities::ToString<bool>(isSerialized()) + " ";
	str += utilities::ToString<size_t>(dependencies.size()) + " ";

	for (size_t i=0; i < dependencies.size(); i++)
		str += utilities::ToString<int>(dependencies[i].getBucketID()) + " ";

	for (size_t i=0; i < dependencies.size(); i++)
			str += dependencies[i].getPointer().toString() + " ";

	str += utilities::ToString<size_t>(kvList.size()) + " ";

	for (size_t i=0; i < kvList.size(); i++)
			str += kvList[i].toString() + " ";

	str += currentP.toString();
	stream << str;
}

void LogEntry::doDeserialize(std::istream& stream, LogEntry &entry){
	std::string pointer_start, pointer_end;
	int dependencyCnt, kvCnt;

	/* - Pointer to the current log entry 	*/
	stream >> pointer_start;

	/* - isSerialized 						*/
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

	/* - KVCnt								*/
	stream >> kvCnt;

	/* - KeyValue[KVCnt]					*/
	Pointer::fromString(pointer_start, entry.currentP);
	entry.kvList = std::vector<KeyValue> (kvCnt);
	for (int i=0; i < kvCnt; i++){
		KeyValue::deserialize(stream, entry.kvList[i]);
	}

	/* - Pointer							*/
	stream >> pointer_end;
	if (pointer_end.compare(pointer_start) != 0) {
		std::cerr << "Pointers do not match" << std::endl;
		exit(1);
	}
}

std::vector<Dependency> LogEntry::getDependencies() const{
	return dependencies;
}

std::vector<KeyValue> LogEntry::getKVs() const {
	return kvList;
}

Pointer LogEntry::getCurrentP() const {
	return currentP;
}

bool LogEntry::isSerialized() const {
	return serialized;
}

bool LogEntry::isEqual(const LogEntry &entry) const {
	if (! currentP.isEqual(entry.getCurrentP())
			|| serialized != entry.isSerialized()
			|| kvList.size() != entry.getKVs().size()
			|| dependencies.size() != entry.getDependencies().size())
		return false;

	for (size_t i = 0; i < dependencies.size(); i++) {
		if (! dependencies.at(i).isEqual(entry.getDependencies().at(i)))
			return false;
	}

	for (size_t i = 0; i < kvList.size(); i++) {
		if (! kvList.at(i).isEqual(entry.getKVs().at(i)))
			return false;
	}
	return true;
}
