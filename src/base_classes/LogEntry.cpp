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

LogEntry::LogEntry(std::vector<KeyValue> kvList, Pointer currentP, bool serialized){
	this->kvList = kvList;
	this->currentP = currentP;
	this->serialized = serialized;
}

void LogEntry::serialize(std::ostream& stream) const{
	std::string str = currentP.toString() + " "
			+ utilities::ToString<bool>(isSerialized()) + " "
			+ utilities::ToString<size_t>(kvList.size()) + " ";

	for (size_t i=0; i < kvList.size(); i++)
		str += kvList[i].toString() + " ";

	str += currentP.toString();
	stream << str;
}

void LogEntry::doDeserialize(std::istream& stream, LogEntry &entry){
	std::string pointer_start;
	std::string pointer_end;

	int kvNum;
	stream >> pointer_start >> entry.serialized >> kvNum;

	Pointer::fromString(pointer_start, entry.currentP);
	entry.kvList = std::vector<KeyValue> (kvNum);
	for (int i=0; i < kvNum; i++){
		KeyValue::deserialize(stream, entry.kvList[i]);
	}

	stream >> pointer_end;
	if (pointer_end.compare(pointer_start) != 0) {
		std::cerr << "Pointers do not match" << std::endl;
		exit(1);
	}
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
	if (! currentP.isEqual(entry.getCurrentP()) || serialized != entry.isSerialized() || kvList.size() != entry.getKVs().size())
		return false;

	for (size_t i = 0; i < kvList.size(); i++) {
		if (! kvList.at(i).isEqual(entry.getKVs().at(i)))
			return false;
	}
	return true;
}
