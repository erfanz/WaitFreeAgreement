/*
 * LogEntry.hpp
 *
 *  Created on: Jun 9, 2015
 *      Author: Erfan Zamanian
 */

#ifndef LOGENTRY_HPP_
#define LOGENTRY_HPP_

#include "../base_types/Serializable.hpp"
#include "../base_types/KeyValue.hpp"
#include "../base_types/Pointer.hpp"

#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <sstream>      // std::ostringstream, std::istringstream
#include <iostream>     // std::ios, std::istream, std::cout

class LogEntry : public Serializable<LogEntry>{
private:
	std::vector<KeyValue> kvList;
	Pointer currentP;
	bool serialized;


public:
	LogEntry();
	LogEntry(std::vector<KeyValue> kvList, Pointer currentP, bool serialized);
	Pointer getCurrentP() const;
	bool isSerialized() const;
	std::vector<KeyValue> getKVs() const;
	bool isEqual(const LogEntry &entry) const;
	virtual void serialize(std::ostream& stream) const;
	static void doDeserialize(std::istream&, LogEntry &);
};

#endif /* LOGENTRY_HPP_ */
